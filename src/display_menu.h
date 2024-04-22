
/********************************************************************/
// Display
bool displayEnabled;
#include <Wire.h>
#include <TeenyPrtVal.h>
#include <TeenyMenu.h>
TFT_eSprite display = TFT_eSprite(&M5.Lcd);
TeenyPrtVal<TFT_eSprite> displayPV(display);
//TeenyMenu<TFT_eSprite> menu(display, 12, 16, false, 16, 16, 6, 10, 11, 154, 6, 0, 20);
TeenyMenu<TFT_eSprite> menu(display, 12, 16, false, 24, 20, 14, 10, 11, 154, 6, 0, 20);

/********************************************************************/
// Display/Message Defines
#define DISPLAY_REFRESH_PERIOD  200
#define MSG_DISPLAY_TIME        5000
int16_t  MENU_MODE = DM_IDLE;
bool displayRefresh;
bool displayUpdate;

/********************************************************************/
// forward declarations
bool msg_update(const char* msgStr);
void deviceMode_init();
void deviceMode_end();

/********************************************************************/
// Menus
//
// menu mode item
void menu_menuModeCB(); // forward declaration
SelectOptionUint8t selectMenuModeOptions[] = {
  {"IDLE",   DM_IDLE},
  {"GPSRCV", DM_GPSRCVR},
  {"GPSLOG", DM_GPSLOGR},
  {"NAVSAT", DM_GPSNSAT},
  {"SATMAP", DM_GPSSMAP},
  {"SATCFG", DM_GPSSCFG},
  {"GPSEMU", DM_GPSEMUL}};
TeenyMenuSelect selectMenuMode(sizeof(selectMenuModeOptions)/sizeof(SelectOptionUint8t), selectMenuModeOptions);
TeenyMenuItem menuItemMenuMode("Device Mode", MENU_MODE, selectMenuMode, menu_menuModeCB);
//
// gps receiver unit
//
void menu_entrGPSRcvrCB(); // forward declaration
void menu_exitGPSRcvrCB(); // forward declaration
TeenyMenuPage menuPageGPSRcvr("GPS RECEIVER MODE", menu_entrGPSRcvrCB, menu_exitGPSRcvrCB);
TeenyMenuItem menuItemGPSRcvr("*START GPS RCVR*", menuPageGPSRcvr);
TeenyMenuItem menuItemGPSRcvrExit(false); // optional return menu item
TeenyMenuItem menuItemGPSRcvrLabel0("");
TeenyMenuItem menuItemGPSRcvrLabel1("");
//
// gps logging unit
//
void menu_entrGPSLogrCB(); // forward declaration
void menu_exitGPSLogrCB(); // forward declaration
TeenyMenuPage menuPageGPSLogr("GPS LOGGING MODE", menu_entrGPSLogrCB, menu_exitGPSLogrCB);
TeenyMenuItem menuItemGPSLogr("*START GPS LOGGER*", menuPageGPSLogr);
TeenyMenuItem menuItemGPSLogrExit(false); // optional return menu item
TeenyMenuItem menuItemGPSLogrLabel0("");
TeenyMenuItem menuItemGPSLogrLabel1("");
TeenyMenuItem menuItemGPSLogrLabel2("");
TeenyMenuItem menuItemGPSLogrLabel3("");
TeenyMenuItem menuItemGPSLogrLabel4("");
TeenyMenuItem menuItemGPSLogrLabel5("");
TeenyMenuItem menuItemGPSLogrLabel6("");
TeenyMenuItem menuItemGPSLogrLabel7("");
//
// ubx logging mode
SelectOptionUint8t selectUBXLogModeOptions[] = {
  {"PVT",    UBXLOG_NAVPVT},
  {"SAT",    UBXLOG_NAVSAT},
  {"PVTSAT", UBXLOG_NAVPVTNAVSAT}};
TeenyMenuSelect selectUBXLogMode(sizeof(selectUBXLogModeOptions)/sizeof(SelectOptionUint8t), selectUBXLogModeOptions);
TeenyMenuItem menuItemUBXLogMode("UBX Log Mode", deviceState.UBXLOGMODE, selectUBXLogMode);
//
// gps logging mode
SelectOptionUint8t selectGPSLogModeOptions[] = {
  {"NONE", GPSLOG_NONE},
  {"GPX", GPSLOG_GPX},
  {"KML", GPSLOG_KML}};
