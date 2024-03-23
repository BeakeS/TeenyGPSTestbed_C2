/*
TeenyRTC - a small library for Arduino providing RTC support for multiple platforms

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Arduino.h>
#include "TeenyRTC.h"


/********************************************************************/
TeenyRTC::TeenyRTC() { }

/********************************************************************/
void TeenyRTC::setValid(bool valid) {
  _valid = valid;
}
bool TeenyRTC::isValid() {
  return _valid;
}

/********************************************************************/
// Get ISO8601 string from date/time
char* TeenyRTC::getISO8601DateTimeStr(rtc_datetime_t dateTime) {
  //e.g. "2020-06-25T15:29:37"
  static char _itdStr[20];
  sprintf(_itdStr, "%u-%02d-%02dT%02d:%02d:%02d",
          dateTime.year, dateTime.month, dateTime.day,
          dateTime.hour, dateTime.minute, dateTime.second);
  return _itdStr;
}

/********************************************************************/
// Get ISO8601 string from date/time with time zone offset
char* TeenyRTC::getISO8601DateTimeStr(rtc_datetime_t dateTime,
                                      int16_t timeZoneOffset) {
  return getISO8601DateTimeStr(getOffsetDateTime(dateTime, timeZoneOffset));
}

/********************************************************************/
// Extract date/time from ISO8601 string
rtc_datetime_t TeenyRTC::extractISO8601DateTime(const char* iso8601DateTimeStr) {
  char ref[] = "2000-01-01T00:00:00";
  rtc_datetime_t _dateTime;
  memcpy(ref, iso8601DateTimeStr, min(strlen(ref), strlen(iso8601DateTimeStr)));
  _dateTime.year   = (ref[0]  - '0') * 1000 +
                     (ref[1]  - '0') * 100 +
                     (ref[2]  - '0') * 10 +
                     (ref[3]  - '0');
  _dateTime.month  = (ref[5]  - '0') * 10 +
                     (ref[6]  - '0');
  _dateTime.day    = (ref[8]  - '0') * 10 +
                     (ref[9]  - '0');
  _dateTime.hour   = (ref[11] - '0') * 10 +
                     (ref[12] - '0');
  _dateTime.minute = (ref[14] - '0') * 10 +
                     (ref[15] - '0');
  _dateTime.second = (ref[17] - '0') * 10 +
                     (ref[18] - '0');
  return _dateTime;
}

/********************************************************************/
// Extract date/time from ISO8601 string with time zone offset
rtc_datetime_t TeenyRTC::extractISO8601DateTime(const char* iso8601DateTimeStr,
                                                int16_t timeZoneOffset) {
  return getOffsetDateTime(extractISO8601DateTime(iso8601DateTimeStr), timeZoneOffset);
}

/********************************************************************/
// Convert dateTime based on time zone offset in minutes
rtc_datetime_t TeenyRTC::getOffsetDateTime(rtc_datetime_t dateTime, int16_t timeZoneOffset) {
  rtc_datetime_t _localTime = dateTime;
  // No offset - GMT
  if(timeZoneOffset == 0) return _localTime;
  // Negative offset
  if(timeZoneOffset < 0) {
    int16_t _hourminutes = (_localTime.hour * 60) + _localTime.minute;
    // To avoid negative numbers in the calculation,
    // add hourminutes and subtract one day
    _hourminutes += (1440 + timeZoneOffset); // remember - offset is negative!
    _localTime.day--;
    // twenty-four hours (1440 minutes) make one day
    if(_hourminutes >= 1440) {
      _hourminutes -= 1440;
      _localTime.day++;
    }
    _localTime.minute = _hourminutes % 60;
    _localTime.hour = _hourminutes / 60;
    // change "zeroth" day of month to last day of previous month
    if(_localTime.day == 0) {
      _localTime.month--;
      if(_localTime.month == 0) {
        _localTime.month = 12;
        _localTime.year--;
      }
      if((_localTime.month == 4) || (_localTime.month == 6) ||
         (_localTime.month == 9) || (_localTime.month == 11)) {
        _localTime.day = 30;
      } else if(_localTime.month == 2) {
        if((_localTime.year % 4) == 0) {
          // this will fail in the year 2100
          _localTime.day = 29;
        } else {
          _localTime.day = 28;
        }
      } else {
        _localTime.day = 31;
      }
    }
  // Positive offset
  } else {
    _localTime.minute += timeZoneOffset % 60;
    if(_localTime.minute >= 60) {
      _localTime.minute -= 60;
      _localTime.hour++;
    }
    _localTime.hour += timeZoneOffset / 60;
    if(_localTime.hour >= 24) {
      _localTime.hour -= 24;
      _localTime.day++;
    }
    if((_localTime.month == 4) || (_localTime.month == 6) ||
       (_localTime.month == 9) || (_localTime.month == 11)) {
      if(_localTime.day == 31) {
        _localTime.month++;
        _localTime.day = 1;
      }
    } else if(_localTime.month == 2) {
      if((_localTime.year % 4) == 0) {
        // this will fail in the year 2100
        if(_localTime.day == 30) {
          _localTime.month++;
          _localTime.day = 1;
        }
      } else {
        if(_localTime.day == 29) {
          _localTime.month++;
          _localTime.day = 1;
        }
      }
    } else {
      if(_localTime.day == 32) {
        _localTime.month++;
        _localTime.day = 1;
      }
      if(_localTime.month == 13) {
        _localTime.month = 1;
        _localTime.year++;
      }
    }
  }
  return _localTime;
}


