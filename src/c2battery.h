
/********************************************************************/
// Core2 Battery

#ifndef Core2BatteryStatus_h
#define Core2BatteryStatus_h

#include <M5Core2.h>

/********************************************************************/
class Core2BatteryStatus {

  public:

    // Constructor / destructor / disallow copy and move
    Core2BatteryStatus() {}
    virtual ~Core2BatteryStatus() {}
    Core2BatteryStatus(const Core2BatteryStatus&);
    Core2BatteryStatus& operator=(const Core2BatteryStatus&);

    void readStatus() {
      uint32_t _nowMS = millis();
      if((_nowMS - lastSampleTime) < samplePeriod) return;
      lastSampleTime = _nowMS;
      voltage = M5.Axp.GetBatVoltage();
      charging = M5.Axp.isCharging();
      float rawPercentage = (voltage < 3.2f) ? 0 : (voltage - 3.2f) * 100;
      if(rawPercentage > 100) {
        percentage = 100;
      } else {
        percentage = static_cast<uint8_t>(rawPercentage);
      }
    }

    float getVoltage() {
      readStatus();
      return voltage;
    }

    uint8_t getPercentage() {
      readStatus();
      return percentage;
    }

    bool isCharging() {
      readStatus();
      return charging;
    }

  private:
    uint32_t samplePeriod = 1000;   // sample once a second
    uint32_t lastSampleTime = millis() - 1000;
    float   voltage;
    uint8_t percentage;
    bool    charging;

};

#endif //Core2BatteryStatus_h

/********************************************************************/
// Battery instance
Core2BatteryStatus battery;

