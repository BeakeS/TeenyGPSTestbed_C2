# TeenyGPSTestbed_C2

TeenyGPSTestbed_C2 is an application for testing Ublox GPS modules.  This application runs on the M5Stack Core2 ESP32 IoT Development Kit.
The application was developed to test Beitian Ublox GPS modules connected to the Core2's Serial2 interface.  The application also has a GPS emulation mode which can be used for testing navigation applications when no GPS signals can be acquired or when a fixed, repeatable GPS source is desired.
TeenyGPSTestbed_C2 utilizes the TeenyMenu library for menu/page navigation via a three button interface.  The TeenyMenu library was derived from the GEM (Good Enough Menu) Arduino library back in 2021 to use with the Adafruit 128x64 OLED Feather (and its three button interface).

Navigating the TeenyGPSTestbed_C2 menu pages on the Core2 utilizes the Core2's three touch buttons adjacent to the display as follows:
- The middle button single touch is SELECT.
- If the menu item you selected is a MENU LINK item (with '>' on the right), it switches to the linked menu page.  Middle button double touch returns you to the previous menu page.
- If the menu item you selected is a PARAMETER ITEM (name:value) the item switches to edit mode (with '+/-' on the right).  You can then scroll up and down to change the value.  While in edit mode, middle button single touch will save the selected value and exit edit mode.  Middle button double touch will exit edit mode without saving.
- If the menu item you selected is a BOOLEAN ITEM (parameter:value with values ON/OFF, YES/NO, TRUE/FALSE, or any defined label pair), select will toggle the value.
- If the menu item you selected is an ACTION ITEM (with '>>' on the left side), the action (function call) is executed.
- Some menus also have an optional EXIT ITEM ('<exit') which returns to the previous menu page just like middle button double touch does.

TeenyGPSTestbed_C2 offers five modes of operation:

GPSRCV: Basic GPS Receiver - Displays PVT packet info including date/time, lat/lon/alt, heading, and positional accuracy data.
![PXL_20240303_170936265](https://github.com/BeakeS/TeenyGPSTestbed_C2/assets/27782001/4e2b1242-d968-42f4-b2d3-a8b80f035e75)

GPSLOG: NAV-PVT Packet Logger - For logging PVT packets for analysis or emulation.
![PXL_20240303_171040320](https://github.com/BeakeS/TeenyGPSTestbed_C2/assets/27782001/937f2e26-c376-4e52-9304-112ede750657)

GPSSAT: NAV-SAT Receiver - For testing satellite reception.  Displays a list of tracked satellites sorted by signal strength.
![PXL_20240308_224714124](https://github.com/BeakeS/TeenyGPSTestbed_C2/assets/27782001/3de865b5-92e7-4fd6-8090-d8b94a51ce38)

GPSMAP: Satellite Constellation Map - View of satellite position data including usedForNav (color blue=true/red=false) and signal strength (ring white>=35db,yellow>20db,orange<20db).
![PXL_20240310_163546313](https://github.com/BeakeS/TeenyGPSTestbed_C2/assets/27782001/459200ef-6d56-4f04-88d8-24f82e0a6b73)

GPSEMU: GPS Emulation - Emulates Beitian BN-880Q GPS module including replay of PVT packets from ROM or SDCARD with progressive date/time updates.
![PXL_20240229_005625834](https://github.com/BeakeS/TeenyGPSTestbed_C2/assets/27782001/f4011f04-dae2-459f-9b68-648e4ca00ba4)
