
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
  {"PVTLOG", DM_GPSLOGR},
  {"NAVSAT", DM_GPSNSAT},
  {"SATMAP", DM_GPSSMAP},
//{"GPSCPT", DM_GPSCAPT},
//{"GPSSST", DM_GPSSSTP},
  {"GPSEMU", DM_GPSEMUL}};
TeenyMenuSelect selectMenuMode(sizeof(selectMenuModeOptions)/sizeof(SelectOptionUint8t), selectMenuModeOptions);
TeenyMenuItem menuItemMenuMode("Device Mode", MENU_MODE, selectMenuMode, menu_menuModeCB);
//
// gps receiver unit
void menu_entrGPSRcvrCB(); // forward declaration
void menu_exitGPSRcvrCB(); // forward declaration
TeenyMenuPage menuPageGPSRcvr("GPS RECEIVER MODE", menu_entrGPSRcvrCB, menu_exitGPSRcvrCB);
TeenyMenuItem menuItemGPSRcvr("*START GPS RCVR*", menuPageGPSRcvr);
TeenyMenuItem menuItemGPSRcvrExit(false); // optional return menu item
TeenyMenuItem menuItemGPSRcvrLabel0("");
TeenyMenuItem menuItemGPSRcvrLabel1("");
//
// gps logging unit
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
void menu_startGPSLogrCB(); // forward declaration
TeenyMenuItem menuItemGPSLogrStrtLog("Start PVT Logging", menu_startGPSLogrCB);
void menu_stopGPSLogrCB(); // forward declaration
TeenyMenuItem menuItemGPSLogrStopLog("Stop PVT Logging", menu_stopGPSLogrCB);
//
// gps navsat unit
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
void menu_entrGPSSmapCB(); // forward declaration
void menu_exitGPSSmapCB(); // forward declaration
TeenyMenuPage menuPageGPSSmap("CONSTELLATION MAP", menu_entrGPSSmapCB, menu_exitGPSSmapCB);
TeenyMenuItem menuItemGPSSmap("*START GPS SATMAP*", menuPageGPSSmap);
TeenyMenuItem menuItemGPSSmapLabel0("");
TeenyMenuItem menuItemGPSSmapExit(false); // optional return menu item
//
// gps transceiver unit
void menu_entrGPSCaptCB(); // forward declaration
void menu_exitGPSCaptCB(); // forward declaration
TeenyMenuPage menuPageGPSCapt("GPS CAPTURE MODE", menu_entrGPSCaptCB, menu_exitGPSCaptCB);
TeenyMenuItem menuItemGPSCapt("*START GPS CAPTURE*", menuPageGPSCapt);
TeenyMenuItem menuItemGPSCaptExit(false); // optional return menu item
TeenyMenuItem menuItemGPSCaptLabel0("");
bool menu_captRxPktInProgress = false;
void menu_captStrtRxPktCB(); // forward declaration
TeenyMenuItem menuItemGPSCaptStrtRxPkt("Capture RX PKT", menu_captStrtRxPktCB);
void menu_captStopRxPktCB(); // forward declaration
TeenyMenuItem menuItemGPSCaptStopRxPkt("Save RX PKT", menu_captStopRxPktCB);
//
// gps single step unit
void menu_entrGPSStepCB(); // forward declaration
void menu_exitGPSStepCB(); // forward declaration
TeenyMenuPage menuPageGPSStep("GPS SINGLESTEP MODE", menu_entrGPSStepCB, menu_exitGPSStepCB);
TeenyMenuItem menuItemGPSStep("*START GPS STEPPER*", menuPageGPSStep);
TeenyMenuItem menuItemGPSStepExit(false); // optional return menu item
void menu_sstBeginCB(); // forward declaration
TeenyMenuItem menuItemGPSStepBegin("gnss.begin", menu_sstBeginCB);
void menu_sstSetBaudCB(); // forward declaration
TeenyMenuItem menuItemGPSStepSetBaud("gnss.setSerialRate", menu_sstSetBaudCB);
void menu_sstSaveConfigCB(); // forward declaration
TeenyMenuItem menuItemGPSStepSaveCfg("gnss.saveConfig", menu_sstSaveConfigCB);
void menu_sstGetVersionCB(); // forward declaration
TeenyMenuItem menuItemGPSStepGetVersion("gnss.getVersion", menu_sstGetVersionCB);
void menu_sstSetPortOutputCB(); // forward declaration
TeenyMenuItem menuItemGPSStepSetPortOutput("gnss.setPortOutput", menu_sstSetPortOutputCB);
void menu_sstSetMeasRateCB(); // forward declaration
TeenyMenuItem menuItemGPSStepMeasRate("gnss.setMeasRate", menu_sstSetMeasRateCB);
void menu_sstSetNavRateCB(); // forward declaration
TeenyMenuItem menuItemGPSStepSetNavRate("gnss.setNavRate", menu_sstSetNavRateCB);
void menu_sstTogAutoPVTCB(); // forward declaration
TeenyMenuItem menuItemGPSStepTogAutoPVT("gnss.togAutoPVT", menu_sstTogAutoPVTCB);
void menu_sstReqCfgPrtCB(); // forward declaration
TeenyMenuItem menuItemGPSStepReqCfgPrt("reqCfgPrt_CMD", menu_sstReqCfgPrtCB);
void menu_sstReqCfgNavx5CB(); // forward declaration
TeenyMenuItem menuItemGPSStepReqCfgNavx5("reqCfgNavx5_CMD", menu_sstReqCfgNavx5CB);
void menu_sstColdStartCB(); // forward declaration
TeenyMenuItem menuItemGPSStepcoldStart("coldStart_CMD", menu_sstColdStartCB);
void menu_sstReqMonVerCB(); // forward declaration
TeenyMenuItem menuItemGPSStepReqMonVer("reqMonVer_CMD", menu_sstReqMonVerCB);
void menu_sstReqMonSmgrCB(); // forward declaration
TeenyMenuItem menuItemGPSStepReqMonSmgr("reqMonSmgr_CMD", menu_sstReqMonSmgrCB);
void menu_sstSetCfgPrtCB(); // forward declaration
TeenyMenuItem menuItemGPSStepSetCfgPrt("setCfgPrt_CMD", menu_sstSetCfgPrtCB);
void menu_sstReqCfgRateCB(); // forward declaration
TeenyMenuItem menuItemGPSStepReqCfgRate("reqCfgRate_CMD", menu_sstReqCfgRateCB);
void menu_sstSetCfgRateCB(); // forward declaration
TeenyMenuItem menuItemGPSStepSetCfgRate("setCfgRate_CMD", menu_sstSetCfgRateCB);
void menu_sstSetCfgCfgCB(); // forward declaration
TeenyMenuItem menuItemGPSStepSetCfgCfg("setCfgCfg_CMD", menu_sstSetCfgCfgCB);
void menu_sstSetCfgMsgCB(); // forward declaration
TeenyMenuItem menuItemGPSStepSetCfgMsg("setCfgMsg_CMD", menu_sstSetCfgMsgCB);
void menu_sstReqNavPvtCB(); // forward declaration
TeenyMenuItem menuItemGPSStepReqNavPvt("reqNavPvt_CMD", menu_sstReqNavPvtCB);
//
// gps emulator unit
void menu_entrGPSEmulCB(); // forward declaration
void menu_exitGPSEmulCB(); // forward declaration
TeenyMenuPage menuPageGPSEmul("GPS EMULATOR MODE", menu_entrGPSEmulCB, menu_exitGPSEmulCB);
TeenyMenuItem menuItemGPSEmul("*START EMULATOR*", menuPageGPSEmul);
TeenyMenuItem menuItemGPSEmulExit(false); // optional return menu item
TeenyMenuItem menuItemGPSEmulLabel0("");
TeenyMenuItem menuItemGPSEmulLabel1("");
//
// gps reset
void menu_GPSResetCB(); // forward declaration
SelectOptionUint8t selectGPSResetOptions[] = {
  {"NO",   GPS_NORESET},
  {"HOT",  GPS_HOTSTART},
  {"WARM", GPS_WARMSTART},
  {"COLD", GPS_COLDSTART}};
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
TeenyMenuItem menuItemRTCSetDateTime("Set Date/Time", menu_setRTC_CB);
//
// EMULATOR SETTINGS
//
// emulator settings menu
TeenyMenuPage menuPageEMULSettings("EMULATOR SETTINGS", menu_getRTC_CB);
TeenyMenuItem menuItemEMULSettings("Emulator Settings", menuPageEMULSettings);
TeenyMenuItem menuItemEMULSettingsExit(false); // optional return menu item
//
// display timeout
int16_t menuColdStartPVTPktsMin = 0;
int16_t menuColdStartPVTPktsMax = 60;
TeenyMenuItem menuItemColdStartPVTPkts("ColdStrtPkts", deviceState.EMUL_NUMCOLDSTARTPVTPACKETS, menuColdStartPVTPktsMin, menuColdStartPVTPktsMax);
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
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrStrtLog);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrStopLog);
  //menuPageGPSLogr.addMenuItem(menuItemGPSLogrExit);
  menuPageMain.addMenuItem(menuItemGPSNsat);
  menuPageGPSNsat.addMenuItem(menuItemGPSNsatLabel0);
  //menuPageGPSNsat.addMenuItem(menuItemGPSNsatExit);
  menuPageMain.addMenuItem(menuItemGPSSmap);
  menuPageGPSSmap.addMenuItem(menuItemGPSSmapLabel0);
  //menuPageGPSSmap.addMenuItem(menuItemGPSSmapExit);
  menuPageMain.addMenuItem(menuItemGPSCapt);
  menuPageGPSCapt.addMenuItem(menuItemGPSCaptLabel0);
  menuPageGPSCapt.addMenuItem(menuItemGPSCaptStrtRxPkt);
  menuPageGPSCapt.addMenuItem(menuItemGPSCaptStopRxPkt);
  //menuPageGPSCapt.addMenuItem(menuItemGPSCaptExit);
  menuPageMain.addMenuItem(menuItemGPSStep);
  menuPageGPSStep.addMenuItem(menuItemGPSStepBegin);
  menuPageGPSStep.addMenuItem(menuItemGPSStepSetBaud);
  menuPageGPSStep.addMenuItem(menuItemGPSStepSaveCfg);
  menuPageGPSStep.addMenuItem(menuItemGPSStepGetVersion);
  menuPageGPSStep.addMenuItem(menuItemGPSStepSetPortOutput);
  menuPageGPSStep.addMenuItem(menuItemGPSStepMeasRate);
  menuPageGPSStep.addMenuItem(menuItemGPSStepSetNavRate);
  menuPageGPSStep.addMenuItem(menuItemGPSStepTogAutoPVT);
  menuPageGPSStep.addMenuItem(menuItemGPSStepReqCfgPrt);
  menuPageGPSStep.addMenuItem(menuItemGPSStepReqCfgNavx5);
  menuPageGPSStep.addMenuItem(menuItemGPSStepcoldStart);
  menuPageGPSStep.addMenuItem(menuItemGPSStepReqMonVer);
  menuPageGPSStep.addMenuItem(menuItemGPSStepReqMonSmgr);
  menuPageGPSStep.addMenuItem(menuItemGPSStepSetCfgPrt);
  menuPageGPSStep.addMenuItem(menuItemGPSStepReqCfgRate);
  menuPageGPSStep.addMenuItem(menuItemGPSStepSetCfgRate);
  menuPageGPSStep.addMenuItem(menuItemGPSStepSetCfgCfg);
  menuPageGPSStep.addMenuItem(menuItemGPSStepSetCfgMsg);
  menuPageGPSStep.addMenuItem(menuItemGPSStepReqNavPvt);
  //menuPageGPSStep.addMenuItem(menuItemGPSStepExit);
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
  menuPageTopLevelSettings.addMenuItem(menuItemEMULSettings);
  menuPageEMULSettings.addMenuItem(menuItemColdStartPVTPkts);
  menuPageEMULSettings.addMenuItem(menuItemEMULSettingsExit); // optional return menu item
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
    case DM_GPSCAPT:
      menu.linkMenuPage(menuPageGPSCapt);
      break;
    case DM_GPSSSTP:
      menu.linkMenuPage(menuPageGPSStep);
      break;
    case DM_GPSEMUL:
      menu.linkMenuPage(menuPageGPSEmul);
      break;
  }
  // init and enter menu
  menu.setTextColor(WHITE, BLACK);
  menuDisplaySleepMode = false;
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
      M5.Lcd.sleep();
    }
  } else if(menuDisplaySleepMode) {
    menuDisplaySleepMode = false;
    M5.Lcd.wakeup();
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
      menuItemGPSReset.hide();
      menuItemLabel2.show();
      menuItemGPSStep.hide();
      menuItemGPSCapt.hide();
      menuItemGPSEmul.hide();
      menuItemLabel3.show();
      break;
    case DM_GPSRCVR:
      menuItemGPSRcvr.show();
      menuItemGPSLogr.hide();
      menuItemGPSNsat.hide();
      menuItemGPSSmap.hide();
      menuItemGPSReset.show();
      menuItemLabel2.hide();
      menuItemGPSStep.hide();
      menuItemGPSCapt.hide();
      menuItemGPSEmul.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSLOGR:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.show();
      menuItemGPSNsat.hide();
      menuItemGPSSmap.hide();
      menuItemGPSReset.show();
      menuItemLabel2.hide();
      menuItemGPSStep.hide();
      menuItemGPSCapt.hide();
      menuItemGPSEmul.hide();
      menuItemLabel3.hide();
      menuItemGPSLogrStrtLog.hide(ubxLoggingInProgress ? true : false);
      menuItemGPSLogrStopLog.hide(ubxLoggingInProgress ? false : true);
      break;
    case DM_GPSNSAT:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSNsat.show();
      menuItemGPSSmap.hide();
      menuItemGPSReset.show();
      menuItemLabel2.hide();
      menuItemGPSStep.hide();
      menuItemGPSCapt.hide();
      menuItemGPSEmul.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSSMAP:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSNsat.hide();
      menuItemGPSSmap.show();
      menuItemGPSReset.show();
      menuItemLabel2.hide();
      menuItemGPSStep.hide();
      menuItemGPSCapt.hide();
      menuItemGPSEmul.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSCAPT:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSNsat.hide();
      menuItemGPSSmap.hide();
      menuItemGPSReset.hide();
      menuItemLabel2.show();
      menuItemGPSStep.hide();
      menuItemGPSCapt.show();
      menuItemGPSEmul.hide();
      menuItemLabel3.hide();
      menuItemGPSCaptStrtRxPkt.hide(menu_captRxPktInProgress ? true : false);
      menuItemGPSCaptStopRxPkt.hide(menu_captRxPktInProgress ? false : true);
      break;
    case DM_GPSSSTP:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSNsat.hide();
      menuItemGPSSmap.hide();
      menuItemGPSReset.hide();
      menuItemLabel2.show();
      menuItemGPSStep.show();
      menuItemGPSCapt.hide();
      menuItemGPSEmul.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSEMUL:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSNsat.hide();
      menuItemGPSSmap.hide();
      menuItemGPSReset.hide();
      menuItemLabel2.show();
      menuItemGPSStep.hide();
      menuItemGPSCapt.hide();
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
  if(sdcard_openLoggingFile()) {
    ubxLoggingInProgress = true;
    menuItemGPSLogrStrtLog.hide();
    menuItemGPSLogrStopLog.show();
    msg_update("PVT Logging Started");
  } else {
    msg_update("SD Card Error");
  }
  displayRefresh = true;
}