TeenyMenuSelect selectGPSLogMode(sizeof(selectGPSLogModeOptions)/sizeof(SelectOptionUint8t), selectGPSLogModeOptions);
TeenyMenuItem menuItemGPSLogMode("GPS Log Mode", deviceState.GPSLOGMODE, selectGPSLogMode);
//
// gps start/stop logging
void menu_startGPSLogrCB(); // forward declaration
TeenyMenuItem menuItemGPSLogrStrtLog("Start GPS Logging", menu_startGPSLogrCB);
void menu_stopGPSLogrCB(); // forward declaration
TeenyMenuItem menuItemGPSLogrStopLog("Stop GPS Logging", menu_stopGPSLogrCB);
//
// gps navsat unit
//
void menu_entrGPSNsatCB(); // forward declaration
void menu_exitGPSNsatCB(); // forward declaration
bool menu_GPSNsatDisplayMap = false;
void menu_GPSNsatKeyUpCB(); // forward declaration
void menu_GPSNsatKeyDnCB(); // forward declaration
TeenyMenuPage menuPageGPSNsat("GPS NAVSAT MODE", menu_entrGPSNsatCB, menu_exitGPSNsatCB, menu_GPSNsatKeyUpCB, menu_GPSNsatKeyDnCB);
TeenyMenuItem menuItemGPSNsat("*START GPS NAVSAT*", menuPageGPSNsat);
TeenyMenuItem menuItemGPSNsatLabel0("");
TeenyMenuItem menuItemGPSNsatExit(false); // optional return menu item
//
// gps satellite constellation map unit
//
void menu_entrGPSSmapCB(); // forward declaration
void menu_exitGPSSmapCB(); // forward declaration
TeenyMenuPage menuPageGPSSmap("CONSTELLATION MAP", menu_entrGPSSmapCB, menu_exitGPSSmapCB);
TeenyMenuItem menuItemGPSSmap("*START GPS SATMAP*", menuPageGPSSmap);
TeenyMenuItem menuItemGPSSmapLabel0("");
TeenyMenuItem menuItemGPSSmapExit(false); // optional return menu item
//
// gps satellite config unit
//
void menu_entrGPSScfgCB(); // forward declaration
void menu_exitGPSScfgCB(); // forward declaration
TeenyMenuPage menuPageGPSScfg("GNSS CONFIG TOOLS", menu_entrGPSScfgCB, menu_exitGPSScfgCB);
TeenyMenuItem menuItemGPSScfg("*START GPS SATCFG*", menuPageGPSScfg);
TeenyMenuItem menuItemGPSScfgLabel0("");
TeenyMenuItem menuItemGPSScfgLabel1("");
TeenyMenuItem menuItemGPSScfgLabel2("");
TeenyMenuItem menuItemGPSScfgExit(false); // optional return menu item
//
// gnss satellite select info
void menu_pollGNSSSelInfoCB(); // forward declaration
TeenyMenuPage menuPageGNSSSelInfo("GNSS SELECT INFO", menu_pollGNSSSelInfoCB);
TeenyMenuItem menuItemGNSSSelInfo("GNSS Select Info", menuPageGNSSSelInfo);
TeenyMenuItem menuItemGNSSSelInfoLabel0("");
TeenyMenuItem menuItemGNSSSelInfoLabel1("");
TeenyMenuItem menuItemGNSSSelInfoLabel2("");
TeenyMenuItem menuItemGNSSSelInfoLabel3("");
TeenyMenuItem menuItemGNSSSelInfoLabel4("");
TeenyMenuItem menuItemGNSSSelInfoExit(false); // optional return menu item
//
// gnss satellite config info
void menu_pollGNSSCfgInfoCB(); // forward declaration
TeenyMenuPage menuPageGNSSCfgInfo("GNSS CONFIG INFO", menu_pollGNSSCfgInfoCB);
TeenyMenuItem menuItemGNSSCfgInfo("GNSS Config Info", menuPageGNSSCfgInfo);
TeenyMenuItem menuItemGNSSCfgInfoLabel0("");
TeenyMenuItem menuItemGNSSCfgInfoLabel1("");
TeenyMenuItem menuItemGNSSCfgInfoLabel2("");
TeenyMenuItem menuItemGNSSCfgInfoLabel3("");
TeenyMenuItem menuItemGNSSCfgInfoLabel4("");
TeenyMenuItem menuItemGNSSCfgInfoLabel5("");
TeenyMenuItem menuItemGNSSCfgInfoLabel6("");
TeenyMenuItem menuItemGNSSCfgInfoLabel7("");
TeenyMenuItem menuItemGNSSCfgInfoLabel8("");
TeenyMenuItem menuItemGNSSCfgInfoLabel9("");
TeenyMenuItem menuItemGNSSCfgInfoExit(false); // optional return menu item
//
// gnss satellite toggle items
void menu_gnssCfgGPSToggleCB(); // forward declaration
void menu_gnssCfgSBASToggleCB(); // forward declaration
void menu_gnssCfgGalileoToggleCB(); // forward declaration
void menu_gnssCfgBeiDouToggleCB(); // forward declaration
void menu_gnssCfgIMESToggleCB(); // forward declaration
void menu_gnssCfgQZSSToggleCB(); // forward declaration
void menu_gnssCfgGLONASSToggleCB(); // forward declaration
// Note:  Toggle menu item titles set by menu_gnssConfigurator()
TeenyMenuItem menuItemGNSSCfgGPSToggle(    "", menu_gnssCfgGPSToggleCB);
TeenyMenuItem menuItemGNSSCfgSBASToggle(   "", menu_gnssCfgSBASToggleCB);
TeenyMenuItem menuItemGNSSCfgGalileoToggle("", menu_gnssCfgGalileoToggleCB);
TeenyMenuItem menuItemGNSSCfgBeiDouToggle( "", menu_gnssCfgBeiDouToggleCB);
TeenyMenuItem menuItemGNSSCfgIMESToggle(   "", menu_gnssCfgIMESToggleCB);
TeenyMenuItem menuItemGNSSCfgQZSSToggle(   "", menu_gnssCfgQZSSToggleCB);
TeenyMenuItem menuItemGNSSCfgGLONASSToggle("", menu_gnssCfgGLONASSToggleCB);
//
// gps emulator unit
//
void menu_entrGPSEmulCB(); // forward declaration
void menu_exitGPSEmulCB(); // forward declaration
TeenyMenuPage menuPageGPSEmul("GPS EMULATOR MODE", menu_entrGPSEmulCB, menu_exitGPSEmulCB);
TeenyMenuItem menuItemGPSEmul("*START EMULATOR*", menuPageGPSEmul);
TeenyMenuItem menuItemGPSEmulExit(false); // optional return menu item
TeenyMenuItem menuItemGPSEmulLabel0("");
TeenyMenuItem menuItemGPSEmulLabel1("");
//
// gps reset
//
void menu_GPSResetCB(); // forward declaration
SelectOptionUint8t selectGPSResetOptions[] = {
  {"NO",   GPS_NORESET},
  {"HOT",  GPS_HOTSTART},
  {"WARM", GPS_WARMSTART},
  {"COLD", GPS_COLDSTART},
  {"HWRST", GPS_HARDWARERESET}};
