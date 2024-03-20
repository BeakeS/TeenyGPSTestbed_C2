
/********************************************************************/
// Real Time Clock

#ifndef RealTimeClock_h
#define RealTimeClock_h

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
class RealTimeClock {

  public:

    // Constructor / destructor / disallow copy and move
    RealTimeClock() {}
    virtual ~RealTimeClock() {}
    RealTimeClock(const RealTimeClock&);
    RealTimeClock& operator=(const RealTimeClock&);

    bool setup() {
      _valid = false;
      return true;
    }

    void setValid(bool valid) {
      _valid = valid;
    }
    bool isValid() {
      return _valid;
    }

    void setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                    uint8_t hour, uint8_t minute, uint8_t second) {
      RTC_TimeTypeDef rtcTime;
      RTC_DateTypeDef rtcDate;
      rtcTime.Hours   = hour;
      rtcTime.Minutes = minute;
      rtcTime.Seconds = second;
      M5.Rtc.SetTime(&rtcTime);
      rtcDate.Year  = year;
      rtcDate.Month = month;
      rtcDate.Date  = day;
      M5.Rtc.SetDate(&rtcDate);
      _valid = true;
    }

    rtc_datetime_t getRTCTime() {
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

    // Convert GMT to local time given offset in minutes
    rtc_datetime_t getLocalTime(rtc_datetime_t greenwichMeanTime, int16_t timeZoneOffset) {
      rtc_datetime_t _localTime = greenwichMeanTime;
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

    // Convert RTC to local time given offset in minutes
    rtc_datetime_t getLocalTime(int16_t timeZoneOffset) {
       return getLocalTime(getRTCTime(), timeZoneOffset);
    }

    rtc_datetime_t extractISO8601DateTime(const char* iso8601DateTimeStr) {
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

    char* getISO8601DateTimeStr(rtc_datetime_t dateTime) {
      //e.g. "2020-06-25T15:29:37"
      static char _itdStr[20];
      sprintf(_itdStr, "%u-%02d-%02dT%02d:%02d:%02d",
              dateTime.year, dateTime.month, dateTime.day,
              dateTime.hour, dateTime.minute, dateTime.second);
      return _itdStr;
    }

  private:

    bool _valid;

};

#endif //RealTimeClock_h

/********************************************************************/
// RTC instance
RealTimeClock rtc;

