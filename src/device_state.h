
/********************************************************************/
// Device State
typedef struct {
  int16_t  TIMEZONE = 0;
  int16_t  DEVICE_MODE = DM_IDLE;
  int16_t  EMUL_NUMCOLDSTARTPVTPACKETS = 10;
  uint8_t  DISPLAYBRIGHTNESS = 50;
  uint8_t  DISPLAYTIMEOUT = 10;
  bool     STATUSLED = true;
  uint8_t  GPSRESET = GPS_NORESET;
  uint8_t  spare00;
  uint8_t  spare01;
} device_state_t;
device_state_t deviceState_defaults;
device_state_t deviceState;

/********************************************************************/
// KVS
/********************************************************************/
#include "TeenyKVS.h"
uint8_t deviceStateKVSArray[256];
TeenyKVSArray deviceStateKVS(deviceStateKVSArray, sizeof(deviceStateKVSArray));

/********************************************************************/
bool writeDeviceStateKVS() {
  bool rc;
  rc = deviceStateKVS.reset();
  if(!rc) return false;
  rc = deviceStateKVS.set("TIMEZONE", strlen("TIMEZONE"),
                          (uint8_t*)&deviceState.TIMEZONE, sizeof(deviceState.TIMEZONE));
  if(!rc) return false;
  rc = deviceStateKVS.set("DEVICE_MODE", strlen("DEVICE_MODE"),
                          (uint8_t*)&deviceState.DEVICE_MODE, sizeof(deviceState.DEVICE_MODE));
  if(!rc) return false;
  rc = deviceStateKVS.set("EMUL_NUMCOLDSTARTPVTPACKETS", strlen("EMUL_NUMCOLDSTARTPVTPACKETS"),
                          (uint8_t*)&deviceState.EMUL_NUMCOLDSTARTPVTPACKETS, sizeof(deviceState.EMUL_NUMCOLDSTARTPVTPACKETS));
  if(!rc) return false;
  rc = deviceStateKVS.set("DISPLAYBRIGHTNESS", strlen("DISPLAYBRIGHTNESS"),
                          (uint8_t*)&deviceState.DISPLAYBRIGHTNESS, sizeof(deviceState.DISPLAYBRIGHTNESS));
  if(!rc) return false;
  rc = deviceStateKVS.set("DISPLAYTIMEOUT", strlen("DISPLAYTIMEOUT"),
                          (uint8_t*)&deviceState.DISPLAYTIMEOUT, sizeof(deviceState.DISPLAYTIMEOUT));
  if(!rc) return false;
  rc = deviceStateKVS.set("STATUSLED", strlen("STATUSLED"),
                          (uint8_t*)&deviceState.STATUSLED, sizeof(deviceState.STATUSLED));
  if(!rc) return false;
  rc = deviceStateKVS.set("GPSRESET", strlen("GPSRESET"),
                          (uint8_t*)&deviceState.GPSRESET, sizeof(deviceState.GPSRESET));
  if(!rc) return false;
  return true;
}

/********************************************************************/
bool readDeviceStateKVS() {
  bool rc;
  rc = deviceStateKVS.get("TIMEZONE", strlen("TIMEZONE"),
                          (uint8_t*)&deviceState.TIMEZONE, sizeof(deviceState.TIMEZONE));
  if(!rc) return false;
  rc = deviceStateKVS.get("DEVICE_MODE", strlen("DEVICE_MODE"),
                          (uint8_t*)&deviceState.DEVICE_MODE, sizeof(deviceState.DEVICE_MODE));
  if(!rc) return false;
  rc = deviceStateKVS.get("EMUL_NUMCOLDSTARTPVTPACKETS", strlen("EMUL_NUMCOLDSTARTPVTPACKETS"),
                          (uint8_t*)&deviceState.EMUL_NUMCOLDSTARTPVTPACKETS, sizeof(deviceState.EMUL_NUMCOLDSTARTPVTPACKETS));
  if(!rc) return false;
  rc = deviceStateKVS.get("DISPLAYBRIGHTNESS", strlen("DISPLAYBRIGHTNESS"),
                          (uint8_t*)&deviceState.DISPLAYBRIGHTNESS, sizeof(deviceState.DISPLAYBRIGHTNESS));
  if(!rc) return false;
  rc = deviceStateKVS.get("DISPLAYTIMEOUT", strlen("DISPLAYTIMEOUT"),
                          (uint8_t*)&deviceState.DISPLAYTIMEOUT, sizeof(deviceState.DISPLAYTIMEOUT));
  if(!rc) return false;
  rc = deviceStateKVS.get("STATUSLED", strlen("STATUSLED"),
                          (uint8_t*)&deviceState.STATUSLED, sizeof(deviceState.STATUSLED));
  if(!rc) return false;
  rc = deviceStateKVS.get("GPSRESET", strlen("GPSRESET"),
                          (uint8_t*)&deviceState.GPSRESET, sizeof(deviceState.GPSRESET));
  if(!rc) return false;
  return true;
}