TeenyMenuSelect selectGPSReset(sizeof(selectGPSResetOptions)/sizeof(SelectOptionUint8t), selectGPSResetOptions);
TeenyMenuItem menuItemGPSReset("GPS Reset", deviceState.GPSRESET, selectGPSReset, menu_GPSResetCB);
//
// TEENYGPSEMULATOR SETTINGS
//
// settings menu
TeenyMenuPage menuPageTopLevelSettings("SETTINGS MENU");
TeenyMenuItem menuItemTopLevelSettings("Device Settings", menuPageTopLevelSettings);
TeenyMenuItem menuItemTopLevelSettingsExit(false); // optional return menu item
//
// TIME ZONE
//
// time zone menu
SelectOptionInt16t selectTimeZoneOptions[] = {
  {"-12:00",-720}, {"-11:00",-660}, {"-10:00",-600}, {"-09:30",-570}, {"-09:00",-540},
  {"-08:00",-480}, {"-07:00",-420}, {"-06:00",-360}, {"-05:00",-300}, {"-04:00",-240},
  {"-03:30",-210}, {"-03:00",-180}, {"-02:30",-150}, {"-02:00",-120}, {"-01:00",-060},
  { "00:00", 000}, { "01:00", 060}, { "02:00", 120}, { "03:00", 180}, { "03:30", 210},
  { "04:00", 240}, { "04:30", 270}, { "05:00", 300}, { "05:30", 330}, { "05:45", 345},
  { "06:00", 360}, { "06:30", 390}, { "07:00", 420}, { "08:00", 480}, { "08:45", 525},
  { "09:00", 540}, { "09:30", 570}, { "10:00", 600}, { "10:30", 630}, { "11:00", 660},
  { "12:00", 720}, { "12:45", 765}, { "13:00", 780}, { "13:45", 825}, { "14:00", 840}};
TeenyMenuSelect selectTimeZone(sizeof(selectTimeZoneOptions)/sizeof(SelectOptionInt16t), selectTimeZoneOptions);
TeenyMenuItem menuItemTimeZone("Time Zone", deviceState.TIMEZONE, selectTimeZone, nullptr, 2);
//
// RTC SETTINGS
//
// rtc settings menu
void menu_getRTC_CB(); // forward declaration
void menu_setRTC_CB(); // forward declaration
TeenyMenuPage menuPageRTCSettings("REAL TIME CLOCK", menu_getRTC_CB);
TeenyMenuItem menuItemRTCSettings("RT Clock Settings", menuPageRTCSettings);
TeenyMenuItem menuItemRTCSettingsLabel0("");
TeenyMenuItem menuItemRTCSettingsLabel1("");
TeenyMenuItem menuItemRTCSettingsLabel2("");
TeenyMenuItem menuItemRTCSettingsExit(false); // optional return menu item
//
// rtc year
int16_t menuRTCYear = 2024;
int16_t menuRTCYearMin = 2024;
int16_t menuRTCYearMax = 2042;
TeenyMenuItem menuItemRTCYear("RTC Year", menuRTCYear, menuRTCYearMin, menuRTCYearMax);
//
// rtc month
uint8_t menuRTCMonth = 1;
uint8_t menuRTCMonthMin = 1;
uint8_t menuRTCMonthMax = 12;
TeenyMenuItem menuItemRTCMonth("RTC Month", menuRTCMonth, menuRTCMonthMin, menuRTCMonthMax);
//
// rtc day
uint8_t menuRTCDay = 1;
uint8_t menuRTCDayMin = 1;
uint8_t menuRTCDayMax = 31;
TeenyMenuItem menuItemRTCDay("RTC Day", menuRTCDay, menuRTCDayMin, menuRTCDayMax);
//
// rtc hour
uint8_t menuRTCHour = 0;
uint8_t menuRTCHourMin = 0;
uint8_t menuRTCHourMax = 23;
TeenyMenuItem menuItemRTCHour("RTC Hour", menuRTCHour, menuRTCHourMin, menuRTCHourMax);
//
// rtc minute
uint8_t menuRTCMinute = 0;
uint8_t menuRTCMinuteMin = 0;
uint8_t menuRTCMinuteMax = 59;
TeenyMenuItem menuItemRTCMinute("RTC Minute", menuRTCMinute, menuRTCMinuteMin, menuRTCMinuteMax);
//
// rtc second
uint8_t menuRTCSecond = 0;
uint8_t menuRTCSecondMin = 0;
uint8_t menuRTCSecondMax = 59;
TeenyMenuItem menuItemRTCSecond("RTC Second", menuRTCSecond, menuRTCSecondMin, menuRTCSecondMax);
//
// rtc set date/time
TeenyMenuItem menuItemRTCSetDateTime("Set RTC Date/Time", menu_setRTC_CB);
//
// GPS SETTINGS
//
// gps settings menu
TeenyMenuPage menuPageGPSSettings("GPS SETTINGS");
TeenyMenuItem menuItemGPSSettings("GPS Settings", menuPageGPSSettings);
TeenyMenuItem menuItemGPSSettingsExit(false); // optional return menu item
//
// gps factory reset menu
void menu_cancelGPSFactoryResetCB(); // forward declaration
TeenyMenuPage menuPageGPSFactoryReset("GPS FACTORY RESET", nullptr, menu_cancelGPSFactoryResetCB);
TeenyMenuItem menuItemGPSFactoryReset("GPS Factory Reset", menuPageGPSFactoryReset);
TeenyMenuItem menuItemGPSFactoryResetExit(false); // optional return menu item
//
// confirm gps reset
void menu_confirmGPSFactoryResetCB(); // forward declaration
TeenyMenuItem menuItemConfirmGPSFactoryReset("Confirm Reset?", menu_confirmGPSFactoryResetCB);
//
// EMULATOR SETTINGS
//
// emulator settings menu
TeenyMenuPage menuPageEMULSettings("EMULATOR SETTINGS");
TeenyMenuItem menuItemEMULSettings("Emulator Settings", menuPageEMULSettings);
TeenyMenuItem menuItemEMULSettingsExit(false); // optional return menu item
//
// emulator cold start packet count
uint8_t menuColdStartPktsMin = 0;
uint8_t menuColdStartPktsMax = 60;
TeenyMenuItem menuItemColdStartPkts("ColdStrtPkts", deviceState.EMUL_NUMCOLDSTARTPACKETS, menuColdStartPktsMin, menuColdStartPktsMax);
//
// display brightness
void menu_displayBrightnessCB(); // forward declaration
SelectOptionUint8t selectDisplayBrightnessOptions[] = {
  {"100", 100},
  {"90",   90},
  {"80",   80},
  {"70",   70},
  {"60",   60},
  {"50",   50}};
