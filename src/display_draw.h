
/********************************************************************/
char* getRTCClockISO8601DateTimeStr() {
  static char _itdStr[20];
  if(rtc.isValid()) {
    return rtc.getRTCISO8601DateTimeStr(deviceState.TIMEZONE);
  }
  sprintf(_itdStr, "**  RTC NOT SET  **");
  return _itdStr;
}

/********************************************************************/
char* getGPSISO8601DateTimeStr() {
  static char _itdStr[20];
  if(gps.isDateValid() && gps.isTimeValid()) {
    rtc_datetime_t dateTime;
    dateTime.year   = gps.getYear();
    dateTime.month  = gps.getMonth();
    dateTime.day    = gps.getDay();
    dateTime.hour   = gps.getHour();
    dateTime.minute = gps.getMinute();
    dateTime.second = gps.getSecond();
    return rtc.getISO8601DateTimeStr(dateTime, deviceState.TIMEZONE);
  }
  sprintf(_itdStr, "* NO DATETIME FIX *");
  return _itdStr;
}

/********************************************************************/
char* getPVTPacketISO8601DateTimeStr(ubxNAVPVTInfo_t _ubxNAVPVTInfo) {
  static char _itdStr[20];
  if(_ubxNAVPVTInfo.dateValid && _ubxNAVPVTInfo.timeValid) {
    rtc_datetime_t dateTime;
    dateTime.year   = _ubxNAVPVTInfo.year;
    dateTime.month  = _ubxNAVPVTInfo.month;
    dateTime.day    = _ubxNAVPVTInfo.day;
    dateTime.hour   = _ubxNAVPVTInfo.hour;
    dateTime.minute = _ubxNAVPVTInfo.minute;
    dateTime.second = _ubxNAVPVTInfo.second;
    return rtc.getISO8601DateTimeStr(dateTime, deviceState.TIMEZONE);
  }
  sprintf(_itdStr, "* NO DATETIME FIX *");
  return _itdStr;
}

/********************************************************************/
char* getLatitudeStr(const float latitude) {
  // latitude range: -90 to 90
  //latitude = 41.311589; // Bear Mountain
  static char _fStr[10];
  static char _lStr[11];
  if(latitude < 0.0) {
    dtostrf(-latitude, -9, 6, _fStr);
    sprintf(_lStr, "S%s", _fStr);
//} else if(latitude == 0.0) {
//  dtostrf(latitude, -9, 6, _fStr);
//  sprintf(_lStr, "%s", _fStr);
  } else {
    dtostrf(latitude, -9, 6, _fStr);
    sprintf(_lStr, "N%s", _fStr);
  }
  return _lStr;
}

/********************************************************************/
char* getLongitudeStr(const float longitude) {
  // longitude range: -180 to 180
  //longitude = -74.008019; // Bear Mountain
  static char _fStr[11];
  static char _lStr[12];
  if(longitude < 0.0) {
    dtostrf(-longitude, -10, 6, _fStr);
    sprintf(_lStr, "W%s", _fStr);
//} else if(longitude == 0.0) {
//  dtostrf(longitude, -10, 6, _fStr);
//  sprintf(_lStr, "%s", _fStr);
  } else {
    dtostrf(longitude, -10, 6, _fStr);
    sprintf(_lStr, "E%s", _fStr);
  }
  return _lStr;
}

/********************************************************************/
char* getHeadingStr(const float degrees) {
  float deg = degrees + 11.25;
  if(deg >= 360.0) deg = deg - 360;
  deg = max(0, deg);
  uint8_t headIdx = (uint8_t)(deg / 22.5);
  switch(headIdx) {
    case  0: return (char*)"N";
    case  1: return (char*)"NNE";
    case  2: return (char*)"NE";
    case  3: return (char*)"ENE";
    case  4: return (char*)"E";
    case  5: return (char*)"ESE";
    case  6: return (char*)"SE";
    case  7: return (char*)"SSE";
    case  8: return (char*)"S";
    case  9: return (char*)"SSW";
    case 10: return (char*)"SW";
    case 11: return (char*)"WSW";
    case 12: return (char*)"W";
    case 13: return (char*)"WNW";
    case 14: return (char*)"NW";
    case 15: return (char*)"NNW";
  }
  return (char*)"";
}

