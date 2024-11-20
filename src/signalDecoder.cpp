/*
  rtl_433_Decoder_ESP - 433.92 MHz protocols library for ESP32
    based on rtl_433_ESP

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 3 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with library. If not, see <http://www.gnu.org/licenses/>


  Project Structure

  rtl_433_Decoder - Main Class
  signalDecoder.cpp - Wrapper and interface for the rtl_433 classes
  rtl_433 - subset of rtl_433 package

*/

#include "signalDecoder.h"

void rtl_433_Decoder::rtlSetup() {
  r_cfg_t* cfg = &g_cfg;
  int rtl_433_Decoder_Stack=11500;

  if (!cfg->demod) {
    r_init_cfg(cfg);

    cfg->conversion_mode = CONVERT_SI; // Default all output to Celsius
    if (_ookModulation) {
      cfg->num_r_devices = NUMOF_OOK_DEVICES;
    } else {
      cfg->num_r_devices = NUMOF_FSK_DEVICES;
      rtl_433_Decoder_Stack=20000;
    }
    cfg->devices = (r_device*)calloc(cfg->num_r_devices, sizeof(r_device));

    if (!cfg->devices) {
      FATAL_CALLOC("cfg->devices");
    }

    if (_ookModulation) {
      #include "ook_devices.stub"
    } else {
      #include "fsk_devices.stub"
    }

    cfg->verbosity = rtlVerbose; // 0=normal, 1=verbose, 2=verbose decoders,

    // expand register_all_protocols to determine heap impact from each decoder
    // register_all_protocols(cfg, 0);

    for (int i = 0; i < cfg->num_r_devices; i++) {
      // register all device protocols that are not disabled
      cfg->devices[i].protocol_num = i;

      char* arg = NULL;
      if (cfg->devices[i].disabled <= 0) {
        register_protocol(cfg, &cfg->devices[i], arg);
      }
    }

    rtl_433_Queue = xQueueCreate(5, sizeof(pulse_data_t*));

    xTaskCreatePinnedToCore(
        this->rtl_433_DecoderTask, /* Function to implement the task */
        "rtl_433_DecoderTask", /* Name of the task */
        rtl_433_Decoder_Stack, /* Stack size in bytes */
        this, /* Task input parameter */
        rtl_433_Decoder_Priority, /* Priority of the task (set lower than core task) */
        &rtl_433_DecoderHandle, /* Task handle. */
        rtl_433_Decoder_Core); /* Core where the task should run */
  }  
}

void rtl_433_Decoder::setCallback(rtl_433_ESPCallBack callback, char* messageBuffer,
                  int bufferSize) {
  // logprintfLn(LOG_DEBUG, "_setCallback location: %p", callback);

  r_cfg_t* cfg = &g_cfg;

  cfg->callback = callback;
  cfg->messageBuffer = messageBuffer;
  cfg->bufferSize = bufferSize;
}

// ---------------------------------------------------------------------------------------------------------

void rtl_433_Decoder::rtl_433_DecoderTask(void* pvParameters) {
  rtl_433_Decoder* thistask= (rtl_433_Decoder *) pvParameters; 
  r_cfg_t* cfg = &thistask->g_cfg;
  decode_job_t* job;

  for (;;) {
//    logprintfLn(LOG_DEBUG, "rtl_433_DecoderTask awaiting signal");
    xQueueReceive(thistask->rtl_433_Queue, &job, portMAX_DELAY);
    // logprintfLn(LOG_DEBUG, "rtl_433_DecoderTask signal received");

    job->rtl_pulses->sample_rate = 1.0e6;
    cfg->ctx=job->ctx;
    int events = 0;

    if (thistask->_ookModulation) {
      events = run_ook_demods(&cfg->demod->r_devs, job->rtl_pulses);
    } else {
      events = run_fsk_demods(&cfg->demod->r_devs, job->rtl_pulses);
    }

    if (events == 0) {
      thistask->unparsedSignals++;
    }

    free(job->rtl_pulses);
    free(job);

  }
}

void rtl_433_Decoder::processSignal(pulse_data_t* rtl_pulses,void* ctx) {
    decode_job_t *job=(decode_job_t*) malloc (sizeof(decode_job_t));
    job->rtl_pulses=rtl_pulses;
    job->ctx=ctx;

  // logprintfLn(LOG_DEBUG, "processSignal() about to place signal on
  // rtl_433_Queue");
  if (xQueueSend(rtl_433_Queue, &job, 0) != pdTRUE) {
    logprintfLn(LOG_ERR, "ERROR: rtl_433_Queue full, discarding signal");
    free(rtl_pulses);
    free(job);
  } else {
    //logprintfLn(LOG_DEBUG, "processSignal() signal placed on rtl_433_Queue");
  }
}

void rtl_433_Decoder::processRaw(std::vector<int> &rawdata,void* ctx) {
  pulse_data_t* rtl_pulses = (pulse_data_t*)heap_caps_calloc(1, sizeof(pulse_data_t), MALLOC_CAP_INTERNAL);
  *rtl_pulses = (pulse_data_t const){0};
  // rtl_pulses->sample_rate = 1000000; // us --- already set in modified api
  int maxsize = sizeof(rtl_pulses->pulse) / sizeof(*rtl_pulses->pulse);
  int rawcount=rawdata.size();
  int i=0;
  int j=0;
  while ((i<maxsize)&&(j<rawcount)) {
    if (rawdata[j]>0) {
      rtl_pulses->pulse[i] = rawdata[j++];
      rtl_pulses->gap[i] = (j<rawcount) && (rawdata[j]<0) ? -rawdata[j++] : 10000;
      ++i;
    } else ++j;
  }

  rtl_pulses->num_pulses=i;

  processSignal(rtl_pulses,ctx);

}

