/*
TeenyGPSConnect.h - Class file for the TeenyGPSConnect Arduino Library.
Copyright (C) *** Need copyright statement here ***

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include "TeenyGPSConnect.h"

/** @brief GNSS handler using UBX protocol library 
 * 
 * When activated, set auto receive for the UBX protocol commands used:
 * UBX-NAV-PVT: Position, velocity, time, PDOP, height, h/v accuracies, number of satellites. Navigation Position Velocity Time Solution.
 * 
*/

/********************************************************************/
TeenyGPSConnect::TeenyGPSConnect()
{
}

TeenyGPSConnect::~TeenyGPSConnect() {
}

/********************************************************************/
// Direct serialPort commands
/********************************************************************/
//void TeenyGPSConnect::begin(uint32_t baudRate_) {
//  serialPort->begin(baudRate_);
//}
//
//void TeenyGPSConnect::end() {
//  serialPort->end();
//}

/********************************************************************/
// GPS INIT PROCS

/********************************************************************/
bool TeenyGPSConnect::gnss_init(HardwareSerial &serialPort_, uint32_t baudRate_, uint8_t autoNAVPVTRate, uint8_t autoNAVSATRate) {

  // Assign serial port
  serialPort = &serialPort_;

  // Set gnss/gps baudRate
  if(!gnss_setSerialRate(baudRate_)) return false;

  // Get protocol version
  gnss_getProtocolVersion();

  // Setup GSP Module with auto NAV-PVT non-blocking access
  // getNAVPVT() will return true if a new navigation solution is available
  gnss.setPortOutput(COM_PORT_UART1, COM_TYPE_UBX); //Set the UART port to output UBX only
  gnss.setMeasurementRate(1000);       //Produce a measurement every 1000ms
  gnss.setNavigationRate(1);           //Produce a navigation solution every measurement
  gnss.setAutoNAVPVTrate(autoNAVPVTRate); //Include NAV-PVT reports 
  gnss.setAutoNAVSATrate(autoNAVSATRate); //Include NAV-SAT reports 

  // Mark the fix items invalid to start
  data.packet_valid = false;
  data.location_fixType = 0;
  data.location_valid = false;
  data.date_valid = false;
  data.time_valid = false;

  return true;
}

/********************************************************************/
bool TeenyGPSConnect::gnss_setSerialRate(uint32_t baudRate_) {
  serialPort->begin(baudRate_);
  if(!gnss.begin(*serialPort)) {
    delay(100);
    serialPort->begin(9600); // default for many gps modules
    if(gnss.begin(*serialPort)) {
      gnss.setSerialRate(baudRate_);
      delay(100);
      serialPort->begin(baudRate_);
      if(gnss.begin(*serialPort)) {
        gnss.saveConfiguration();
      } else {
        return false;
      }
    } else {
      return false;
    }
  }
  return true;
}

/********************************************************************/
void TeenyGPSConnect::gnss_getProtocolVersion() {
  data.protocolVersionHigh = gnss.getProtocolVersionHigh();
  data.protocolVersionLow = gnss.getProtocolVersionLow();
}

/********************************************************************/
// GNSS ACCESS PROCS
/********************************************************************/
void TeenyGPSConnect::gnss_checkUblox() {
  gnss.checkUblox();
}

/********************************************************************/
bool TeenyGPSConnect::getNAVPVT() {
  // getNAVPVT will return true if there actually is a fresh
  // navigation solution available. "LLH" is longitude, latitude, height.
  // getNAVPVT() returns UTC date and time.
  // Do not use GNSS time, see u-blox spec section 9.
  if(gnss.getNAVPVT()) {
    time_getnavpvt.restart();
    data.packet_valid = true;

    if(gnss.getGnssFixOk()) {
      data.location_fixType = gnss.getFixType();
      data.location_valid = true;
      location_timer.restart();
      data.latitude = (float)(gnss.getLatitude() * 1e-7);
      data.longitude = (float)(gnss.getLongitude() * 1e-7);
      data.altitudeMSL = gnss.getAltitudeMSL() / 1000;
      data.heading = (float)(gnss.getHeading() * 1e-5);
      data.numSV = gnss.getSIV();
      data.hacc = gnss.getHorizontalAccEst() / 1000;
      data.vacc = gnss.getVerticalAccEst() / 1000;
      data.pdop = ((float)gnss.getPDOP()) * 0.01;
    }
    else {
      // No valid fix
      if(location_timer.isExpired()) {
        data.location_fixType = 0;
        data.location_valid = false;
      }
    }
 
    if(gnss.getDateValid()) {
      data.date_valid = true;
      date_timer.restart();
      data.year = gnss.getYear();
      data.month = gnss.getMonth();
      data.day = gnss.getDay();
    } else if(date_timer.isExpired()) {
      data.date_valid = false;
    }

    if(gnss.getTimeValid()) {
      data.time_valid = true;
      time_timer.restart();
      data.hour = gnss.getHour();
      data.minute = gnss.getMinute();
      data.second = gnss.getSecond();
    } else if(time_timer.isExpired()) {
      data.time_valid = false;
    }
    return true;
  }
  // else lost packet(s)
  if(time_getnavpvt.isExpired()) {
    data.packet_valid = false;
    data.location_fixType = 0;
    data.location_valid = false;
    data.date_valid = false;
    data.time_valid = false;
  }
  return false;
}