/********************************************************************/
void display_refresh() {
  uint32_t _nowMS = millis();
  static uint32_t _display_refresh_timer = _nowMS;
  if((_nowMS-_display_refresh_timer) < DISPLAY_REFRESH_PERIOD) {
    return;
  }
  _display_refresh_timer = _nowMS;
  char _dispStr[22];
  if(msg_update(nullptr)) {
    displayRefresh = true;
  }
  // Update display
  if(displayRefresh) {
    // clear display buffer
    display_clearDisplay();
    // draw the menu first
    if(!menuDisplaySleepMode) {
      menu.drawMenu();
      if(menu.isMenuPageCurrent(menuPageMain)) {
        // main/idle display
        sprintf(_dispStr, "Bat %d%% %s",
                battery.getPercentage(),
                battery.isCharging() ? "Charging" : "");
        displayPV.prt_str(_dispStr, 20, 0, 270);
      } else if(menu.isMenuPageCurrent(menuPageGPSRcvr)) {
        // RTC Clock
        displayPV.prt_str(getRTCClockISO8601DateTimeStr(), 19, 6, 24);
        if(gps.isPacketValid()) {
          sprintf(_dispStr, " UBLOX NAVPVT INFO");
          displayPV.prt_str(_dispStr, 20, 0, 48);
          // GPS Clock
          displayPV.prt_str(getGPSISO8601DateTimeStr(), 19, 6, 66);
          // GPS Location
          if(gps.isLocationValid()) {
            sprintf(_dispStr, "Lat:     %s", getLatitudeStr(gps.getLatitude()));
            displayPV.prt_str(_dispStr, 20, 0, 84);
            sprintf(_dispStr, "Lon:     %s", getLongitudeStr(gps.getLongitude()));
            displayPV.prt_str(_dispStr, 20, 0, 102);
            sprintf(_dispStr, "Alt:     %03d", max(min(gps.getAltitudeMSL(), 999), -99));
            displayPV.prt_str(_dispStr, 20, 0, 120);
            sprintf(_dispStr, "Heading: %s", getHeadingStr(gps.getHeading()));
            displayPV.prt_str(_dispStr, 20, 0, 138);
            sprintf(_dispStr, "HorAcc:  %03d", min(gps.getHAccEst(), 999));
            displayPV.prt_str(_dispStr, 20, 0, 156);
            sprintf(_dispStr, "VerAcc:  %03d", min(gps.getVAccEst(), 999));
            displayPV.prt_str(_dispStr, 20, 0, 174);
            sprintf(_dispStr, "Fix:     %dD", min(gps.getLocationFixType(), 9));
            displayPV.prt_str(_dispStr, 20, 0, 192);
            sprintf(_dispStr, "Sats:    %02d", min(gps.getNumSV(), 99));
            displayPV.prt_str(_dispStr, 20, 0, 210);
            displayPV.prt_str("PosDOP:  ", 9, 0, 228);
            displayPV.prt_float(min(gps.getPDOP(), 9.9), 3, 1, 108, 228);
          } else {
            sprintf(_dispStr, "**  NO GNSS FIX  **");
            displayPV.prt_str(_dispStr, 19, 6, 90);
          }
        } else {
          sprintf(_dispStr, "** NO NAVPVT DATA **");
          displayPV.prt_str(_dispStr, 20, 0, 66);
        }
      //sprintf(_dispStr, "P%XL%XD%XT%X",
      //        gps.isPacketValid(), gps.isLocationValid(),
      //        gps.isDateValid(), gps.isTimeValid());
      //displayPV.prt_str(_dispStr, 20, 0, 284);
      } else if(menu.isMenuPageCurrent(menuPageGPSLogr)) {
        // RTC Clock
        displayPV.prt_str(getRTCClockISO8601DateTimeStr(), 19, 6, 24);
        if(gps.isPacketValid()) {
          sprintf(_dispStr, " UBLOX NAVPVT INFO");
          displayPV.prt_str(_dispStr, 20, 0, 48);
          // GPS Clock
          displayPV.prt_str(getGPSISO8601DateTimeStr(), 19, 6, 66);
          // GPS Location
          if(gps.isLocationValid()) {
            sprintf(_dispStr, "Lat=%s", getLatitudeStr(gps.getLatitude()));
            displayPV.prt_str(_dispStr, 20, 0, 84);
            sprintf(_dispStr, "Lon=%s", getLongitudeStr(gps.getLongitude()));
            displayPV.prt_str(_dispStr, 20, 0, 102);
            sprintf(_dispStr, "ALT=%03d", max(min(gps.getAltitudeMSL(), 999), -99));
            displayPV.prt_str(_dispStr, 8, 0, 120);
            sprintf(_dispStr, "HA=%03d", min(gps.getHAccEst(), 999));
            displayPV.prt_str(_dispStr, 8, 90, 120);
            sprintf(_dispStr, "VA=%03d", min(gps.getVAccEst(), 999));
            displayPV.prt_str(_dispStr, 8, 168, 120);
            sprintf(_dispStr, "H=%s", getHeadingStr(gps.getHeading()));
            displayPV.prt_str(_dispStr, 5, 0, 138);
            sprintf(_dispStr, "F=%d", min(gps.getLocationFixType(), 9));
            displayPV.prt_str(_dispStr, 3, 72, 138);
            sprintf(_dispStr, "S=%02d", min(gps.getNumSV(), 99));
            displayPV.prt_str(_dispStr, 4, 114, 138);
            displayPV.prt_str("PP=", 3, 168, 138);
            displayPV.prt_float(min(gps.getPDOP(), 9.9), 3, 1, 204, 138);
          } else {
            sprintf(_dispStr, "**  NO GNSS FIX  **");
            displayPV.prt_str(_dispStr, 19, 6, 90);
          }
        } else {
          sprintf(_dispStr, "** NO NAVPVT DATA **");
          displayPV.prt_str(_dispStr, 20, 0, 66);
        }
        if(ubxLoggingInProgress) {
          sprintf(_dispStr, "  Total Pkts=%04d",
                  min(ubxLoggingFileWriteCount, 9999));
          displayPV.prt_str(_dispStr, 20, 0, 204);
          sprintf(_dispStr, "  Valid Pkts=%04d",
                  min(ubxLoggingFileWriteValidCount, 9999));
          displayPV.prt_str(_dispStr, 20, 0, 222);
        }
      //sprintf(_dispStr, "P%XL%XD%XT%X",
      //        gps.isPacketValid(), gps.isLocationValid(),
      //        gps.isDateValid(), gps.isTimeValid());
      //displayPV.prt_str(_dispStr, 20, 0, 284);
      } else if(menu.isMenuPageCurrent(menuPageGPSNsat)) {
        // GPS Clock
        if(gps.isPacketValid()) {
          displayPV.prt_str(getGPSISO8601DateTimeStr(), 19, 6, 24);
        } else {
          sprintf(_dispStr, "** NO NAVPVT DATA **");
          displayPV.prt_str(_dispStr, 20, 0, 24);
        }
        if(menu_GPSNsatDisplayMap) {
          // NAVSAT Map
          drawSatConstellation(mapCompAngle);
        } else {
          // NAVSAT Data
          ubloxPacket_t navsatPacket;
          ubloxNAVSATInfo_t navsatInfo;
          gps.getNAVSATPacket(navsatPacket);
          gps.getNAVSATInfo(navsatInfo);
          if(navsatPacket.validPacket) {
            sprintf(_dispStr, "   SATELLITE INFO");
            displayPV.prt_str(_dispStr, 20, 0, 48);
            sprintf(_dispStr, "Total=%02d", navsatInfo.numSvs);
            displayPV.prt_str(_dispStr, 20, 0, 68);
            sprintf(_dispStr, "HealthySignal=%02d", navsatInfo.numSvsHealthy);
            displayPV.prt_str(_dispStr, 20, 0, 86);
            sprintf(_dispStr, "EphemerisValid=%02d", navsatInfo.numSvsEphValid);
            displayPV.prt_str(_dispStr, 20, 0, 104);
            sprintf(_dispStr, "UsedForNav=%02d", navsatInfo.numSvsUsed);
            displayPV.prt_str(_dispStr, 20, 0, 122);
            displayPV.prt_str("Satellites(id/snr):", 20, 0, 146);
            for(uint8_t i=0; i<min(navsatInfo.numSvsHealthy, 24); i++) {
              sprintf(_dispStr, "%c%02d/%02d",
                      navsatInfo.svSortList[i].gnssIdType,
                      navsatInfo.svSortList[i].svId,
                      navsatInfo.svSortList[i].cno);
              displayPV.prt_str(_dispStr, 6, (i%3)*84, ((i/3)*16)+166);
            }
          //for(uint8_t i=0; i<min(navsatInfo.numSvsHealthy, 8); i++) {
          //  sprintf(_dispStr, "%c%02d/%02d %d%d%d %02d/%03d",
          //          navsatInfo.svSortList[i].gnssIdType,
          //          navsatInfo.svSortList[i].svId,
          //          navsatInfo.svSortList[i].cno,
          //          navsatInfo.svSortList[i].healthy,
          //          navsatInfo.svSortList[i].ephValid,
          //          navsatInfo.svSortList[i].svUsed,
          //          navsatInfo.svSortList[i].elev,
          //          navsatInfo.svSortList[i].azim);
          //  displayPV.prt_str(_dispStr, 20, 0, (i*16)+166);
          //}
          } else {
            sprintf(_dispStr, "** NO NAVSAT DATA **");
            displayPV.prt_str(_dispStr, 20, 0, 64);
          }
        }
      //sprintf(_dispStr, "P%XL%XD%XT%X P%XS%dH%dU%d",
      //        gps.isPacketValid(), gps.isLocationValid(),
      //        gps.isDateValid(), gps.isTimeValid(),
      //        navsatPacket.validPacket, navsatInfo.numSvs,
      //        navsatInfo.numSvsHealthy, navsatInfo.numSvsUsed);
      //displayPV.prt_str(_dispStr, 20, 0, 284);
      } else if(menu.isMenuPageCurrent(menuPageGPSSmap)) {
        if(gps.isPacketValid()) {
          // GPS Clock
          displayPV.prt_str(getGPSISO8601DateTimeStr(), 19, 6, 20);
        } else {
          sprintf(_dispStr, "** NO NAVPVT DATA **");
          displayPV.prt_str(_dispStr, 20, 0, 20);
        }
        drawSatConstellation(0);
      } else if(menu.isMenuPageCurrent(menuPageGPSScfg)) {
      } else if(menu.isMenuPageCurrent(menuPageGNSSSelInfo)) {
        ubloxMONGNSSInfo_t gnssSelectInfo = gps.getGNSSSelectionInfo();
        sprintf(_dispStr, "Supported:%s%s%s%s",
                (gnssSelectInfo.supportedGNSS & 0x01) ? " G" : "",
                (gnssSelectInfo.supportedGNSS & 0x02) ? " R" : "",
                (gnssSelectInfo.supportedGNSS & 0x04) ? " B" : "",
                (gnssSelectInfo.supportedGNSS & 0x08) ? " E" : "");
        displayPV.prt_str(_dispStr, 20, 0, 24);
        sprintf(_dispStr, "Default:%s%s%s%s",
                (gnssSelectInfo.defaultGNSS & 0x01) ? " G" : "",
                (gnssSelectInfo.defaultGNSS & 0x02) ? " R" : "",
                (gnssSelectInfo.defaultGNSS & 0x04) ? " B" : "",
                (gnssSelectInfo.defaultGNSS & 0x08) ? " E" : "");
        displayPV.prt_str(_dispStr, 20, 0, 44);
        sprintf(_dispStr, "Enabled:%s%s%s%s",
                (gnssSelectInfo.enabledGNSS & 0x01) ? " G" : "",
                (gnssSelectInfo.enabledGNSS & 0x02) ? " R" : "",
                (gnssSelectInfo.enabledGNSS & 0x04) ? " B" : "",
                (gnssSelectInfo.enabledGNSS & 0x08) ? " E" : "");
        displayPV.prt_str(_dispStr, 20, 0, 64);
        sprintf(_dispStr, "Simultaneous: %d", gnssSelectInfo.simultaneousGNSS);
        displayPV.prt_str(_dispStr, 20, 0, 84);
      } else if(menu.isMenuPageCurrent(menuPageGNSSCfgInfo)) {
        ubloxCFGGNSSInfo_t gnssConfigInfo = gps.getGNSSConfigInfo();
        sprintf(_dispStr, "numTrkChHw=%02d", gnssConfigInfo.numTrkChHw);
        displayPV.prt_str(_dispStr, 20, 0, 24);
        sprintf(_dispStr, "numTrkChUse=%02d", gnssConfigInfo.numTrkChUse);
        displayPV.prt_str(_dispStr, 20, 0, 44);
        sprintf(_dispStr, "numConfigBlocks=%02d", gnssConfigInfo.numConfigBlocks);
        displayPV.prt_str(_dispStr, 20, 0, 64);
        for(uint8_t i=0; i<min(gnssConfigInfo.numConfigBlocks, 8); i++) {
          sprintf(_dispStr, "S=%c E=%c C=%02d/%02d M=%02X",
                  gnssConfigInfo.configBlockList[i].gnssIdType,
                  gnssConfigInfo.configBlockList[i].enable ? 'T' : 'F',
                  gnssConfigInfo.configBlockList[i].resTrkCh,
                  gnssConfigInfo.configBlockList[i].maxTrkCh,
                  gnssConfigInfo.configBlockList[i].sigCfgMask);
          displayPV.prt_str(_dispStr, 20, 0, (i*18)+84);
        }
      } else if(menu.isMenuPageCurrent(menuPageGPSCapt)) {
        if(menu_captRxPktInProgress) {
          sprintf(_dispStr, " File = %s", rxPktFileName);
          displayPV.prt_str(_dispStr, 20, 0, 64);
          sprintf(_dispStr, " Bytes = %d", rxPktWriteCount);
          displayPV.prt_str(_dispStr, 20, 0, 80);
        }
      } else if(menu.isMenuPageCurrent(menuPageGPSStep)) {
      } else if(menu.isMenuPageCurrent(menuPageGPSEmul)) {
        sprintf(_dispStr, "   EMULATOR STATE");
        displayPV.prt_str(_dispStr, 20, 0, 24);
        sprintf(_dispStr, "BaudRate=%d", emulator.getBaudRate());
        displayPV.prt_str(_dispStr, 20, 0, 40);
        sprintf(_dispStr, "OutputUBX=%s", emulator.getOutputUBX() ? "true" : "false");
        displayPV.prt_str(_dispStr, 20, 0, 56);
        sprintf(_dispStr, "MeasureRate=%d", emulator.getMeasurementRate());
        displayPV.prt_str(_dispStr, 20, 0, 72);
        sprintf(_dispStr, "NavigationRate=%d", emulator.getNavigationRate());
        displayPV.prt_str(_dispStr, 20, 0, 88);
        sprintf(_dispStr, "AutoPVTRate=%d", emulator.getAutoPVTRate());
        displayPV.prt_str(_dispStr, 20, 0, 104);
        sprintf(_dispStr, "   PVT OUTPUT PKT");
        displayPV.prt_str(_dispStr, 20, 0, 130);
        displayPV.prt_str(getRTCClockISO8601DateTimeStr(), 19, 6, 146);
        ubxNAVPVTInfo_t _ubxNAVPVTInfo = emulator.getPVTPacketInfo();
        sprintf(_dispStr, "Lat=%s", getLatitudeStr(gps.getLatitude()));
        displayPV.prt_str(_dispStr, 20, 0, 162);
        sprintf(_dispStr, "Lon=%s", getLongitudeStr(gps.getLongitude()));
        displayPV.prt_str(_dispStr, 20, 0, 178);
        sprintf(_dispStr, "ALT=%03d", max(min(_ubxNAVPVTInfo.altitudeMSL / 1000, 999), -99));
        displayPV.prt_str(_dispStr, 8, 0, 194);
        sprintf(_dispStr, "HA=%03d", min(_ubxNAVPVTInfo.hAcc / 1000, 999));
        displayPV.prt_str(_dispStr, 8, 90, 194);
        sprintf(_dispStr, "VA=%03d", min(_ubxNAVPVTInfo.vAcc / 1000, 999));
        displayPV.prt_str(_dispStr, 8, 168, 194);
        sprintf(_dispStr, "H=%s", getHeadingStr((float)_ubxNAVPVTInfo.headMot * 1e-5));
        displayPV.prt_str(_dispStr, 5, 0, 210);
        sprintf(_dispStr, "F=%d", min(_ubxNAVPVTInfo.fixType, 9));
        displayPV.prt_str(_dispStr, 3, 72, 210);
        sprintf(_dispStr, "S=%02d", min(_ubxNAVPVTInfo.numSV, 99));
        displayPV.prt_str(_dispStr, 4, 114, 210);
        displayPV.prt_str("PP=", 3, 168, 210);
        displayPV.prt_float(min((float)_ubxNAVPVTInfo.pDOP * 1e-2, 9.9), 3, 1, 204, 210);
      //sprintf(_dispStr, "RXP CL%02XID%02XPL%02dV%d",
      //        emulator.receivedPacket.messageClass,
      //        emulator.receivedPacket.messageID,
      //        emulator.receivedPacket.payloadLength,
      //        emulator.receivedPacket.validPacket);
      //displayPV.prt_str(_dispStr, 20, 0, 32);
      //sprintf(_dispStr, "RSP CL%02XID%02XPL%02dV%d",
      //        emulator.responsePacket.messageClass,
      //        emulator.responsePacket.messageID,
      //        emulator.responsePacket.payloadLength,
      //        emulator.responsePacket.validPacket);
      //displayPV.prt_str(_dispStr, 20, 0, 48);
      //sprintf(_dispStr, "AKP CL%02XID%02XPL%02dV%d",
      //        emulator.acknowledgePacket.messageClass,
      //        emulator.acknowledgePacket.messageID,
      //        emulator.acknowledgePacket.payloadLength,
      //        emulator.acknowledgePacket.validPacket);
      //displayPV.prt_str(_dispStr, 20, 0, 64);
      //sprintf(_dispStr, "UNP CL%02XID%02XPL%02dV%d",
      //        emulator.unknownPacket.messageClass,
      //        emulator.unknownPacket.messageID,
      //        emulator.unknownPacket.payloadLength,
      //        emulator.unknownPacket.validPacket);
      //displayPV.prt_str(_dispStr, 20, 0, 80);
      }
    }
    msg_update(nullptr);
    displayRefresh = false;
    displayUpdate = true;
  }
}

/********************************************************************/
void display_update() {
  if(displayUpdate) {
    display_display();
    displayUpdate = false;
  }
}