#ifdef M5_CORE2
#include <M5Core2.h>
/********************************************************************/
// Core2 RTC
/********************************************************************/
TeenyCore2RTC::TeenyCore2RTC() { }

TeenyCore2RTC::~TeenyCore2RTC() { }

/********************************************************************/
bool TeenyCore2RTC::setup() {
  _valid = false;
  return true;
}

/********************************************************************/
void TeenyCore2RTC::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                               uint8_t hour, uint8_t minute, uint8_t second) {
  RTC_TimeTypeDef rtcTime;
  RTC_DateTypeDef rtcDate;
  rtcDate.Year    = year;
  rtcDate.Month   = month;
  rtcDate.Date    = day;
  rtcTime.Hours   = hour;
  rtcTime.Minutes = minute;
  rtcTime.Seconds = second;
  M5.Rtc.SetTime(&rtcTime);
  M5.Rtc.SetDate(&rtcDate);
  _valid = true;
}
/********************************************************************/
void TeenyCore2RTC::setRTCTime(rtc_datetime_t dateTime) {
  RTC_TimeTypeDef rtcTime;
  RTC_DateTypeDef rtcDate;
  rtcDate.Year    = dateTime.year;
  rtcDate.Month   = dateTime.month;
  rtcDate.Date    = dateTime.day;
  rtcTime.Hours   = dateTime.hour;
  rtcTime.Minutes = dateTime.minute;
  rtcTime.Seconds = dateTime.second;
  M5.Rtc.SetTime(&rtcTime);
  M5.Rtc.SetDate(&rtcDate);
  _valid = true;
}
/********************************************************************/
// Set RTC date/time with time zone offset
void TeenyCore2RTC::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                               uint8_t hour, uint8_t minute, uint8_t second,
                               int16_t timeZoneOffset) {
  rtc_datetime_t _dateTime;
  _dateTime.year   = year;
  _dateTime.month  = month;
  _dateTime.day    = day;
  _dateTime.hour   = hour;
  _dateTime.minute = minute;
  _dateTime.second = second;
  setRTCTime(getOffsetDateTime(_dateTime, timeZoneOffset));
}
/********************************************************************/
// Set RTC date/time with time zone offset
void TeenyCore2RTC::setRTCTime(rtc_datetime_t dateTime, int16_t timeZoneOffset) {
  setRTCTime(getOffsetDateTime(dateTime, timeZoneOffset));
}

/********************************************************************/
rtc_datetime_t TeenyCore2RTC::getRTCTime() {
  rtc_datetime_t _dateTime;
  RTC_TimeTypeDef rtcTime;
  RTC_DateTypeDef rtcDate;
  M5.Rtc.GetTime(&rtcTime);
  M5.Rtc.GetDate(&rtcDate);
  _dateTime.year   = rtcDate.Year;
  _dateTime.month  = rtcDate.Month;
  _dateTime.day    = rtcDate.Date;
  _dateTime.hour   = rtcTime.Hours;
  _dateTime.minute = rtcTime.Minutes;
  _dateTime.second = rtcTime.Seconds;
  return _dateTime;
}
/********************************************************************/
// Get RTC date/time with time zone offset
rtc_datetime_t TeenyCore2RTC::getRTCTime(int16_t timeZoneOffset) {
   return getOffsetDateTime(getRTCTime(), timeZoneOffset);
}
/********************************************************************/
// Get RTC ISO8601 string
char* TeenyCore2RTC::getRTCISO8601DateTimeStr() {
  return getISO8601DateTimeStr(getRTCTime());
}
/********************************************************************/
// Get RTC ISO8601 string with time zone offset
char* TeenyCore2RTC::getRTCISO8601DateTimeStr(int16_t timeZoneOffset) {
  return getISO8601DateTimeStr(getRTCTime(timeZoneOffset));
}
#endif


