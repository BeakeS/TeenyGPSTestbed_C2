
/********************************************************************/
// Satellite Constellation

uint8_t drawSatConstellation() {

  ubloxPacket_t navsatPacket;
  ubloxNAVSATInfo_t navsatInfo;

  int16_t  mapRadius = 118;
  int16_t  mapSatRadius = 107; // Keep sat graphics inside map
  int16_t  mapCenterX = 120;
  int16_t  mapCenterY = 180;
  uint8_t  numSats = 0;
  int16_t  satX;
  int16_t  satY;
  int16_t  satRadius = 12;
  uint32_t satColor;
  int16_t  satRingRadius = 14;
  uint32_t satRingColor;
  char _dispStr[4];

  // draw main circles, one at 0deg, and one at 45deg elevation
  display.drawCircle(mapCenterX, mapCenterY, mapRadius, WHITE);
  display.drawCircle(mapCenterX, mapCenterY, 60, WHITE);

  // draw lines at 0, 45, 90, 135 etc degrees azimuth
  for (int16_t i = 0; i < 8; i++) {
    int16_t satX = sin(radians((i * 45) + 90)) * mapRadius;
    int16_t satY = cos(radians((i * 45) + 90)) * mapRadius;
    display.drawLine(mapCenterX, mapCenterY, satX + mapCenterX, satY + mapCenterY, WHITE);
  }

  // Add map labels
  display.fillRect(mapCenterX - 9, mapCenterY - mapRadius, 18, 17, BLACK);
  displayPV.prt_char('N', 1, mapCenterX - 5, mapCenterY - mapRadius);
  display.fillRect(mapCenterX - 9, mapCenterY + mapRadius - 16, 18, 17, BLACK);
  displayPV.prt_char('S', 1, mapCenterX - 5, mapCenterY + mapRadius - 13);
  display.fillRect(mapCenterX + mapRadius - 15, mapCenterY - 11, 16, 22, BLACK);
  displayPV.prt_char('E', 1, mapCenterX + mapRadius - 10, mapCenterY - 7);
  display.fillRect(mapCenterX - mapRadius, mapCenterY - 11, 16, 22, BLACK);
  displayPV.prt_char('W', 1, mapCenterX - mapRadius, mapCenterY - 7);

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
           map(navsatInfo.svSortList[i].elevation, 0, 90, mapSatRadius, 1);
    satY = cos(radians(navsatInfo.svSortList[i].azimuth + 180)) *
           map(navsatInfo.svSortList[i].elevation, 0, 90, mapSatRadius, 1);

    // Sat color based on svUsed vs just svHealth
    if(i < navsatInfo.numSvsUsed) {
      satColor = BLUE;
    } else {
      satColor = RED;
    }
    display.fillCircle(satX + mapCenterX, satY + mapCenterY, satRadius, satColor);

    // Sat ring color based on SNR
    if(navsatInfo.svSortList[i].cno >= 35) {
      satRingColor = WHITE;
    } else if(navsatInfo.svSortList[i].cno >=20) {
      satRingColor = YELLOW;
    } else {
      satRingColor = ORANGE;
    }
    display.drawCircle(satX + mapCenterX, satY + mapCenterY, satRingRadius, satRingColor);
    display.drawCircle(satX + mapCenterX, satY + mapCenterY, satRingRadius-1, satRingColor);

    // Sat label
    sprintf(_dispStr, "%c%02d",
            navsatInfo.svSortList[i].gnssIdType,
            navsatInfo.svSortList[i].svId);
    displayPV.prt_str(_dispStr, 3, satX + mapCenterX - 8, satY + mapCenterY - 3);
  }

  // Restore text size
  display.setTextSize(2); // font is 6x8

  // Return number of sats
  return numSats;

}