TeenyMenuSelect selectDisplayBrightness(sizeof(selectDisplayBrightnessOptions)/sizeof(SelectOptionUint8t), selectDisplayBrightnessOptions);
TeenyMenuItem menuItemDisplayBrightness("Dsp Brghtns", deviceState.DISPLAYBRIGHTNESS, selectDisplayBrightness, menu_displayBrightnessCB);
//
// display timeout
SelectOptionUint8t selectDisplayTimeoutOptions[] = {
  {"NEVER", 99},
  {"30",    30},
  {"20",    20},
  {"10",    10},
  {"5",      5},
  {"1",      1}};
TeenyMenuSelect selectDisplayTimeout(sizeof(selectDisplayTimeoutOptions)/sizeof(SelectOptionUint8t), selectDisplayTimeoutOptions);
TeenyMenuItem menuItemDisplayTimeout("Dsp Timeout", deviceState.DISPLAYTIMEOUT, selectDisplayTimeout);
//
// status led
//TeenyMenuItem menuItemStatusLED("Status LED", deviceState.STATUSLED, nullptr, "OFF", "ON");
//
// SAVE SETTINGS
//
// save settings menu
void menu_cancelSaveSettingsCB(); // forward declaration
TeenyMenuPage menuPageSaveSettings("SAVE DEVICE SETTINGS", nullptr, menu_cancelSaveSettingsCB);
TeenyMenuItem menuItemSaveSettings("Save Dev Settings", menuPageSaveSettings);
TeenyMenuItem menuItemSaveSettingsExit(false); // optional return menu item
//
// save state to flash
void menu_confirmSaveSettingsCB(); // forward declaration
TeenyMenuItem menuItemConfirmSaveSettings("Confirm Save?", menu_confirmSaveSettingsCB);
//
// RESTART DEVICE
//
// restart device menu
void menu_cancelRestartDeviceCB(); // forward declaration
TeenyMenuPage menuPageRestartDevice("RESTART DEVICE", nullptr, menu_cancelRestartDeviceCB);
TeenyMenuItem menuItemRestartDevice("Restart Device", menuPageRestartDevice);
TeenyMenuItem menuItemRestartDeviceExit(false); // optional return menu item
//
// confirm restart
void menu_confirmRestartDeviceCB(); // forward declaration
TeenyMenuItem menuItemConfirmRestartDevice("Confirm Restart?", menu_confirmRestartDeviceCB);
//
// RESET DEVICE
//
// reset device menu
void menu_cancelResetDeviceCB(); // forward declaration
TeenyMenuPage menuPageResetDevice("RESET DEVICE", nullptr, menu_cancelResetDeviceCB);
TeenyMenuItem menuItemResetDevice("Reset Device", menuPageResetDevice);
TeenyMenuItem menuItemResetDeviceExit(false); // optional return menu item
//
// confirm reset
void menu_confirmResetDeviceCB(); // forward declaration
TeenyMenuItem menuItemConfirmResetDevice("Confirm Reset?", menu_confirmResetDeviceCB);
//
// MAIN MENU
//
//TeenyMenuPage menuPageMain("TEENY GPS TESTBED", nullptr, menu_exit);
TeenyMenuPage menuPageMain("TEENY GPS TESTBED");
TeenyMenuItem menuItemLabel0("");
TeenyMenuItem menuItemLabel1("");
TeenyMenuItem menuItemLabel2("");
TeenyMenuItem menuItemLabel3("");
TeenyMenuItem menuItemLabel4("");
TeenyMenuItem menuItemLabel5("");
TeenyMenuItem menuItemLabel6("");
//
// menu variables
uint32_t menuInputTime;
bool menuDisplaySleepMode;

