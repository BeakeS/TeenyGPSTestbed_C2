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

#ifndef TEENYRTC_H
#define TEENYRTC_H

#include <Arduino.h>

/********************************************************************/
// RTC Date/Time Struct
typedef struct {
  uint16_t year;
  uint8_t  month;
  uint8_t  day;
  uint8_t  hour;
  uint8_t  minute;
  uint8_t  second;
  uint8_t  pad0;
} rtc_datetime_t;

/********************************************************************/
class TeenyRTC {

  public:

    // Constructor
    TeenyRTC();

    virtual bool setup() = 0;

    void setValid(bool valid=true);
    bool isValid();

    // Set RTC dateTime
    virtual void setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                            uint8_t hour, uint8_t minute, uint8_t second) = 0;
    virtual void setRTCTime(rtc_datetime_t dateTime) = 0;
    virtual void setRTCTime(uint32_t unixTime) = 0;
    // Set RTC dateTime with time zone offset
    virtual void setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                            uint8_t hour, uint8_t minute, uint8_t second,
                            int16_t timeZoneOffset) = 0;
    virtual void setRTCTime(rtc_datetime_t dateTime, int16_t timeZoneOffset) = 0;
    virtual void setRTCTime(uint32_t unixTime, int16_t timeZoneOffset) = 0;

    // Get RTC dateTime
    virtual rtc_datetime_t getRTCTime() = 0;
    // Get RTC dateTime with time zone offset
    virtual rtc_datetime_t getRTCTime(int16_t timeZoneOffset) = 0;

    // Get RTC unixTime
    virtual uint32_t getRTCUnixTime() = 0;
    // Get RTC unixTime with time zone offset
    virtual uint32_t getRTCUnixTime(int16_t timeZoneOffset) = 0;

    // Get RTC ISO8601 string
    virtual char* getRTCISO8601DateTimeStr() = 0;
    // Get RTC ISO8601 string with time zone offset
    virtual char* getRTCISO8601DateTimeStr(int16_t timeZoneOffset) = 0;

    // Convert unixTime to dateTime
    rtc_datetime_t unixTimeToDateTime(uint32_t unixTime);
    // Convert unixTime to dateTime with time zone offset
    rtc_datetime_t unixTimeToDateTime(uint32_t unixTime,
                                      int16_t timeZoneOffset);

    // Convert dateTime to unixTime
    uint32_t dateTimeToUnixTime(rtc_datetime_t dateTime);
    // Convert dateTime to unixTime with time zone offset
    uint32_t dateTimeToUnixTime(rtc_datetime_t dateTime,
                                int16_t timeZoneOffset);

    // Get ISO8601 string from dateTime
    char* getISO8601DateTimeStr(rtc_datetime_t dateTime);
    // Get ISO8601 string from dateTime with time zone offset
    char* getISO8601DateTimeStr(rtc_datetime_t dateTime,
                                int16_t timeZoneOffset);

    // Get ISO8601 string from unixTime
    char* getISO8601DateTimeStr(uint32_t unixTime);
    // Get ISO8601 string from unixTime with time zone offset
    char* getISO8601DateTimeStr(uint32_t unixTime,
                                int16_t timeZoneOffset);

    // Extract dateTime from ISO8601 string
    rtc_datetime_t extractISO8601DateTime(const char* iso8601DateTimeStr);
    // Extract dateTime from ISO8601 string with time zone offset
    rtc_datetime_t extractISO8601DateTime(const char* iso8601DateTimeStr,
                                          int16_t timeZoneOffset);

    // Convert dateTime based on time zone offset in minutes
    rtc_datetime_t getOffsetDateTime(rtc_datetime_t dateTime, int16_t timeZoneOffset);

  protected:

    bool _valid;

  private:

};


#ifdef M5_CORE2
/********************************************************************/
class TeenyCore2RTC : public TeenyRTC {

  public:

    // Constructor / destructor / disallow copy and move
    TeenyCore2RTC();
    virtual ~TeenyCore2RTC();
    TeenyCore2RTC(const TeenyCore2RTC&);
    TeenyCore2RTC& operator=(const TeenyCore2RTC&);

    bool setup();

