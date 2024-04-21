
/********************************************************************/
// SD Card
/********************************************************************/
bool sdcardEnabled;
#include <SPI.h>
#include <SD.h>
// m5stack core2 chip select = 4
const int sdChipSelect = 4;
File sdFile; // SD file pointer
File sdFile_gpx; // SD file pointer

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
bool     ubxLoggingInProgress = false;
uint8_t  ubxLoggingFileNum = 0;
char     ubxLoggingFileName[14]={0};
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
// GPX Logging File Writer
// See "https://www.topografix.com/GPX/1/1/" for details
/********************************************************************/
uint8_t  gpxLoggingFileNum = 0;
char     gpxLoggingFileName[14]={0};
uint16_t gpxLoggingFileWriteCount;
/********************************************************************/
bool sdcard_openGPXLoggingFile() {
  if(!sdcardEnabled) return false;
  sprintf(gpxLoggingFileName, "/GPSLOG%02d.gpx", gpxLoggingFileNum);
  if(SD.exists(gpxLoggingFileName)) {
    if(!SD.remove(gpxLoggingFileName)) return false;
  }
  //SdFile::dateTimeCallback(sdDateTimeCB);
  sdFile_gpx = SD.open(gpxLoggingFileName, FILE_WRITE);
  if(!sdFile_gpx) return false;
  sdFile_gpx.write((uint8_t*)"<gpx version=\"1.1\" creator=\"TeenyGPSTestbed\">\n",
                   strlen("<gpx version=\"1.1\" creator=\"TeenyGPSTestbed\">\n"));
  sdFile_gpx.write((uint8_t*)"  <trk>\n",
                   strlen("  <trk>\n"));
  sdFile_gpx.write((uint8_t*)"    <trkseg>\n",
                   strlen("    <trkseg>\n"));
  gpxLoggingFileWriteCount = 0;
  return true;
}
/********************************************************************/
void sdcard_writeGPXLoggingFile(const uint8_t *buf, size_t size) {
  sdFile_gpx.write(buf, size);
  gpxLoggingFileWriteCount++;
}
/********************************************************************/
uint16_t sdcard_closeGPXLoggingFile() {
  sdFile_gpx.write((uint8_t*)"    </trkseg>\n",
                   strlen("    </trkseg>\n"));
  sdFile_gpx.write((uint8_t*)"  </trk>\n",
                   strlen("  </trk>\n"));
  sdFile_gpx.write((uint8_t*)"</gpx>\n",
                   strlen("</gpx>\n"));
  sdFile_gpx.close();
  gpxLoggingFileNum++;
  if(gpxLoggingFileNum > 99) gpxLoggingFileNum = 0;
  return gpxLoggingFileWriteCount;
}

/********************************************************************/
// UBX Emulation Loop File Reader
/********************************************************************/
// TBD

