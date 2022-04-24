# Arduino came reader

[![Compile Sketch](https://github.com/jehy/arduino-came-reader/actions/workflows/compile-sketch.yaml/badge.svg)](https://github.com/jehy/arduino-came-reader/actions/workflows/compile-sketch.yaml)
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.me/jehyrus)

Arduino 433 Mhz reader for CAME gates, based on [this sketch](https://gist.github.com/superyarik/3eb4da9da728466c072e716532d732ef).

It can read both 10 and 20 bit keys.

Just connect any 433Mhz receiver, specify port, launch sketch, push button on your remote and get your key in binary.

If you wanna open gates using received code, use [this sketch](https://github.com/jehy/arduino-came-gates-mqtt).

Please note that this surely works on Arduino UNO, somehow does not work on Wemos D1 R1 and I'm not sure if it works on other controllers.
