# firduino - Arduino FIR Library using ESP-DSP FIR Assembler

This library uses the [Arduino Audio Tools](https://github.com/pschatzmann/arduino-audio-tools) library and implements a BaseConverter subclass that accepts convolution coefficients for left and right channel.

The convolution algoritm is implemented using the [dsps_fir_f32_ae32.S](https://github.com/espressif/esp-dsp/blob/master/modules/fir/float/dsps_fir_f32_ae32.S) from the [esp-dsp](https://github.com/espressif/esp-dsp) library

This implementation is quite efficient and allows (at least) up to 501 taps at a sampling frequency of 44.1kHz

For an example implementation see [Radio]{https://github.com/thaaraak/ESP32-A1S-Tayloe/tree/master/Radio) and the simpler [FIR](https://github.com/thaaraak/ESP32-A1S-Tayloe/tree/master/streams-i2s-fir-i2s)

Note this library only works on the ESP32 platform
