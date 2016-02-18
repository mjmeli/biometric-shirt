## RFduino

This folder contains example and project code for the RFduino system, plus user
guides and other important content.

### Setup
##### Driver Setup
USB Serial Drivers for RFduino: http://www.ftdichip.com/Drivers/VCP.htm

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

##### Library Setup
In the `libraries` folder you will see some directories. These should be copied
to the `libraries` folder in the Arduino IDE install directory
(i.e. `C:\Program Files (x86)\Arduino\libraries`).

**If you wish to do any development on the libraries**, consider creating a
symlink instead of copying the files directory. See the symlink section.

OneWire Library: http://www.pjrc.com/teensy/td_libs_OneWire.html

### symlink
If you want to make changes to the AD5933 library, then follow these instructions.
As the library has to be installed to the Arduino IDE `libraries` folder, this
folder isn't under source control. In order to have it under source control,
I recommend making a symlink. The symlink can be either **TO** or **FROM** the
Arduino directory.

In the following examples, I symlink the directory from the git directory to
the Arduino directory.

On Windows:

    mklink /D "d:\Libraries\Documents\Arduino\libraries\AD5933" "d:\Libraries\Documents\GitHub\biometric-shirt\rfduino\libraries\AD5933"

### Sketches

##### bluetooth-test
Simply sends a string over Bluetooth once every 5 seconds. This is useful to
test that the RFduino is correctly sending data to the receiver.

##### temperature-test
Reads the temperature from a temperature sensor and transmits over serial.

**Pay attention to the connections in the comment at the top of the file.**

##### fall-design-day-demo
Reads the temperature from the temperature sensors and transmits it over
Bluetooth once every second.

##### ad5933-test
Tests taking impedance measurements from the AD5933.
