# TeenyGPSTestbed_C2

TeenyGPSTestbed_C2 is an application for testing Ublox GPS modules which runs on the M5Stack Core2 ESP32 IoT Development Kit.
This application was developed to test Beitian BN GPS modules connected to the Core2's Serial2 interface.  This application also has a GPS emulator which can be used for testing navigation applications when no GPS signals can be acquired or when a fixed, repeatable GPS source is desired.
TeenyGPSTestbed_C2 utilizes the TeenyMenu library for menu/page navigation.  The TeenyMenu library was derived from the GEM (Good Enough Menu) Arduino library (https://github.com/Spirik/GEM.git) back in August of 2021 for use with the Adafruit 128x64 OLED Feather with its three button interface.

Navigating the TeenyGPSTestbed_C2 menu pages on the Core2 utilizes the three touch buttons on the Core2 adjacent to the display as follows:
- The middle button single touch is SELECT.
- If the menu item you selected is a MENU LINK item (with '>' on the right), it switches to the linked menu.  Middle button double touch returns you to the previous menu.
- If the menu item you selected is a PARAMETER ITEM (name:value) the item switches to edit mode (with '+/-' on the right).  You can then scroll up and down to change the value.  While in edit mode, middle button single touch will save the selected value and exit edit mode.  Middle button double touch will exit edit mode without saving.
- If the menu item you selected is a BOOLEAN ITEM (parameter:value with values ON/OFF, YES/NO, TRUE/FALSE, or any defined label pair), select will toggle the value.
- If the menu item you selected is an ACTION ITEM (with '>>' on the left side), the action (function call) is executed.
- Some menus also have an optional EXIT ITEM ('<exit') which returns to the previous menu just like middle button double touch does.

TeenyGPSTestbed_C2 offers four modes of operation:

GPSRCV: Basic GPS Receiver - Displays PVT packet info including date/time, lat/lon/alt, heading, and positional accuracy data.
![PXL_20240303_170936265](https://github.com/BeakeS/TeenyGPSTestbed_C2/assets/27782001/4e2b1242-d968-42f4-b2d3-a8b80f035e75)

GPSLOG: NAV-PVT Packet Logger - For logging PVT packets for analysis or emulation.
![PXL_20240303_171040320](https://github.com/BeakeS/TeenyGPSTestbed_C2/assets/27782001/937f2e26-c376-4e52-9304-112ede750657)

GPSSAT: NAV-SAT Receiver - For testing satellite reception.  Displays a list of tracked satellites sorted by signal strength.
![PXL_20240303_171059576](https://github.com/BeakeS/TeenyGPSTestbed_C2/assets/27782001/e15d2d89-31d1-480e-90a5-53cbbd6d3d68)

GPSEMU: GPS Emulation - Emulates Beitian BN-880Q GPS module including replay of PVT packets from ROM or SDCARD with progressive date/time updates.
![PXL_20240229_005625834](https://github.com/BeakeS/TeenyGPSTestbed_C2/assets/27782001/f4011f04-dae2-459f-9b68-648e4ca00ba4)
