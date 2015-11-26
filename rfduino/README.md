## RFduino

This folder contains example and project code for the RFduino system, plus user
guides and other important content.

### Setup
The following steps are modified from the RFduino Quick Start Guide (pdf) and
the [RFduino Readme](https://github.com/RFduino/RFduino/blob/master/README.md).

##### Driver Setup
USB Serial Drivers for RFduino: http://www.ftdichip.com/Drivers/VCP.htm

    NOTE: I had to install these drivers twice to get everything to work.

Bluetooth Drivers for BLE Module: http://www.inateck.com/download/inateck-bta-csr4b6-usb-bluetooth-4-0-adapter/

##### IDE Setup
1. Download the latest Arduino IDE: https://www.arduino.cc/en/Main/Software
    * If you need to drop to 1.6.1 or below, see [this link for fixing the compiler version](  http://openbci.com/forum/index.php?p=/discussion/259/missing-compiler-when-re-programming-the-rfduinos-dongle-and-openbci-device)
2. Open the Arduino IDE
3. Go to `Edit > Preferences` and add `http://rfduino.com/package_rfduino_index.json`
to Additional Board Manager URLs, then click OK and restart the IDE
4. Open the Boards Manager under `Tools > Boards`
5. Scroll down until you find the RFduino package and click Install
6. Select the RFduino board from `Tools > Boards`

You can test that the IDE is setup properly by compiling code. You can find sample
sketches in `File > Examples > RFduinoBLE`.

##### Temperature Sensor Setup

You need to install the supporting OneWire library to the `libraries` folder in
the Arduino IDE install directory (i.e. `C:\Program Files (x86)\Arduino\libraries`).

OneWire Library: http://www.pjrc.com/teensy/td_libs_OneWire.html

### Sketches

##### bluetooth-test
Simply sends a string over Bluetooth once every 5 seconds. This is useful to
test that the RFduino is correctly sending data to the receiver.

##### temperature-test
Reads the temperature from the temperature sensors and transmits over serial.

**Pay attention to the connections in the comment at the top of the file.**