#ifdef TEENSY41
#include <TimeLib.h>
/********************************************************************/
// Teensy41 RTC
/********************************************************************/
// static function wrapper for getting Teensy's RTC clock
time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

/********************************************************************/
TeenyTeensy41RTC::TeenyTeensy41RTC() { }

TeenyTeensy41RTC::~TeenyTeensy41RTC() { }

/********************************************************************/
bool TeenyTeensy41RTC::setup() {
  _valid = false;
  // set the Time library to use Teensy's RTC to keep time
  setSyncProvider(getTeensy3Time);
  return true;
}

/********************************************************************/
void TeenyTeensy41RTC::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                                  uint8_t hour, uint8_t minute, uint8_t second) {
  setTime(hour, minute, second, day, month, year);
  Teensy3Clock.set(now());
  _valid = true;
}
/********************************************************************/
void TeenyTeensy41RTC::setRTCTime(rtc_datetime_t dateTime) {
  setTime(dateTime.hour, dateTime.minute, dateTime.second,
          dateTime.day, dateTime.month, dateTime.year);
  Teensy3Clock.set(now());
  _valid = true;
}
/********************************************************************/
// Set RTC date/time with time zone offset
void TeenyTeensy41RTC::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                          uint8_t hour, uint8_t minute, uint8_t second,
                          int16_t timeZoneOffset) {
  rtc_datetime_t _dateTime;
  _dateTime.year   = year;
  _dateTime.month  = month;
  _dateTime.day    = day;
  _dateTime.hour   = hour;
  _dateTime.minute = minute;
  _dateTime.second = second;
  setRTCTime(getOffsetDateTime(_dateTime, timeZoneOffset));
}
/********************************************************************/
// Set RTC date/time with time zone offset
void TeenyTeensy41RTC::setRTCTime(rtc_datetime_t dateTime, int16_t timeZoneOffset) {
  setRTCTime(getOffsetDateTime(dateTime, timeZoneOffset));
}

/********************************************************************/
rtc_datetime_t TeenyTeensy41RTC::getRTCTime() {
  time_t _now = getTeensy3Time();
  rtc_datetime_t _dateTime;
  _dateTime.year   = year(_now);
  _dateTime.month  = month(_now);
  _dateTime.day    = day(_now);
  _dateTime.hour   = hour(_now);
  _dateTime.minute = minute(_now);
  _dateTime.second = second(_now);
  return _dateTime;
}
/********************************************************************/
// Get RTC date/time with time zone offset
rtc_datetime_t TeenyTeensy41RTC::getRTCTime(int16_t timeZoneOffset) {
   return getOffsetDateTime(getRTCTime(), timeZoneOffset);
}
/********************************************************************/
// Get RTC ISO8601 string
char* TeenyTeensy41RTC::getRTCISO8601DateTimeStr() {
  return getISO8601DateTimeStr(getRTCTime());
}
/********************************************************************/
// Get RTC ISO8601 string with time zone offset
char* TeenyTeensy41RTC::getRTCISO8601DateTimeStr(int16_t timeZoneOffset) {
  return getISO8601DateTimeStr(getRTCTime(timeZoneOffset));
}
#endif


#ifdef FEATHERM0
#include <RTCZero.h>
/********************************************************************/
// SAMD21 ARM Cortex M0 RTC
/********************************************************************/
TeenyZeroRTC::TeenyZeroRTC() { }

TeenyZeroRTC::~TeenyZeroRTC() { }

/********************************************************************/
bool TeenyZeroRTC::setup() {
  _valid = false;
  rtc.begin();
  return true;
}

/********************************************************************/
void TeenyZeroRTC::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                              uint8_t hour, uint8_t minute, uint8_t second) {
  rtc.setTime(hour, minute, second);
  rtc.setDate(day, month, (uint8_t)(year - 2000));
  _valid = true;
}
void TeenyZeroRTC::setRTCTime(rtc_datetime_t dateTime) {
  rtc.setTime(dateTime.hour, dateTime.minute, dateTime.second);
  rtc.setDate(dateTime.day, dateTime.month, (uint8_t)(dateTime.year - 2000));
  _valid = true;
}
/********************************************************************/
// Set RTC date/time with time zone offset
void TeenyZeroRTC::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                          uint8_t hour, uint8_t minute, uint8_t second,
                          int16_t timeZoneOffset) {
  rtc_datetime_t _dateTime;
  _dateTime.year   = year;
  _dateTime.month  = month;
  _dateTime.day    = day;
  _dateTime.hour   = hour;
  _dateTime.minute = minute;
  _dateTime.second = second;
  setRTCTime(getOffsetDateTime(_dateTime, timeZoneOffset));
}
/********************************************************************/
// Set RTC date/time with time zone offset
void TeenyZeroRTC::setRTCTime(rtc_datetime_t dateTime, int16_t timeZoneOffset) {
  setRTCTime(getOffsetDateTime(dateTime, timeZoneOffset));
}