    void setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                    uint8_t hour, uint8_t minute, uint8_t second);
    void setRTCTime(rtc_datetime_t dateTime);
    void setRTCTime(uint32_t unixTime);
    void setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                    uint8_t hour, uint8_t minute, uint8_t second,
                    int16_t timeZoneOffset);
    void setRTCTime(rtc_datetime_t dateTime, int16_t timeZoneOffset);
    void setRTCTime(uint32_t unixTime, int16_t timeZoneOffset);

    rtc_datetime_t getRTCTime();
    rtc_datetime_t getRTCTime(int16_t timeZoneOffset);

    uint32_t getRTCUnixTime();
    uint32_t getRTCUnixTime(int16_t timeZoneOffset);

    char* getRTCISO8601DateTimeStr();
    char* getRTCISO8601DateTimeStr(int16_t timeZoneOffset);

  private:

};
#endif


#ifdef TEENSY41
/********************************************************************/
class TeenyTeensy41RTC : public TeenyRTC {

  public:

    // Constructor / destructor / disallow copy and move
    TeenyTeensy41RTC();
    virtual ~TeenyTeensy41RTC();
    TeenyTeensy41RTC(const TeenyTeensy41RTC&);
    TeenyTeensy41RTC& operator=(const TeenyTeensy41RTC&);

    bool setup();

    void setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                    uint8_t hour, uint8_t minute, uint8_t second);
    void setRTCTime(rtc_datetime_t dateTime);
    void setRTCTime(uint32_t unixTime);
    void setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                    uint8_t hour, uint8_t minute, uint8_t second,
                    int16_t timeZoneOffset);
    void setRTCTime(rtc_datetime_t dateTime, int16_t timeZoneOffset);
    void setRTCTime(uint32_t unixTime, int16_t timeZoneOffset);

    rtc_datetime_t getRTCTime();
    rtc_datetime_t getRTCTime(int16_t timeZoneOffset);

    uint32_t getRTCUnixTime();
    uint32_t getRTCUnixTime(int16_t timeZoneOffset);

    char* getRTCISO8601DateTimeStr();
    char* getRTCISO8601DateTimeStr(int16_t timeZoneOffset);

  private:

};
#endif


#ifdef FEATHERM0
#include <RTCZero.h>
/********************************************************************/
class TeenyZeroRTC : public TeenyRTC {

  public:

    // Constructor / destructor / disallow copy and move
    TeenyZeroRTC();
    virtual ~TeenyZeroRTC();
    TeenyZeroRTC(const TeenyZeroRTC&);
    TeenyZeroRTC& operator=(const TeenyZeroRTC&);

    bool setup();

    void setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                    uint8_t hour, uint8_t minute, uint8_t second);
    void setRTCTime(rtc_datetime_t dateTime);
    void setRTCTime(uint32_t unixTime);
    void setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                    uint8_t hour, uint8_t minute, uint8_t second,
                    int16_t timeZoneOffset);
    void setRTCTime(rtc_datetime_t dateTime, int16_t timeZoneOffset);
    void setRTCTime(uint32_t unixTime, int16_t timeZoneOffset);

    rtc_datetime_t getRTCTime();
    rtc_datetime_t getRTCTime(int16_t timeZoneOffset);

    uint32_t getRTCUnixTime();
    uint32_t getRTCUnixTime(int16_t timeZoneOffset);

    char* getRTCISO8601DateTimeStr();
    char* getRTCISO8601DateTimeStr(int16_t timeZoneOffset);

  private:

    RTCZero rtc;

};
#endif


#ifdef FEATHERDS3231
#include <RTClib.h>
/********************************************************************/
class TeenyDS3231RTC : public TeenyRTC {

  public:

    // Constructor / destructor / disallow copy and move
    TeenyDS3231RTC();
    virtual ~TeenyDS3231RTC();
    TeenyDS3231RTC(const TeenyDS3231RTC&);
    TeenyDS3231RTC& operator=(const TeenyDS3231RTC&);

    bool setup();

    bool isValid();

    void setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                    uint8_t hour, uint8_t minute, uint8_t second);
    void setRTCTime(rtc_datetime_t dateTime);
    void setRTCTime(uint32_t unixTime);
    void setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                    uint8_t hour, uint8_t minute, uint8_t second,
                    int16_t timeZoneOffset);
    void setRTCTime(rtc_datetime_t dateTime, int16_t timeZoneOffset);
    void setRTCTime(uint32_t unixTime, int16_t timeZoneOffset);

    rtc_datetime_t getRTCTime();
    rtc_datetime_t getRTCTime(int16_t timeZoneOffset);

    uint32_t getRTCUnixTime();
    uint32_t getRTCUnixTime(int16_t timeZoneOffset);

    char* getRTCISO8601DateTimeStr();
    char* getRTCISO8601DateTimeStr(int16_t timeZoneOffset);

  private:

    RTC_DS3231 rtc;

};
#endif


#endif //TEENYRTC_H