/********************************************************************/
void menu_stopGPSLogrCB() {
  char _msgStr[22];
  if(!ubxLoggingInProgress) return;
  sdcard_closeLoggingFile();
  ubxLoggingInProgress = false;
  menuItemGPSLogrStrtLog.show();
  menuItemGPSLogrStopLog.hide();
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
void menu_entrGPSCaptCB() {
  deviceState.DEVICE_MODE = DM_GPSCAPT;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_captStrtRxPktCB() {
  if(menu_captRxPktInProgress) return;
  if(sdcard_openRxPktFile()) {
    menu_captRxPktInProgress = true;
    menuItemGPSCaptStrtRxPkt.hide();
    menuItemGPSCaptStopRxPkt.show();
    msg_update("Ready for RX PKT");
  } else {
    msg_update("SD Card Error");
  }
  displayRefresh = true;
}

/********************************************************************/
void menu_captStopRxPktCB() {
  char _msgStr[22];
  if(!menu_captRxPktInProgress) return;
  sdcard_closeRxPktFile();
  menu_captRxPktInProgress = false;
  menuItemGPSCaptStrtRxPkt.show();
  menuItemGPSCaptStopRxPkt.hide();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSCaptCB() {
  menu_captStopRxPktCB();
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSStepCB() {
  deviceState.DEVICE_MODE = DM_GPSSSTP;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSStepCB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_sstBeginCB() {
  char _msgStr[22];
  gpsSerial->begin(GPS_BAUD_RATE);
  bool rcode = gps.gnss_ss_begin(*gpsSerial);
  sprintf(_msgStr, "gnss.begin rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstSetBaudCB() {
  char _msgStr[22];
  gps.gnss_ss_setSerialRate(GPS_BAUD_RATE);
  sprintf(_msgStr, "gnss.setSerRate");
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstSaveConfigCB() {
  char _msgStr[22];
  while(gpsSerial->available()) gpsSerial->read();
  bool rcode = gps.gnss_ss_saveConfiguration();
  sprintf(_msgStr, "gnss.saveCfg rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstGetVersionCB() {
  char _msgStr[22];
  while(gpsSerial->available()) gpsSerial->read();
  bool rcode = gps.gnss_ss_getProtocolVersion();
  sprintf(_msgStr, "gnss.getPV rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstSetPortOutputCB() {
  char _msgStr[22];
  while(gpsSerial->available()) gpsSerial->read();
  bool rcode = gps.gnss_ss_setPortOutput(COM_PORT_UART1, COM_TYPE_UBX);
  sprintf(_msgStr, "gnss.setPtOut rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstSetMeasRateCB() {
  char _msgStr[22];
  while(gpsSerial->available()) gpsSerial->read();
  bool rcode = gps.gnss_ss_setMeasurementRate(1000);
  sprintf(_msgStr, "gnss.setMR rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstSetNavRateCB() {
  char _msgStr[22];
  while(gpsSerial->available()) gpsSerial->read();
  bool rcode = gps.gnss_ss_setNavigationRate(1);
  sprintf(_msgStr, "gnss.setNR rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstTogAutoPVTCB() {
  char _msgStr[22];
  static bool enable = true;
  while(gpsSerial->available()) gpsSerial->read();
  bool rcode = gps.gnss_ss_setAutoNAVPVT(enable);
  sprintf(_msgStr, "gnss.setANPVT rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
  enable = !enable;
}

/********************************************************************/
void menu_sstReqCfgPrtCB() {
  char _msgStr[22];
  uint8_t host_reqCFGPRT[9]  = {0xb5,0x62,0x06,0x00,0x01,0x00,0x01,0x08,0x22};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_reqCFGPRT, sizeof(host_reqCFGPRT));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstReqCfgNavx5CB() {
  char _msgStr[22];
  uint8_t host_reqCFGNAVX5[8]  = {0xb5,0x62,0x06,0x23,0x00,0x00,0x29,0x81};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_reqCFGNAVX5, sizeof(host_reqCFGNAVX5));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstColdStartCB() {
  char _msgStr[22];
  const uint8_t host_coldStart[12] = {0xB5,0x62,0x06,0x04,0x04,0x00,0xFF,0xB9,0x02,0x00,0xC8,0x8F};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_coldStart, sizeof(host_coldStart));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstReqMonVerCB() {
  char _msgStr[22];
  uint8_t host_reqMONVER[8]  = {0xb5,0x62,0x0a,0x04,0x00,0x00,0x0e,0x34};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_reqMONVER, sizeof(host_reqMONVER));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstReqMonSmgrCB() {
  char _msgStr[22];
  uint8_t host_reqMONSMGR[8]  = {0xb5,0x62,0x0a,0x2e,0x00,0x00,0x38,0xb2};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_reqMONSMGR, sizeof(host_reqMONSMGR));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstSetCfgPrtCB() {
  char _msgStr[22];
  uint8_t host_setCFGPRT[28]  = {0xb5,0x62,0x06,0x00,0x14,0x00,0x01,0x00,0x00,0x00,0xc0,0x08,0x00,0x00,0x00,0x96,0x00,0x00,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x81,0x84};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_setCFGPRT, sizeof(host_setCFGPRT));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstReqCfgRateCB() {
  char _msgStr[22];
  uint8_t host_reqCFGRATE[8]  = {0xb5,0x62,0x06,0x08,0x00,0x00,0x0e,0x30};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_reqCFGRATE, sizeof(host_reqCFGRATE));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstSetCfgRateCB() {
  char _msgStr[22];
  uint8_t host_setCFGRATE[14] = {0xb5,0x62,0x06,0x08,0x06,0x00,0xe8,0x03,0x01,0x00,0x01,0x00,0x01,0x39};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_setCFGRATE, sizeof(host_setCFGRATE));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstSetCfgCfgCB() {
  char _msgStr[22];
  uint8_t host_setCFGCFG[20]  = {0xb5,0x62,0x06,0x09,0x0c,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x80};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_setCFGCFG, sizeof(host_setCFGCFG));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstSetCfgMsgCB() {
  char _msgStr[22];
  static bool enable = true;
  uint8_t host_setCFGMSG1[11]  = {0xb5,0x62,0x06,0x01,0x03,0x00,0x01,0x07,0x01,0x13,0x51};
  uint8_t host_setCFGMSG0[11]  = {0xb5,0x62,0x06,0x01,0x03,0x00,0x01,0x07,0x00,0x12,0x50};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  if(enable) {
    gpsSerial->write(host_setCFGMSG1, sizeof(host_setCFGMSG1));
  } else {
    gpsSerial->write(host_setCFGMSG0, sizeof(host_setCFGMSG0));
  }
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
  enable = !enable;
}

/********************************************************************/
void menu_sstReqNavPvtCB() {
  char _msgStr[22];
  uint8_t host_reqNAVPVT[8]  = {0xb5,0x62,0x01,0x07,0x00,0x00,0x08,0x19};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_reqNAVPVT, sizeof(host_reqNAVPVT));
  while((millis()-_nowMS)<1500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
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
    rtc_datetime_t now = rtc.getRTCTime(deviceState.TIMEZONE); // get the RTC
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
  msg_update("RTC Clock Set");
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

