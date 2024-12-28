This library enables [rtl_433](https://github.com/merbanan/rtl_433) signal decoding on ESP32 devices.  It is heavily based on the work of [rtl_433_ESP](https://github.com/NorthernMan54/rtl_433_ESP), but with the goal of just being a decoder library without dependencies on receiver hardware or MQTT output clients.

# Library Usage
## gist...
```cpp
void process_callback(char *msg, void *ctx) {
  // msg points to a null terminated string in JSON format of the decoded data

  // ctx points to whatever you set it to when calling processRaw. 

  // free() msg when you are done with it!
  free(msg);
}

// call processRaw with a vector of on(+)/off(-) integer pulses
void recv_raw(rtl_433_Decoder &rd,std::vector<int> &rawdata) {
  rd.processRaw(rawdata, nullptr);
}

// Setup decoder with callback
void init() {
  rtl_433_Decoder rd;
  rd.setCallback(&process_callback);
  rd.rtlSetup();
}
```

An example used for a ESPHome component can be found [here](https://github.com/juanboro/esphome-rtl_433-decoder/blob/main/components/rtl_433/rtl_433.cpp)

## Recommendations
The main goal here is for being able to decode known devices, not as a signal analysis tool.  I'd recommend using rtl_sdr on a PC with a SDR (as well as other tools) to determine/test specific signals, and then configure this once working devices are determined for rtl_433.

# More Advanced Usage and Notes
## Compile definition options
- MY_RTL433_DEVICES - allows compiling only a subset of decoders.  This could be desirable in order to help reduce memory and cpu overhead.  Example: ```-DMY_RTL433_DEVICES="DECL(govee_h5054) DECL(lacrosse_tx141x) "```

## Porting approach
See: [tools/update_rtl433.py](https://github.com/juanboro/rtl_433_Decoder_ESP/blob/main/tools/update_rtl433.py)

# Todo
* [X] Allow reporting the device enabled/disabled list 
* [X] Allow enabling disabled devices, and disable enabled
* [X] Init class by default to allow OOK and FSK, and then control FSK/OOK on per signal basis (multiple receiver support)
* [ ] Actually test with real FSK signals.
* [ ] Re-enable _some_ of the debugging from rtl_433_ESP
For the first 3 above - see basic esphome [example](https://github.com/juanboro/esphome-rtl_433-decoder/blob/main/rtl_433_protocols.yaml)
