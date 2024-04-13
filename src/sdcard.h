
/********************************************************************/
// SD Card
/********************************************************************/
bool sdcardEnabled;
#include <SPI.h>
#include <SD.h>
// m5stack core2 chip select = 4
const int sdChipSelect = 4;
File sdFile; // SD file pointer

/********************************************************************/
bool sdcard_setup() {
  if(SD.begin(sdChipSelect)) {
    sdcardEnabled = true;
  } else {
    sdcardEnabled = false;
  }
  return sdcardEnabled;
}

/********************************************************************/
//void sdDateTimeCB(uint16_t* date, uint16_t* time) {
//  rtc_datetime_t now = getRTCTime(); // get the RTC
//  // return date using FAT_DATE macro to format fields
//  *date = FAT_DATE(now.year, now.month, now.day);
//  // return time using FAT_TIME macro to format fields
//  *time = FAT_TIME(now.hour, now.minute, now.second);
//}

/********************************************************************/
// Config File Save/restore
/********************************************************************/
bool sdcard_deviceStateReset() {
  if(!sdcardEnabled) return false;
  if(SD.exists("/TEENYGPS.cfg")) {
    if(!SD.remove("/TEENYGPS.cfg")) return false;
  }
  return true;
}
/********************************************************************/
bool sdcard_deviceStateSave() {
  if(!writeDeviceStateKVS()) return false;
  if(!sdcard_deviceStateReset()) return false;
  //SdFile::dateTimeCallback(sdDateTimeCB);
  sdFile = SD.open("/TEENYGPS.cfg", FILE_WRITE);
  if(!sdFile) return false;
  sdFile.write(deviceStateKVSArray, deviceStateKVS.used_bytes());
  sdFile.close();
  return true;
}
/********************************************************************/
bool sdcard_deviceStateRestore() {
  if(sdcardEnabled && SD.exists("/TEENYGPS.cfg")) {
    sdFile = SD.open("/TEENYGPS.cfg");
    if(sdFile) {
      sdFile.read(deviceStateKVSArray, min(sdFile.size(), sizeof(deviceStateKVSArray)));
      sdFile.close();
      if(readDeviceStateKVS()) {
        return true;
      }
      sdcard_deviceStateReset();
    }
  }
  deviceState = deviceState_defaults;
  return false;
}

/********************************************************************/
// UBX Logging File Writer
/********************************************************************/
bool ubxLoggingInProgress = false;
uint8_t ubxLoggingFileNum = 0;
char ubxLoggingFileName[14]={0};
uint16_t ubxLoggingFileWriteCount;
uint16_t ubxLoggingFileWriteValidCount;
/********************************************************************/
bool sdcard_openLoggingFile() {
  if(!sdcardEnabled) return false;
  sprintf(ubxLoggingFileName, "/GPSLOG%02d.hex", ubxLoggingFileNum);
  if(SD.exists(ubxLoggingFileName)) {
    if(!SD.remove(ubxLoggingFileName)) return false;
  }
  //SdFile::dateTimeCallback(sdDateTimeCB);
  sdFile = SD.open(ubxLoggingFileName, FILE_WRITE);
  if(!sdFile) return false;
  ubxLoggingFileWriteCount = 0;
  ubxLoggingFileWriteValidCount = 0;
  return true;
}
/********************************************************************/
void sdcard_writeLoggingFile(const uint8_t *buf, size_t size) {
  sdFile.write(buf, size);
  //ubxLoggingFileWriteCount++; //done in loop along with fixed count
}
/********************************************************************/
uint16_t sdcard_closeLoggingFile() {
  sdFile.close();
  ubxLoggingFileNum++;
  if(ubxLoggingFileNum > 99) ubxLoggingFileNum = 0;
  return ubxLoggingFileWriteCount;
}

/********************************************************************/
// UBX Emulation Loop File Reader
/********************************************************************/
// TBD

/********************************************************************/
// Rx Pkt File Writer
/********************************************************************/
uint8_t rxPktFileNum = 0;
char rxPktFileName[14]={0};
uint16_t rxPktWriteCount;
/********************************************************************/
bool sdcard_openRxPktFile() {
  if(!sdcardEnabled) return false;
  rxPktWriteCount = 0;
  while(gpsSerial->available()) gpsSerial->read();
  sprintf(rxPktFileName, "/RXPKT%03d.hex", rxPktFileNum);
  if(SD.exists(rxPktFileName)) {
    if(!SD.remove(rxPktFileName)) return false;
  }
  //SdFile::dateTimeCallback(sdDateTimeCB);
  sdFile = SD.open(rxPktFileName, FILE_WRITE);
  if(!sdFile) return false;
  return true;
}
/********************************************************************/
void sdcard_writeRxPktFile() {
  while(gpsSerial->available()) {
    sdFile.write(gpsSerial->read());
    rxPktWriteCount++;
  }
}
/********************************************************************/
uint16_t sdcard_closeRxPktFile() {
  sdFile.close();
  rxPktFileNum++;
  return rxPktWriteCount;
}

