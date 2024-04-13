
/********************************************************************/
// GPS Startup Modes
enum gpsReset_mode_t : uint8_t {
  GPS_NORESET = 0,
  GPS_HOTSTART,
  GPS_WARMSTART,
  GPS_COLDSTART,
  GPS_HARDWARERESET
};

/********************************************************************/
// GPS Logging Modes
enum gpsLogging_mode_t : uint8_t {
  GPSLOG_NAVPVT = 0,
  GPSLOG_NAVSAT,
  GPSLOG_NAVPVTNAVSAT
};

/********************************************************************/
// GPS
bool gpsEnabled;
#include "TeenyGPSConnect.h"
TeenyGPSConnect gps;