/********************************************************************/
rtc_datetime_t TeenyZeroRTC::getRTCTime() {
  rtc_datetime_t _dateTime;
  _dateTime.year   = (uint16_t)rtc.getYear() + 2000;
  _dateTime.month  = rtc.getMonth();
  _dateTime.day    = rtc.getDay();
  _dateTime.hour   = rtc.getHours();
  _dateTime.minute = rtc.getMinutes();
  _dateTime.second = rtc.getSeconds();
  return _dateTime;
}
/********************************************************************/
// Get RTC date/time with time zone offset
rtc_datetime_t TeenyZeroRTC::getRTCTime(int16_t timeZoneOffset) {
   return getOffsetDateTime(getRTCTime(), timeZoneOffset);
}
/********************************************************************/
// Get RTC ISO8601 string
char* TeenyZeroRTC::getRTCISO8601DateTimeStr() {
  return getISO8601DateTimeStr(getRTCTime());
}
/********************************************************************/
// Get RTC ISO8601 string with time zone offset
char* TeenyZeroRTC::getRTCISO8601DateTimeStr(int16_t timeZoneOffset) {
  return getISO8601DateTimeStr(getRTCTime(timeZoneOffset));
}
#endif


#ifdef FEATHERDS3231
#include <RTClib.h>
/********************************************************************/
// DS3231 RTC
/********************************************************************/
TeenyDS3231RTC::TeenyDS3231RTC() { }

TeenyDS3231RTC::~TeenyDS3231RTC() { }

/********************************************************************/
bool TeenyDS3231RTC::setup() {
  return rtc.begin();
}

/********************************************************************/
bool TeenyDS3231RTC::isValid() {
  return !rtc.lostPower();
}

/********************************************************************/
void TeenyDS3231RTC::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                              uint8_t hour, uint8_t minute, uint8_t second) {
  rtc.adjust(DateTime(year, month, day, hour, minute, second));
}
/********************************************************************/
void TeenyDS3231RTC::setRTCTime(rtc_datetime_t dateTime) {
  rtc.adjust(DateTime(dateTime.year, dateTime.month, dateTime.day,
                      dateTime.hour, dateTime.minute, dateTime.second));
}
/********************************************************************/
// Set RTC date/time with time zone offset
void TeenyDS3231RTC::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                          uint8_t hour, uint8_t minute, uint8_t second,
                          int16_t timeZoneOffset) {
  rtc_datetime_t _dateTime;
  _dateTime.year   = year;
  _dateTime.month  = month;
  _dateTime.day    = day;
  _dateTime.hour   = hour;
  _dateTime.minute = minute;
  _dateTime.second = second;
  setRTCTime(getOffsetDateTime(_dateTime, timeZoneOffset));
}
/********************************************************************/
// Set RTC date/time with time zone offset
void TeenyDS3231RTC::setRTCTime(rtc_datetime_t dateTime, int16_t timeZoneOffset) {
  setRTCTime(getOffsetDateTime(dateTime, timeZoneOffset));
}

/********************************************************************/
rtc_datetime_t TeenyDS3231RTC::getRTCTime() {
  DateTime _now = rtc.now();
  rtc_datetime_t _dateTime;
  _dateTime.year   = _now.year();
  _dateTime.month  = _now.month();
  _dateTime.day    = _now.day();
  _dateTime.hour   = _now.hour();
  _dateTime.minute = _now.minute();
  _dateTime.second = _now.second();
  return _dateTime;
}
/********************************************************************/
// Get RTC date/time with time zone offset
rtc_datetime_t TeenyDS3231RTC::getRTCTime(int16_t timeZoneOffset) {
   return getOffsetDateTime(getRTCTime(), timeZoneOffset);
}
/********************************************************************/
// Get RTC ISO8601 string
char* TeenyDS3231RTC::getRTCISO8601DateTimeStr() {
  return getISO8601DateTimeStr(getRTCTime());
}
/********************************************************************/
// Get RTC ISO8601 string with time zone offset
char* TeenyDS3231RTC::getRTCISO8601DateTimeStr(int16_t timeZoneOffset) {
  return getISO8601DateTimeStr(getRTCTime(timeZoneOffset));
}
#endif

