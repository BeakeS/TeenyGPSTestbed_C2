
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
// UBX/GPS Logging Modes
enum ubxLogging_mode_t : uint8_t {
  UBXLOG_NAVPVT = 0,
  UBXLOG_NAVSAT,
  UBXLOG_NAVPVTNAVSAT
};
enum gpsLogging_mode_t : uint8_t {
  GPSLOG_NONE = 0,
  GPSLOG_GPX,
  GPSLOG_KML
};

/********************************************************************/
// GPS
bool gpsEnabled;
#include "TeenyGPSConnect.h"
TeenyGPSConnect gps;

