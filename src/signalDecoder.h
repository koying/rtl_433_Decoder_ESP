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

#ifndef rtl_433_DECODER_H
#define rtl_433_DECODER_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

// Decoder task settings
#define rtl_433_Decoder_Priority 2
#define rtl_433_Decoder_Core     1

#include <cstring>
#include <vector>

extern "C" {
#include "bitbuffer.h"
#include "fatal.h"
#include "list.h"
#include "pulse_detect.h"
#include "r_api.h"
#include "r_private.h"
#include "rtl_433.h"
#include "rtl_433_devices.h"
#include "rfraw.h"

}

#include "log.h"

/*----------------------------- functions -----------------------------*/

typedef void (*rtl_433_ESPCallBack)(char* message, void* ctx);

typedef struct decode_job {
  pulse_data_t* rtl_pulses;
  void* ctx;
} decode_job_t;

class rtl_433_Decoder {
public:
  // construct
  rtl_433_Decoder(bool ookModulation = true) : _ookModulation(ookModulation) {
    memset(&g_cfg, 0, sizeof(r_cfg_t));
  }

  // call this to setup the decoder
  void rtlSetup();
  /// @brief Set callback function for receiving decoded messages
  /// @param callback Pointer to callback function
  // The callback function should be of type:  void (*callback)(char *message, void *ctx);
  //   where message will point to the json formatted string of the decoded message.  You *must* free() this
  //   when you are done with your own processing of it!  ctx is a context pointer you can optionally 
  //   set via the processRaw method.
  void setCallback(rtl_433_ESPCallBack callback);
  // process rtl_433 format pulse_data_t pulses
  void processSignal(pulse_data_t* rtl_pulses,void* ctx=nullptr);
  /// @brief Process raw format data.
  /// @param rawdata Vector of on/mark (positive integer microseconds) and off/space (negative integer microseconds)
  /// @param ctx Optional context pointer for callback
  void processRaw(std::vector<int32_t>& rawdata,void* ctx=nullptr);
  /// @brief Process RF raw format data.
  /// @param p Pointer to RFraw null-term string data
  /// @param ctx Optional context pointer for callback
  void processRFRaw(char const *p,void* ctx=nullptr);
  /// @brief set modululation to ook
  /// @param ook true=ook, false=fsk
  void setook(bool ook) { _ookModulation=ook; }
  unsigned int unparsedSignals = 0;

  r_cfg_t g_cfg; // Global config object

protected:
  static void rtl_433_DecoderTask(void* pvParameters);

private:
  bool _ookModulation = true;

  int rtlVerbose = 0;

  TaskHandle_t rtl_433_DecoderHandle;
  QueueHandle_t rtl_433_Queue;
};

#endif
