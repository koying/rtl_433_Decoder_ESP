This library enables [rtl_433](https://github.com/merbanan/rtl_433) signal decoding on ESP32 devices.  It is heavily based on the work of [rtl_433_ESP](https://github.com/NorthernMan54/rtl_433_ESP), but with the goal of just being a decoder library without dependencies on receiver hardware or MQTT output clients.

# Library Usage
example link WIP

## Recommendations


# More Advanced Usage and Notes
## Compile definition options
- MY_RTL433_DEVICES - allows compiling only a subset of decoders.  This could be desirable in order to help reduce memory and cpu overhead.  Example: ```-DMY_RTL433_DEVICES="DECL(govee_h5054) DECL(lacrosse_tx141x) "```

## Porting approach
See: [tools/update_rtl433.py](https://github.com/juanboro/rtl_433_Decoder_ESP/blob/main/tools/update_rtl433.py)

# Todo
* [] Allow reporting the device enabled/disabled list
* [] Allow enabling disabled devices
* [] Init class by default to allow OOK and FSK, and then control FSK/OOK on per signal basis (multiple receiver support)
* [] Re-enable _some_ of the debugging from rtl_433_ESP