/********************************************************************/
bool TeenyGPSConnect::pollNAVPVT() {
  if(gnss.pollNAVPVT()) {
    data.location_fixType = gnss.getFixType();
    data.location_valid = gnss.getGnssFixOk();
    data.longitude = (float)(gnss.getLongitude() * 1e-7);
    data.latitude = (float)(gnss.getLatitude() * 1e-7);
    data.altitudeMSL = gnss.getAltitudeMSL() / 1000;
    data.heading = (float)(gnss.getHeading() * 1e-5);
    data.numSV = gnss.getSIV();
    data.hacc = gnss.getHorizontalAccEst() / 1000;
    data.vacc = gnss.getVerticalAccEst() / 1000;
    data.pdop = ((float)gnss.getPDOP()) * 0.01;
    data.date_valid = gnss.getDateValid();
    data.year = gnss.getYear();
    data.month = gnss.getMonth();
    data.day = gnss.getDay();
    data.time_valid = gnss.getTimeValid();
    data.hour = gnss.getHour();
    data.minute = gnss.getMinute();
    data.second = gnss.getSecond();
    return true;
  }
  return false;
}

/********************************************************************/
void TeenyGPSConnect::getNAVPVTPacket(uint8_t *packet) {
  gnss.getNAVPVTPacket(packet);
}

/********************************************************************/
bool TeenyGPSConnect::isPacketValid() {
  return data.packet_valid;
}
uint8_t TeenyGPSConnect::getLocationFixType() {
  return data.location_fixType;
}
bool TeenyGPSConnect::isLocationValid() {
  return data.location_valid;
}
float TeenyGPSConnect::getLatitude() {
  return data.latitude;
}
float TeenyGPSConnect::getLongitude() {
  return data.longitude;
}
int32_t TeenyGPSConnect::getAltitudeMSL() {
  return data.altitudeMSL;
}
float TeenyGPSConnect::getHeading() {
  return data.heading;
}

/********************************************************************/
uint8_t TeenyGPSConnect::getNumSV() {
  return data.numSV;
}
uint32_t TeenyGPSConnect::getHAccEst() {
  return data.hacc;
}
uint32_t TeenyGPSConnect::getVAccEst() {
  return data.vacc;
}
float TeenyGPSConnect::getPDOP() {
  return data.pdop;
}

/********************************************************************/
bool TeenyGPSConnect::isDateValid() {
  return data.date_valid;
}
uint16_t TeenyGPSConnect::getYear() {
  return data.year;
}
uint8_t TeenyGPSConnect::getMonth() {
  return data.month;
}
uint8_t TeenyGPSConnect::getDay() {
  return data.day;
}
bool TeenyGPSConnect::isTimeValid() {
  return data.time_valid;
}
uint8_t TeenyGPSConnect::getHour() {
  return data.hour;
}
uint8_t TeenyGPSConnect::getMinute() {
  return data.minute;
}
uint8_t TeenyGPSConnect::getSecond() {
  return data.second;
}

/********************************************************************/
bool TeenyGPSConnect::getNAVSAT() {
  // getNAVSAT will return true if there actually is fresh
  // navigation satellite data.
  if(gnss.getNAVSAT()) {
    time_getnavsat.restart();

    gnss.getNAVSATPacket(navsatPacket);
    gnss.getNAVSATInfo(navsatInfo);
    return true;
  }
  // else lost packet(s)
  if(time_getnavsat.isExpired()) {
    navsatPacket.validPacket = false;
    navsatInfo.numSvs = 0;
    navsatInfo.numSvsHealthy = 0;
    navsatInfo.numSvsUsed = 0;
  }
  return false;
}

/********************************************************************/
bool TeenyGPSConnect::pollNAVSAT() {
  if(gnss.pollNAVSAT()) {
    return true;
  }
  return false;
}

/********************************************************************/
void TeenyGPSConnect::getNAVSATPacket(ubloxPacket_t &packet_) {
  packet_ = navsatPacket;
}

/********************************************************************/
void TeenyGPSConnect::getNAVSATInfo(ubloxNAVSATInfo_t &info_) {
  info_ = navsatInfo;
}

/********************************************************************/
byte TeenyGPSConnect::getProtocolVersionHigh() {
  return data.protocolVersionHigh;
}
byte TeenyGPSConnect::getProtocolVersionLow() {
  return data.protocolVersionLow;
}

/********************************************************************/
// Single Step Commands (for capturing host transmit packets on emulator side)
/********************************************************************/
bool TeenyGPSConnect::gnss_ss_begin(Stream &serialPort_) {
  return gnss.begin(serialPort_);
}

/********************************************************************/
void TeenyGPSConnect::gnss_ss_setSerialRate(uint32_t baudRate) {
  gnss.setSerialRate(baudRate);
}

/********************************************************************/
bool TeenyGPSConnect::gnss_ss_saveConfiguration() {
  return gnss.saveConfiguration();
}

/********************************************************************/
bool TeenyGPSConnect::gnss_ss_getProtocolVersion() {
  return gnss.getProtocolVersion();
}

/********************************************************************/
bool TeenyGPSConnect::gnss_ss_setPortOutput(uint8_t portID, uint8_t comSettings) {
  return gnss.setPortOutput(portID, comSettings);
}

/********************************************************************/
bool TeenyGPSConnect::gnss_ss_setMeasurementRate(uint16_t rate) {
  return gnss.setMeasurementRate(rate);
}

/********************************************************************/
bool TeenyGPSConnect::gnss_ss_setNavigationRate(uint16_t rate) {
  return gnss.setNavigationRate(rate);
}

/********************************************************************/
bool TeenyGPSConnect::gnss_ss_setAutoNAVPVT(bool enabled) {
  return gnss.setAutoNAVPVT(enabled);
}