/********************************************************************/
void menu_setup() {
  // add menu items
  menuPageMain.addMenuItem(menuItemLabel0);
  menuPageMain.addMenuItem(menuItemGPSRcvr);
  menuPageGPSRcvr.addMenuItem(menuItemGPSRcvrLabel0);
  menuPageGPSRcvr.addMenuItem(menuItemGPSRcvrLabel1);
  //menuPageBaseUnit.addMenuItem(menuItemGPSRcvrExit);
  menuPageMain.addMenuItem(menuItemGPSLogr);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel0);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel1);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel2);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel3);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel4);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel5);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel6);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel7);
  menuPageGPSLogr.addMenuItem(menuItemUBXLogMode);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogMode);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrStrtLog);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrStopLog);
  //menuPageGPSLogr.addMenuItem(menuItemGPSLogrExit);
  menuPageMain.addMenuItem(menuItemGPSNsat);
  menuPageGPSNsat.addMenuItem(menuItemGPSNsatLabel0);
  //menuPageGPSNsat.addMenuItem(menuItemGPSNsatExit);
  menuPageMain.addMenuItem(menuItemGPSSmap);
  menuPageGPSSmap.addMenuItem(menuItemGPSSmapLabel0);
  //menuPageGPSSmap.addMenuItem(menuItemGPSSmapExit);
  menuPageMain.addMenuItem(menuItemGPSScfg);
  menuPageGPSScfg.addMenuItem(menuItemGPSScfgLabel0);
  menuPageGPSScfg.addMenuItem(menuItemGNSSSelInfo);
  menuPageGNSSSelInfo.addMenuItem(menuItemGNSSSelInfoLabel0);
  menuPageGNSSSelInfo.addMenuItem(menuItemGNSSSelInfoLabel1);
  menuPageGNSSSelInfo.addMenuItem(menuItemGNSSSelInfoLabel2);
  menuPageGNSSSelInfo.addMenuItem(menuItemGNSSSelInfoLabel3);
  menuPageGNSSSelInfo.addMenuItem(menuItemGNSSSelInfoLabel4);
  menuPageGNSSSelInfo.addMenuItem(menuItemGNSSSelInfoExit);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgInfo);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel0);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel1);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel2);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel3);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel4);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel5);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel6);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel7);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel8);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel9);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoExit);
  menuPageGPSScfg.addMenuItem(menuItemGPSScfgLabel1);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgGPSToggle);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgSBASToggle);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgGalileoToggle);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgBeiDouToggle);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgIMESToggle);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgQZSSToggle);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgGLONASSToggle);
  menuPageGPSScfg.addMenuItem(menuItemGPSScfgLabel2);
  menuPageGPSScfg.addMenuItem(menuItemGPSScfgExit);
  menuPageMain.addMenuItem(menuItemGPSEmul);
  menuPageGPSEmul.addMenuItem(menuItemGPSEmulLabel0);
  menuPageGPSEmul.addMenuItem(menuItemGPSEmulLabel1);
  //menuPageGPSEmul.addMenuItem(menuItemGPSEmulExit);
  menuPageMain.addMenuItem(menuItemGPSReset);
  menuPageMain.addMenuItem(menuItemLabel1);
  menuPageMain.addMenuItem(menuItemLabel2);
  menuPageMain.addMenuItem(menuItemLabel3);
  menuPageMain.addMenuItem(menuItemMenuMode);
  menuPageMain.addMenuItem(menuItemLabel4);
  menuPageMain.addMenuItem(menuItemLabel5);
  menuPageMain.addMenuItem(menuItemLabel6);
  menuPageMain.addMenuItem(menuItemTopLevelSettings);
  menuPageTopLevelSettings.addMenuItem(menuItemTimeZone);
  menuPageTopLevelSettings.addMenuItem(menuItemRTCSettings);
  menuPageRTCSettings.addMenuItem(menuItemRTCYear);
  menuPageRTCSettings.addMenuItem(menuItemRTCMonth);
  menuPageRTCSettings.addMenuItem(menuItemRTCDay);
  menuPageRTCSettings.addMenuItem(menuItemRTCHour);
  menuPageRTCSettings.addMenuItem(menuItemRTCMinute);
  menuPageRTCSettings.addMenuItem(menuItemRTCSecond);
  menuPageRTCSettings.addMenuItem(menuItemRTCSettingsLabel0);
  menuPageRTCSettings.addMenuItem(menuItemRTCSetDateTime);
  menuPageRTCSettings.addMenuItem(menuItemRTCSettingsLabel1);
  menuPageRTCSettings.addMenuItem(menuItemRTCSettingsLabel2);
  menuPageRTCSettings.addMenuItem(menuItemRTCSettingsExit); // optional return menu item
  menuPageTopLevelSettings.addMenuItem(menuItemGPSSettings);
  menuPageGPSSettings.addMenuItem(menuItemGPSFactoryReset);
  menuPageGPSFactoryReset.addMenuItem(menuItemConfirmGPSFactoryReset);
  menuPageGPSFactoryReset.addMenuItem(menuItemGPSFactoryResetExit);
  menuPageGPSSettings.addMenuItem(menuItemGPSSettingsExit); // optional return menu item
  menuPageTopLevelSettings.addMenuItem(menuItemEMULSettings);
  menuPageEMULSettings.addMenuItem(menuItemColdStartPkts);
  menuPageEMULSettings.addMenuItem(menuItemEMULSettingsExit); // optional return menu item
  menuPageTopLevelSettings.addMenuItem(menuItemDisplayBrightness);
  menuPageTopLevelSettings.addMenuItem(menuItemDisplayTimeout);
  //menuPageTopLevelSettings.addMenuItem(menuItemStatusLED);
  menuPageTopLevelSettings.addMenuItem(menuItemSaveSettings);
  menuPageSaveSettings.addMenuItem(menuItemConfirmSaveSettings);
  menuPageSaveSettings.addMenuItem(menuItemSaveSettingsExit);
  menuPageTopLevelSettings.addMenuItem(menuItemRestartDevice);
  menuPageRestartDevice.addMenuItem(menuItemConfirmRestartDevice);
  menuPageRestartDevice.addMenuItem(menuItemRestartDeviceExit);
  menuPageTopLevelSettings.addMenuItem(menuItemResetDevice);
  menuPageResetDevice.addMenuItem(menuItemConfirmResetDevice);
  menuPageResetDevice.addMenuItem(menuItemResetDeviceExit);
  menuPageTopLevelSettings.addMenuItem(menuItemTopLevelSettingsExit);
  // assign the starting menu page
  menu.setMenuPageCurrent(menuPageMain);
  switch(deviceState.DEVICE_MODE) {
    case DM_GPSRCVR:
      menu.linkMenuPage(menuPageGPSRcvr);
      break;
    case DM_GPSLOGR:
      menu.linkMenuPage(menuPageGPSLogr);
      break;
    case DM_GPSNSAT:
      menu.linkMenuPage(menuPageGPSNsat);
      break;
    case DM_GPSSMAP:
      menu.linkMenuPage(menuPageGPSSmap);
      break;
    case DM_GPSSCFG:
      menu.linkMenuPage(menuPageGPSScfg);
      break;
    case DM_GPSEMUL:
      menu.linkMenuPage(menuPageGPSEmul);
      break;
  }
  // init and enter menu
  menu.setTextColor(WHITE, BLACK);
  menuDisplaySleepMode = false;
  menu_displayBrightnessCB();
}

