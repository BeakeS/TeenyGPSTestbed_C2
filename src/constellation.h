
/********************************************************************/
// Satellite Constellation Map Generator

uint8_t drawSatConstellation(int16_t compAngle) {

  ubloxNAVSATInfo_t navsatInfo;

  int16_t  mapRadius = 118;
  int16_t  mapSatRadius = 106; // Keep sat graphics inside map
  int16_t  mapCenterX = 120;
  int16_t  mapCenterY = 162;
  uint8_t  numSats = 0;
  int16_t  xCoord;
  int16_t  yCoord;
  int16_t  satRadius = 10;
  uint32_t satColor;
  int16_t  satRingRadius = 12;
  uint32_t satRingColor;
  char _dispStr[4];

  // draw main circles, one at 0deg, and one at 45deg elevation
  display.drawCircle(mapCenterX, mapCenterY, mapRadius, WHITE);
  display.drawCircle(mapCenterX, mapCenterY, (mapRadius>>1)+1, WHITE);

  // draw lines at 0, 45, 90, 135 etc degrees azimuth
  for (int16_t i = 0; i <= 7; i++) {
    xCoord = round(-sin(radians((i * 45) + 180 + compAngle)) * mapRadius);
    yCoord = round(cos(radians((i * 45) + 180 + compAngle)) * mapRadius);
    display.drawLine(mapCenterX, mapCenterY, xCoord + mapCenterX, yCoord + mapCenterY, WHITE);
    if(i % 2) continue;
    xCoord = round(-sin(radians((i * 45) + 180 + compAngle)) * (mapRadius - 8));
    yCoord = round(cos(radians((i * 45) + 180 + compAngle)) * (mapRadius - 8));
    display.fillCircle(xCoord + mapCenterX, yCoord + mapCenterY, 12, BLACK);
    char label;
    switch(i) {
      case 0: label = 'N'; break;
      case 2: label = 'E'; break;
      case 4: label = 'S'; break;
      case 6: label = 'W'; break;
    }
    display.drawChar(xCoord + mapCenterX - 5, yCoord + mapCenterY - 7, label, WHITE, BLACK, 2);
  }

  // Check NAVSAT Packet
  gps.getNAVSATInfo(navsatInfo);
  if(!navsatInfo.validPacket) {
    return numSats;
  }

  // Set text size for satellite IDs
  display.setTextSize(1); // font is 6x8

  // draw the positions of the sats
  for(int16_t i = navsatInfo.numSvsEphValid -1; i >= 0; i--) {

    // Sat position
    numSats ++;
    xCoord = round(-sin(radians(navsatInfo.svSortList[i].azim + 180 + compAngle)) *
                   map(max(navsatInfo.svSortList[i].elev, 0), 0, 90, mapSatRadius, 1));
    yCoord = round(cos(radians(navsatInfo.svSortList[i].azim + 180 + compAngle)) *
                   map(max(navsatInfo.svSortList[i].elev, 0), 0, 90, mapSatRadius, 1));

    // Sat ring color based on SNR
    if(navsatInfo.svSortList[i].cno >= 35) {
      satRingColor = WHITE;
    } else if(navsatInfo.svSortList[i].cno >=20) {
      satRingColor = YELLOW;
    } else {
      satRingColor = ORANGE;
    }
    display.fillCircle(xCoord + mapCenterX, yCoord + mapCenterY, satRingRadius, satRingColor);

    // Sat color based on svUsed
    if(navsatInfo.svSortList[i].svUsed) {
      satColor = BLUE;
    } else if(navsatInfo.svSortList[i].healthy) {
      satColor = RED;
    } else {
      satColor = BLACK;
    }
    display.fillCircle(xCoord + mapCenterX, yCoord + mapCenterY, satRadius, satColor);

    // Sat label
    sprintf(_dispStr, "%c%02d",
            navsatInfo.svSortList[i].gnssIdType,
            navsatInfo.svSortList[i].svId);
    displayPV.prt_str(_dispStr, 3, xCoord + mapCenterX - 8, yCoord + mapCenterY - 3);
  }

  // Restore text size
  display.setTextSize(2); // font is 6x8

  // Return number of sats
  return numSats;

}

