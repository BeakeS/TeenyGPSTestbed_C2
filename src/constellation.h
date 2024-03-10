
/********************************************************************/
// Satellite Constellation

uint8_t drawSatConstellation() {

  ubloxPacket_t navsatPacket;
  ubloxNAVSATInfo_t navsatInfo;

  int16_t  mapRadius = 118;
  int16_t  centerX = 120;
  int16_t  centerY = 180;
  uint8_t  numSats = 0;
  int16_t  satX;
  int16_t  satY;
  int16_t  satRadius = 12;
  uint32_t satColor;
  int16_t  satRingRadius = 14;
  uint32_t satRingColor;
  char _dispStr[4];

  // draw main circles, one at 0deg, and one at 45deg elevation
  display.drawCircle(centerX, centerY, mapRadius, WHITE);
  display.drawCircle(centerX, centerY, 60, WHITE);

  // draw lines at 0, 45, 90, 135 etc degrees azimuth
  for (int16_t i = 0; i < 8; i++) {
    int16_t satX = sin(radians((i * 45) + 90)) * mapRadius;
    int16_t satY = cos(radians((i * 45) + 90)) * mapRadius;
    display.drawLine(centerX, centerY, satX + centerX, satY + centerY, WHITE);
  }

  // Add map labels
  display.fillRect(centerX - 9, centerY - mapRadius, 18, 17, BLACK);
  displayPV.prt_char('N', 1, centerX - 5, centerY - mapRadius);
  display.fillRect(centerX - 9, centerY + mapRadius - 16, 18, 17, BLACK);
  displayPV.prt_char('S', 1, centerX - 5, centerY + mapRadius - 13);
  display.fillRect(centerX + mapRadius - 15, centerY - 11, 16, 22, BLACK);
  displayPV.prt_char('E', 1, centerX + mapRadius - 10, centerY - 7);
  display.fillRect(centerX - mapRadius, centerY - 11, 16, 22, BLACK);
  displayPV.prt_char('W', 1, centerX - mapRadius, centerY - 7);

  // Check NAVSAT Packet
  gps.getNAVSATPacket(navsatPacket);
  gps.getNAVSATInfo(navsatInfo);
  if(!navsatPacket.validPacket) {
    return numSats;
  }

  // Set text size for satellite IDs
  display.setTextSize(1); // font is 6x8

  // draw the positions of the sats
  for(int16_t i = navsatInfo.numSvsHealthy -1; i >= 0; i--) {
    if((navsatInfo.svSortList[i].elevation < -90) ||
       (navsatInfo.svSortList[i].elevation > 90)) {
      continue;
    }

    // Sat position
    numSats ++;
    satX = -sin(radians(navsatInfo.svSortList[i].azimuth + 180)) *
           map(navsatInfo.svSortList[i].elevation, 0, 90, mapRadius-6, 1);
    satY = cos(radians(navsatInfo.svSortList[i].azimuth + 180)) *
           map(navsatInfo.svSortList[i].elevation, 0, 90, mapRadius-6, 1);

    // Sat color based on svUsed vs just svHealth
    if(i < navsatInfo.numSvsUsed) {
      satColor = BLUE;
    } else {
      satColor = RED;
    }
    display.fillCircle(satX + centerX, satY + centerY, satRadius, satColor);

    // Sat ring color based on SNR
    if(navsatInfo.svSortList[i].cno >= 35) {
      satRingColor = WHITE;
    } else if(navsatInfo.svSortList[i].cno >=20) {
      satRingColor = YELLOW;
    } else {
      satRingColor = ORANGE;
    }
    display.drawCircle(satX + centerX, satY + centerY, satRingRadius, satRingColor);
    display.drawCircle(satX + centerX, satY + centerY, satRingRadius-1, satRingColor);

    // Sat label
    sprintf(_dispStr, "%c%02d",
            navsatInfo.svSortList[i].gnssIdType,
            navsatInfo.svSortList[i].svId);
    displayPV.prt_str(_dispStr, 3, satX + centerX - 8, satY + centerY - 3);
  }

  // Restore text size
  display.setTextSize(2); // font is 6x8

  // Return number of sats
  return numSats;

}