/********************************************************************/
void menu_init() {
  menu_menuModeCB();
}

/********************************************************************/
void menu_enter() {
  menu.resetMenu();
  displayRefresh = true;
}

/********************************************************************/
void menu_exit() {
  // This is an optional exitAction for menuPageMain (top level menu page)
  // You can execute something here to exit to another/master GUI
}

/********************************************************************/
void menu_idle_timer() {
  if(deviceState.DISPLAYTIMEOUT == 99) {
    menuDisplaySleepMode = false;
    return;
  }
  if((millis()-menuInputTime) > (uint32_t)(deviceState.DISPLAYTIMEOUT*MILLIS_PER_MIN)) {
    if(!menuDisplaySleepMode) {
      menuDisplaySleepMode = true;
      //msg_update("Enter Sleep Mode");
      //M5.Lcd.sleep();
      M5.Axp.SetDCDC3(false);
      M5.Axp.ScreenBreath(0);
    }
  } else if(menuDisplaySleepMode) {
    menuDisplaySleepMode = false;
    //M5.Lcd.wakeup();
    menu_displayBrightnessCB();
    msg_update("Exit Sleep Mode");
  }
}

/********************************************************************/
void menu_menuModeCB() {
  // choose submenu item based on MENU_MODE
  switch(MENU_MODE) {
    case DM_IDLE:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSNsat.hide();
      menuItemGPSSmap.hide();
      menuItemGPSScfg.hide();
      menuItemGPSReset.hide();
      menuItemLabel2.show();
      menuItemGPSEmul.hide();
      menuItemLabel3.show();
      break;
    case DM_GPSRCVR:
      menuItemGPSRcvr.show();
      menuItemGPSLogr.hide();
      menuItemGPSNsat.hide();
      menuItemGPSSmap.hide();
      menuItemGPSScfg.hide();
      menuItemGPSReset.show();
      menuItemLabel2.hide();
      menuItemGPSEmul.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSLOGR:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.show();
      menuItemGPSNsat.hide();
      menuItemGPSSmap.hide();
      menuItemGPSScfg.hide();
      menuItemGPSReset.show();
      menuItemLabel2.hide();
      menuItemGPSEmul.hide();
      menuItemLabel3.hide();
      menuItemUBXLogMode.hide(ubxLoggingInProgress ? true : false);
      menuItemGPSLogMode.hide(ubxLoggingInProgress ? true : false);
      menuItemGPSLogrStrtLog.hide(ubxLoggingInProgress ? true : false);
      menuItemGPSLogrStopLog.hide(ubxLoggingInProgress ? false : true);
      break;
    case DM_GPSNSAT:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSNsat.show();
      menuItemGPSSmap.hide();
      menuItemGPSScfg.hide();
      menuItemGPSReset.show();
      menuItemLabel2.hide();
      menuItemGPSEmul.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSSMAP:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSNsat.hide();
      menuItemGPSSmap.show();
      menuItemGPSScfg.hide();
      menuItemGPSReset.show();
      menuItemLabel2.hide();
      menuItemGPSEmul.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSSCFG:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSNsat.hide();
      menuItemGPSSmap.hide();
      menuItemGPSScfg.show();
      menuItemGPSReset.show();
      menuItemLabel2.hide();
      menuItemGPSEmul.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSEMUL:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSNsat.hide();
      menuItemGPSSmap.hide();
      menuItemGPSScfg.hide();
      menuItemGPSReset.hide();
      menuItemLabel2.show();
      menuItemGPSEmul.show();
      menuItemLabel3.hide();
      break;
  }
  if(MENU_MODE==DM_IDLE) {
    menuItemMenuMode.setTitle("Device Mode");
  } else {
    menuItemMenuMode.setTitle("Change Mode");
    menu.resetMenu();
  }
}

/********************************************************************/
void menu_GPSResetCB() {
  menu.resetMenu();
}

/********************************************************************/
void menu_entrGPSRcvrCB() {
  deviceState.DEVICE_MODE = DM_GPSRCVR;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSRcvrCB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSLogrCB() {
  deviceState.DEVICE_MODE = DM_GPSLOGR;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_startGPSLogrCB() {
  if(ubxLoggingInProgress) return;
  if(sdcard_openUBXLoggingFile()) {
    ubxLoggingInProgress = true;
    menuItemUBXLogMode.hide();
    menuItemGPSLogMode.hide();
    menuItemGPSLogrStrtLog.hide();
    menuItemGPSLogrStopLog.show();
    msg_update("GPS Logging Started");
  } else {
    msg_update("SD Card Error");
  }
  if((deviceState.UBXLOGMODE == GPSLOG_GPX) && !sdcard_openGPXLoggingFile()) {
    msg_update("SD Card Error");
  }
  if((deviceState.UBXLOGMODE == GPSLOG_KML) && !sdcard_openKMLLoggingFile()) {
    msg_update("SD Card Error");
  }
  displayRefresh = true;
}

/********************************************************************/
void menu_stopGPSLogrCB() {
  char _msgStr[22];
  if(!ubxLoggingInProgress) return;
  sdcard_closeUBXLoggingFile();
  ubxLoggingInProgress = false;
  menuItemUBXLogMode.show();
  menuItemGPSLogMode.show();
  menuItemGPSLogrStrtLog.show();
  menuItemGPSLogrStopLog.hide();
  if(deviceState.UBXLOGMODE == GPSLOG_GPX) {
    sdcard_closeGPXLoggingFile();
  }
  if(deviceState.UBXLOGMODE == GPSLOG_KML) {
    sdcard_closeKMLLoggingFile();
  }
  sprintf(_msgStr, "F%02d TP=%04d VP=%04d",
          ubxLoggingFileNum,
          min(ubxLoggingFileWriteCount, 9999),
          min(ubxLoggingFileWriteValidCount, 9999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSLogrCB() {
  menu_stopGPSLogrCB();
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSNsatCB() {
  deviceState.DEVICE_MODE = DM_GPSNSAT;
  deviceMode_init();
  menu_GPSNsatDisplayMap = false;
  displayRefresh = true;
}

/********************************************************************/
void menu_GPSNsatKeyUpCB() {
  menu_GPSNsatDisplayMap = !menu_GPSNsatDisplayMap;
  displayRefresh = true;
}

/********************************************************************/
void menu_GPSNsatKeyDnCB() {
  menu_GPSNsatDisplayMap = !menu_GPSNsatDisplayMap;
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSNsatCB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSSmapCB() {
  deviceState.DEVICE_MODE = DM_GPSSMAP;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSSmapCB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssConfigurator(char toggleGnssIdType) {
  char _msgStr[22];
  // GNSS State - Unknown=-1, Disabled=0, Enabled=1
  typedef struct {
    int8_t GPS = -1;
    int8_t SBAS = -1;
    int8_t Galileo = -1;
    int8_t BeiDou = -1;
    int8_t IMES = -1;
    int8_t QZSS = -1;
    int8_t GLONASS = -1;
    int8_t spare00;
  } gnss_state_t;
  gnss_state_t gnssState;
  // Reset State
  // Poll GNSS config state
  bool pollRC = gps.pollGNSSConfigInfo();
  ubloxCFGGNSSInfo_t gnssConfigInfo = gps.getGNSSConfigInfo();
  uint8_t gnssId;
  char gnssIdType;
  uint8_t gnssEnable;
  bool toggleRC;
  // Toggle selected GNSS
  if(pollRC) {
    for(uint8_t i=0; i<min(gnssConfigInfo.numConfigBlocks, 7); i++) {
      gnssId = gnssConfigInfo.configBlockList[i].gnssId;
      gnssIdType = gnssConfigInfo.configBlockList[i].gnssIdType;
      gnssEnable = gnssConfigInfo.configBlockList[i].enable;
      if(toggleGnssIdType == gnssIdType) {
        toggleRC = gps.setGNSSConfig(gnssId, !gnssEnable);
        if(toggleRC) gnssEnable = !gnssEnable;
        sprintf(_msgStr, "Set CFG-GNSS rc=%d", toggleRC);
        msg_update(_msgStr);
      }
      switch(gnssIdType) {
        case  'G':
          gnssState.GPS = gnssEnable;
          break;
        case  'S':
          gnssState.SBAS = gnssEnable;
          break;
        case  'E':
          gnssState.Galileo = gnssEnable;
          break;
        case  'B':
          gnssState.BeiDou = gnssEnable;
          break;
        case  'I':
          gnssState.IMES = gnssEnable;
          break;
        case  'Q':
          gnssState.QZSS = gnssEnable;
          break;
        case  'R':
          gnssState.GLONASS = gnssEnable;
          break;
      }
    }
  }
  if((!pollRC) || toggleGnssIdType == '\0') {
    sprintf(_msgStr, "Poll CFG-GNSS rc=%d", pollRC);
    msg_update(_msgStr);
  }
  // Update toggle menu item titles
  // Note: setTitle() only accepts const char*
  switch(gnssState.GPS) {
    case -1: menuItemGNSSCfgGPSToggle.setTitle("GPS    :Unknown"); break;
    case  0: menuItemGNSSCfgGPSToggle.setTitle("GPS    :Disabled"); break;
    case  1: menuItemGNSSCfgGPSToggle.setTitle("GPS    :Enabled"); break;
  }
  switch(gnssState.SBAS) {
    case -1: menuItemGNSSCfgSBASToggle.setTitle("SBAS   :Unknown"); break;
    case  0: menuItemGNSSCfgSBASToggle.setTitle("SBAS   :Disabled"); break;
    case  1: menuItemGNSSCfgSBASToggle.setTitle("SBAS   :Enabled"); break;
  }
  switch(gnssState.Galileo) {
    case -1: menuItemGNSSCfgGalileoToggle.setTitle("Galileo:Unknown"); break;
    case  0: menuItemGNSSCfgGalileoToggle.setTitle("Galileo:Disabled"); break;
    case  1: menuItemGNSSCfgGalileoToggle.setTitle("Galileo:Enabled"); break;
  }
  switch(gnssState.BeiDou) {
    case -1: menuItemGNSSCfgBeiDouToggle.setTitle("BeiDou :Unknown"); break;
    case  0: menuItemGNSSCfgBeiDouToggle.setTitle("BeiDou :Disabled"); break;
    case  1: menuItemGNSSCfgBeiDouToggle.setTitle("BeiDou :Enabled"); break;
  }
  switch(gnssState.IMES) {
    case -1: menuItemGNSSCfgIMESToggle.setTitle("IMES   :Unknown"); break;
    case  0: menuItemGNSSCfgIMESToggle.setTitle("IMES   :Disabled"); break;
    case  1: menuItemGNSSCfgIMESToggle.setTitle("IMES   :Enabled"); break;
  }
  switch(gnssState.QZSS) {
    case -1: menuItemGNSSCfgQZSSToggle.setTitle("QZSS   :Unknown"); break;
    case  0: menuItemGNSSCfgQZSSToggle.setTitle("QZSS   :Disabled"); break;
    case  1: menuItemGNSSCfgQZSSToggle.setTitle("QZSS   :Enabled"); break;
  }
  switch(gnssState.GLONASS) {
    case -1: menuItemGNSSCfgGLONASSToggle.setTitle("GLONASS:Unknown"); break;
    case  0: menuItemGNSSCfgGLONASSToggle.setTitle("GLONASS:Disabled"); break;
    case  1: menuItemGNSSCfgGLONASSToggle.setTitle("GLONASS:Enabled"); break;
  }
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSScfgCB() {
  deviceState.DEVICE_MODE = DM_GPSSCFG;
  deviceMode_init();
  menu_gnssConfigurator('\0');
  displayRefresh = true;
}

/********************************************************************/
void menu_pollGNSSSelInfoCB() {
  char _msgStr[22];
  bool rcode = gps.pollGNSSSelectionInfo();
  sprintf(_msgStr, "Poll MON-GNSS rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_pollGNSSCfgInfoCB() {
  char _msgStr[22];
  bool rcode = gps.pollGNSSConfigInfo();
  sprintf(_msgStr, "Poll CFG-GNSS rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgGPSToggleCB() {
  menu_gnssConfigurator('G');
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgSBASToggleCB() {
  menu_gnssConfigurator('S');
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgGalileoToggleCB() {
  menu_gnssConfigurator('E');
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgBeiDouToggleCB() {
  menu_gnssConfigurator('B');
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgIMESToggleCB() {
  menu_gnssConfigurator('I');
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgQZSSToggleCB() {
  menu_gnssConfigurator('Q');
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgGLONASSToggleCB() {
  menu_gnssConfigurator('R');
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSScfgCB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSEmulCB() {
  deviceState.DEVICE_MODE = DM_GPSEMUL;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSEmulCB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_getRTC_CB() {
  if(rtc.isValid()) {
    rtc_datetime_t now = rtc.getRTCTime(deviceState.TIMEZONE);
    menuRTCYear    = now.year;
    menuRTCMonth   = now.month;
    menuRTCDay     = now.day;
    menuRTCHour    = now.hour;
    menuRTCMinute  = now.minute;
    menuRTCSecond  = now.second;
  }
}

/********************************************************************/
void menu_setRTC_CB() {
  rtc.setRTCTime(menuRTCYear, menuRTCMonth, menuRTCDay,
                 menuRTCHour, menuRTCMinute, menuRTCSecond,
                 -deviceState.TIMEZONE);
  msg_update("RTC Date/Time Set");
}

/********************************************************************/
void menu_cancelGPSFactoryResetCB() {
  menu.exitToParentMenuPage();
  msg_update("GPS Reset Canceled");
  displayRefresh = true;
}

/********************************************************************/
void menu_confirmGPSFactoryResetCB() {
  menu.exitToParentMenuPage();
  gpsSerial = &Serial2;
  if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_NORESET, 0, 0) &&
     gps.factoryReset()) {
    msg_update("GPS Reset Completed");
  } else {
    msg_update("GPS Reset Failed");
  }
  displayRefresh = true;
}

/********************************************************************/
void menu_displayBrightnessCB() {
  // Make sure screen is turned on
  M5.Axp.SetDCDC3(true);
  // Set brightness
  M5.Axp.ScreenBreath(deviceState.DISPLAYBRIGHTNESS);
}

/********************************************************************/
void menu_cancelSaveSettingsCB() {
  menu.exitToParentMenuPage();
  msg_update("Save Canceled");
  displayRefresh = true;
}

/********************************************************************/
void menu_confirmSaveSettingsCB() {
  menu.exitToParentMenuPage();
  if(sdcard_deviceStateSave()) {
    msg_update("Settings Saved");
  } else {
    msg_update("Save Settings Failed");
  }
  displayRefresh = true;
}

/********************************************************************/
void menu_cancelRestartDeviceCB() {
  menu.exitToParentMenuPage();
  msg_update("Restart Canceled");
  displayRefresh = true;
}

/********************************************************************/
void menu_confirmRestartDeviceCB() {
  device_reset();
}

/********************************************************************/
void menu_cancelResetDeviceCB() {
  menu.exitToParentMenuPage();
  msg_update("Reset Canceled");
  displayRefresh = true;
}

/********************************************************************/
void menu_confirmResetDeviceCB() {
  sdcard_deviceStateReset();
  device_reset();
}

/********************************************************************/
void display_clearDisplay() {
  display.fillSprite(BLACK);
}

/********************************************************************/
void display_display() {
  display.pushSprite(0, 0);
}

/********************************************************************/
bool display_setup() {
  // Setup display
  //M5.Lcd.setRotation(1); // M5Display.cpp default
  M5.Lcd.setRotation(2);
  display.setPsram(true); // Use psram
  //display.setColorDepth(1); // Monochrome
  display.setColorDepth(8);
  display.setTextSize(2); // font is 12x16
  display.setTextColor(WHITE);
  display.createSprite(240, 320); //320x240 rotated 90 degrees
  display_clearDisplay();
  displayEnabled = true;
  return displayEnabled;
}

