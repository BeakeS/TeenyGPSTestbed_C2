/*
TeenyGPSEmulate.h - Class file for the TeenyGPSEmulate Arduino Library.
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
#include "TeenyGPSEmulate.h"

/********************************************************************/
TeenyGPSEmulate::TeenyGPSEmulate() { }

TeenyGPSEmulate::~TeenyGPSEmulate() { }

/********************************************************************/
bool TeenyGPSEmulate::init(HardwareSerial &serialPort_, uint32_t baudRate_) {
  serialPort = &serialPort_;
  baudRate = baudRate_;
  serialPort->begin(baudRate);
  reset();
  return true;
}

/********************************************************************/
bool TeenyGPSEmulate::reset() {
  lostRxPacketCount = 0;
  requestPVTPacket = false;
  lostPVTRequestCount = 0;
  loopPacketNum = 0;
  return true;
}

/********************************************************************/
/********************************************************************/
// Methods for receiving host commands and sending replies and ACK/NAK
/********************************************************************/
/********************************************************************/
void TeenyGPSEmulate::tick() {
  while(serialPort->available()) {
    if(incomingPacket.validPacket) return;
    processIncomingByte(serialPort->read());
  }
}

/********************************************************************/
void TeenyGPSEmulate::processIncomingByte(uint8_t incomingByte) {

  // synch1
  if(!incomingPacket.receivingPacket) {
    if(incomingByte == incomingPacket.synch1) {
      // This is the start of a binary sentence. Reset flags.
      // Reset the packet byte counter
      incomingPacket.receivingPacket = true;
      incomingPacket.packetCounter = 1;
    } else {
      // This character is unknown or we missed the previous start of a sentence
    }

  // synch2
  } else if(incomingPacket.receivingPacket &&
            (incomingPacket.packetCounter == 1)) {
    if(incomingByte == incomingPacket.synch2) {
      incomingPacket.packetCounter++;
      incomingPacket.rollingChecksumA = 0;
      incomingPacket.rollingChecksumB = 0;
    } else {
      // This character is unknown or we missed the previous start of a sentence
      incomingPacket.receivingPacket = false;
    }

  // messageClass, messageID, payloadLength
  } else if(incomingPacket.receivingPacket &&
            (incomingPacket.packetCounter < 6)) {
    if(incomingPacket.packetCounter == 2) {
      incomingPacket.messageClass = incomingByte;
    } else if(incomingPacket.packetCounter == 3) {
      incomingPacket.messageID = incomingByte;
    } else if(incomingPacket.packetCounter == 4) {
      incomingPacket.payloadLength = incomingByte;
    } else if(incomingPacket.packetCounter == 5) {
      incomingPacket.payloadLength |= (incomingByte << 8);
      incomingPacket.payloadCounter = 0;
    }
    incomingPacket.packetCounter++;
    incomingPacket.rollingChecksumA += incomingByte;
    incomingPacket.rollingChecksumB += incomingPacket.rollingChecksumA;

  // payload
  } else if(incomingPacket.receivingPacket &&
            (incomingPacket.payloadCounter < incomingPacket.payloadLength)) {
    if(incomingPacket.payloadCounter < sizeof(incomingPacket.payload)) {
      // Not storing large full packets, just checking and responding with NAK
      incomingPacket.payload[incomingPacket.payloadCounter] = incomingByte;
    }
    incomingPacket.payloadCounter++;
    incomingPacket.packetCounter++;
    incomingPacket.rollingChecksumA += incomingByte;
    incomingPacket.rollingChecksumB += incomingPacket.rollingChecksumA;

  // checksumA
  } else if(incomingPacket.receivingPacket &&
            (incomingPacket.packetCounter == (incomingPacket.payloadLength + 6))) {
    incomingPacket.checksumA = incomingByte;
    incomingPacket.packetCounter++;

  // checksumB
  } else if(incomingPacket.receivingPacket &&
            (incomingPacket.packetCounter == (incomingPacket.payloadLength + 7))) {
    incomingPacket.checksumB = incomingByte;
    incomingPacket.packetCounter++;
    if((incomingPacket.checksumA == incomingPacket.rollingChecksumA) &&
       (incomingPacket.checksumB == incomingPacket.rollingChecksumB)) {
      incomingPacket.validPacket = true;
    }
    incomingPacket.receivingPacket = false;
  }
}

/********************************************************************/
void TeenyGPSEmulate::processIncomingPacket() {
  if(incomingPacket.validPacket) {
    if(receivedPacket.validPacket) {
      // Lost rx packet
      lostRxPacketCount += (lostRxPacketCount < 99) ? 1 : 0;
    } else {
      receivedPacket = incomingPacket;
    }
    incomingPacket.validPacket = false;
  }
  if(receivedPacket.validPacket) {

    // Poll CFG-PRT COM_PORT_UART1
    if((receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
       (receivedPacket.messageID == TGPSE_UBX_CFG_PRT) &&
       (receivedPacket.payloadLength == 1) &&
       (receivedPacket.payload[0] == TGPSE_COM_PORT_UART1)) {
      // Return configuration for COM_PORT_UART1
      responsePacket.messageClass = receivedPacket.messageClass;
      responsePacket.messageID = receivedPacket.messageID;
      responsePacket.payloadLength = TGPSE_UBX_CFG_PRT_PAYLOADLENGTH;
      memcpy(responsePacket.payload, TGPSE_UBX_CFG_PRT_PAYLOAD, TGPSE_UBX_CFG_PRT_PAYLOADLENGTH);
      responsePacket.payload[0] = TGPSE_COM_PORT_UART1;
      responsePacket.payload[8] = baudRate & 0xFF;
      responsePacket.payload[9] = baudRate >> 8;
      responsePacket.payload[10] = baudRate >> 16;
      responsePacket.payload[11] = baudRate >> 24;
      responsePacket.payload[14] = emulatorSettings.outputUBX ? 0x01 : 0x00;
      calcChecksum(&responsePacket);
      responsePacket.validPacket = true;
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // Set CFG-PRT COM_PORT_UART1
    } else if((receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_PRT) &&
              (receivedPacket.payloadLength == TGPSE_UBX_CFG_PRT_PAYLOADLENGTH) &&
              (receivedPacket.payload[0] == TGPSE_COM_PORT_UART1)) {
      // Update configuration for COM_PORT_UART1 and ACK if not changing baudRate
      requestedBaudRate = receivedPacket.payload[8];
      requestedBaudRate |= receivedPacket.payload[9] << 8;
      requestedBaudRate |= receivedPacket.payload[10] << 16;
      requestedBaudRate |= receivedPacket.payload[11] << 24;
      if(requestedBaudRate != baudRate) {
        if((requestedBaudRate == 9600) ||
           (requestedBaudRate == 38400) ||
           (requestedBaudRate == 115200)) {
          init(*serialPort, requestedBaudRate);
          receivedPacket.validPacket = false;
          return;
        } else {
          buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, false);
        }
      } else if(((receivedPacket.payload[14] & 0xFE) != 0x00) ||
                (receivedPacket.payload[15] != 0x00)) {
        buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, false);
      } else {
        emulatorSettings.outputUBX = receivedPacket.payload[14] & 0x01;
        buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);
      }

    // Poll CFG-RATE (measureRate and navigationRate)
    } else if((receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_RATE) &&
              (receivedPacket.payloadLength == 0)) {
      // Return measureRate and navigationRate
      responsePacket.messageClass = receivedPacket.messageClass;
      responsePacket.messageID = receivedPacket.messageID;
      responsePacket.payloadLength = TGPSE_UBX_CFG_RATE_PAYLOADLENGTH;
      memcpy(responsePacket.payload, TGPSE_UBX_CFG_RATE_PAYLOAD, TGPSE_UBX_CFG_RATE_PAYLOADLENGTH);
      responsePacket.payload[0] = emulatorSettings.measureRate & 0xFF;
      responsePacket.payload[1] = emulatorSettings.measureRate >> 8;
      responsePacket.payload[2] = emulatorSettings.navigationRate & 0xFF;
      responsePacket.payload[3] = emulatorSettings.navigationRate >> 8;
      calcChecksum(&responsePacket);
      responsePacket.validPacket = true;
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // Set CFG-RATE (measureRate and navigationRate)
    } else if((receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_RATE) &&
              (receivedPacket.payloadLength == TGPSE_UBX_CFG_RATE_PAYLOADLENGTH)) {
      // Update measureRate and navigationRate
      emulatorSettings.measureRate = receivedPacket.payload[0];
      emulatorSettings.measureRate |= receivedPacket.payload[1] << 8;
      emulatorSettings.navigationRate = receivedPacket.payload[2];
      emulatorSettings.navigationRate |= receivedPacket.payload[3] << 8;
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // Poll CFG-MSG (autoPVTRate)
    } else if((receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_MSG) &&
              (receivedPacket.payloadLength == 2) &&
              (receivedPacket.payload[0] == TGPSE_UBX_CLASS_NAV) &&
              (receivedPacket.payload[1] == TGPSE_UBX_NAV_PVT)) {
      // Return autoPVTRate setting
      responsePacket.messageClass = receivedPacket.messageClass;
      responsePacket.messageID = receivedPacket.messageID;
      responsePacket.payloadLength = 3;
      responsePacket.payload[0] = TGPSE_UBX_CLASS_NAV;
      responsePacket.payload[1] = TGPSE_UBX_NAV_PVT;
      responsePacket.payload[2] = emulatorSettings.autoPVTRate;
      calcChecksum(&responsePacket);
      responsePacket.validPacket = true;
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // Set CFG-MSG (autoPVTRate)
    } else if((receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_MSG) &&
              (receivedPacket.payloadLength == 3)) {
      if((receivedPacket.payload[0] == TGPSE_UBX_CLASS_NAV) &&
         (receivedPacket.payload[1] == TGPSE_UBX_NAV_PVT)) {
        // Update autoPVTRate setting
        emulatorSettings.autoPVTRate = receivedPacket.payload[2];
        buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);
      } else {
        buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, false);
      }

    // Poll NAV-PVT (Navigation position velocity time solution - manual polling mode)
    } else if((receivedPacket.messageClass == TGPSE_UBX_CLASS_NAV) &&
              (receivedPacket.messageID == TGPSE_UBX_NAV_PVT) &&
              (receivedPacket.payloadLength == 0)) {
      // Request NAV-PVT packet if not in autoPVT mode
      if(emulatorSettings.autoPVTRate!=0) {
        if(requestPVTPacket) {
          lostPVTRequestCount += (lostPVTRequestCount < 99) ? 1 : 0;
        } else {
          requestPVTPacket = true;
        }
      }
      // *** DON'T ACK UBX-NAV-PVT REQUESTS ***

    // Poll MON-VER (receiver and software versions)
    } else if((receivedPacket.messageClass == TGPSE_UBX_CLASS_MON) &&
              (receivedPacket.messageID == TGPSE_UBX_MON_VER) &&
              (receivedPacket.payloadLength == 0)) {
      // Return receiver and software versions
      responsePacket.messageClass = receivedPacket.messageClass;
      responsePacket.messageID = receivedPacket.messageID;
      responsePacket.payloadLength = TGPSE_UBX_MON_VER_PAYLOADLENGTH;
      memcpy(responsePacket.payload, TGPSE_UBX_MON_VER_PAYLOAD, TGPSE_UBX_MON_VER_PAYLOADLENGTH);
      calcChecksum(&responsePacket);
      responsePacket.validPacket = true;
      // *** DON'T ACK UBX-MON-VER REQUESTS ***

    // NAK anything else (because it is not supported
    } else {
      unknownPacket = receivedPacket;
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, false);
    }

    receivedPacket.validPacket = false;
    sendPackets();
  }
}

/********************************************************************/
uint8_t TeenyGPSEmulate::getLostRxPacketCount() {
  return lostRxPacketCount;
}

/********************************************************************/
void TeenyGPSEmulate::buildAcknowledgePacket(uint8_t messageClass, uint8_t messageID, bool ack) {
  acknowledgePacket.messageClass = TGPSE_UBX_CLASS_ACK;
  acknowledgePacket.messageID = ack ? TGPSE_UBX_ACK_ACK : TGPSE_UBX_ACK_NAK;
  acknowledgePacket.payloadLength = 2;
  acknowledgePacket.payload[0] = messageClass;
  acknowledgePacket.payload[1] = messageID;
  calcChecksum(&acknowledgePacket);
  acknowledgePacket.validPacket = true;
}

/********************************************************************/
void TeenyGPSEmulate::sendPackets() {
  if(responsePacket.validPacket) {
    sendPacket(&responsePacket);
    responsePacket.validPacket = false;
  }
  if(acknowledgePacket.validPacket) {
    sendPacket(&acknowledgePacket);
    acknowledgePacket.validPacket = false;
  }
}

/********************************************************************/
void TeenyGPSEmulate::sendPacket(ubxPacket_t *pkt) {
  serialPort->write(pkt->synch1);
  serialPort->write(pkt->synch2);
  serialPort->write(pkt->messageClass);
  serialPort->write(pkt->messageID);
  serialPort->write(pkt->payloadLength & 0xFF);
  serialPort->write(pkt->payloadLength >> 8);
  //for(uint16_t i = 0; i < pkt->payloadLength; i++) {
  //  serialPort->write(pkt->payload[i]);
  //}
  serialPort->write(pkt->payload, pkt->payloadLength);
  serialPort->write(pkt->checksumA);
  serialPort->write(pkt->checksumB);
}

/********************************************************************/
/********************************************************************/
// Methods for accessing emulator state
/********************************************************************/
/********************************************************************/
uint32_t TeenyGPSEmulate::getBaudRate() {
  return emulatorSettings.baudRate;
}

/********************************************************************/
bool TeenyGPSEmulate::getOutputUBX() {
  return emulatorSettings.outputUBX;
}

/********************************************************************/
uint16_t TeenyGPSEmulate::getMeasurementRate() {
  return emulatorSettings.measureRate;
}

/********************************************************************/
uint16_t TeenyGPSEmulate::getNavigationRate() {
  return emulatorSettings.navigationRate;
}

/********************************************************************/
uint8_t TeenyGPSEmulate::getAutoPVTRate() {
  return emulatorSettings.autoPVTRate;
}

/********************************************************************/
/********************************************************************/
// Methods for manual and automatic UBX PVT transmission
/********************************************************************/
/********************************************************************/
uint32_t TeenyGPSEmulate::getPVTTransmissionRate() {
  if(emulatorSettings.autoPVTRate == 0) {
    return 0;
  }
  return ((uint32_t)emulatorSettings.measureRate *
                    emulatorSettings.navigationRate) / emulatorSettings.autoPVTRate;
}

/********************************************************************/
bool TeenyGPSEmulate::isPVTPacketRequested() {
  if(requestPVTPacket) {
    requestPVTPacket = false;
    return true;
  }
  return false;
}

/********************************************************************/
uint8_t TeenyGPSEmulate::getLostPVTRequestCount() {
  return lostPVTRequestCount;
}

/********************************************************************/
void TeenyGPSEmulate::setAutoPVTRate(uint8_t rate) {
  emulatorSettings.autoPVTRate = rate;
}

/********************************************************************/
bool TeenyGPSEmulate::isAutoPVTEnabled() {
  return emulatorSettings.autoPVTRate!=0;
}

/********************************************************************/
bool TeenyGPSEmulate::setPVTPacket(uint8_t *buf, size_t size) {
  if(size != sizeof(ubxPVTPacket)) return false;
  memcpy((uint8_t*)&ubxPVTPacket, buf, size);
  return true;
}

/********************************************************************/
bool TeenyGPSEmulate::setPVTColdPacket() {
  memcpy(&ubxPVTPacket.payload, TGPSE_UBX_NAV_PVT_COLD_PAYLOAD, TGPSE_UBX_NAV_PVT_PAYLOADLENGTH);
  return true;
}

/********************************************************************/
bool TeenyGPSEmulate::setPVTLoopPacket() {
  memcpy(&ubxPVTPacket, TGPSE_UBX_NAVPVT_LOOP_PACKETS[loopPacketNum], TGPSE_UBX_NAV_PVT_PACKETLENGTH);
  loopPacketNum++;
  if(loopPacketNum >= (sizeof(TGPSE_UBX_NAVPVT_LOOP_PACKETS) /
                       sizeof(TGPSE_UBX_NAVPVT_LOOP_PACKETS[0]))) {
    loopPacketNum = 0;
  }
  return true;
}

/********************************************************************/
ubxNAVPVTInfo_t TeenyGPSEmulate::getPVTPacketInfo() {
  ubxNAVPVTInfo.year = ubxPVTPacket.payload[4];
  ubxNAVPVTInfo.year |= ubxPVTPacket.payload[5] << 8;
  ubxNAVPVTInfo.month = ubxPVTPacket.payload[6];
  ubxNAVPVTInfo.day = ubxPVTPacket.payload[7];
  ubxNAVPVTInfo.hour = ubxPVTPacket.payload[8];
  ubxNAVPVTInfo.minute = ubxPVTPacket.payload[9];
  ubxNAVPVTInfo.second = ubxPVTPacket.payload[10];
  ubxNAVPVTInfo.dateValid = ubxPVTPacket.payload[11] & 0x01;
  ubxNAVPVTInfo.timeValid = ubxPVTPacket.payload[11] & 0x02;
  ubxNAVPVTInfo.tAcc = ubxPVTPacket.payload[12];
  ubxNAVPVTInfo.tAcc |= ubxPVTPacket.payload[13] << 8;
  ubxNAVPVTInfo.tAcc |= ubxPVTPacket.payload[14] << 16;
  ubxNAVPVTInfo.tAcc |= ubxPVTPacket.payload[15] << 24;
  ubxNAVPVTInfo.tAcc = ubxNAVPVTInfo.tAcc;
  ubxNAVPVTInfo.fixType = ubxPVTPacket.payload[20];
  ubxNAVPVTInfo.locationValid = ubxPVTPacket.payload[21] & 0x01;
  ubxNAVPVTInfo.numSV = ubxPVTPacket.payload[23];
  ubxNAVPVTInfo.longitude = ubxPVTPacket.payload[24];
  ubxNAVPVTInfo.longitude |= ubxPVTPacket.payload[25] << 8;
  ubxNAVPVTInfo.longitude |= ubxPVTPacket.payload[26] << 16;
  ubxNAVPVTInfo.longitude |= ubxPVTPacket.payload[27] << 24;
  ubxNAVPVTInfo.latitude = ubxPVTPacket.payload[28];
  ubxNAVPVTInfo.latitude |= ubxPVTPacket.payload[29] << 8;
  ubxNAVPVTInfo.latitude |= ubxPVTPacket.payload[30] << 16;
  ubxNAVPVTInfo.latitude |= ubxPVTPacket.payload[31] << 24;
  ubxNAVPVTInfo.altitudeMSL = ubxPVTPacket.payload[36];
  ubxNAVPVTInfo.altitudeMSL |= ubxPVTPacket.payload[37] << 8;
  ubxNAVPVTInfo.altitudeMSL |= ubxPVTPacket.payload[38] << 16;
  ubxNAVPVTInfo.altitudeMSL |= ubxPVTPacket.payload[39] << 24;
  ubxNAVPVTInfo.altitudeMSL = ubxNAVPVTInfo.altitudeMSL;
  ubxNAVPVTInfo.hAcc = ubxPVTPacket.payload[40];
  ubxNAVPVTInfo.hAcc |= ubxPVTPacket.payload[41] << 8;
  ubxNAVPVTInfo.hAcc |= ubxPVTPacket.payload[42] << 16;
  ubxNAVPVTInfo.hAcc |= ubxPVTPacket.payload[43] << 24;
  ubxNAVPVTInfo.hAcc = ubxNAVPVTInfo.hAcc;
  ubxNAVPVTInfo.vAcc = ubxPVTPacket.payload[44];
  ubxNAVPVTInfo.vAcc |= ubxPVTPacket.payload[45] << 8;
  ubxNAVPVTInfo.vAcc |= ubxPVTPacket.payload[46] << 16;
  ubxNAVPVTInfo.vAcc |= ubxPVTPacket.payload[47] << 24;
  ubxNAVPVTInfo.vAcc = ubxNAVPVTInfo.vAcc;
  ubxNAVPVTInfo.headMot = ubxPVTPacket.payload[64];
  ubxNAVPVTInfo.headMot |= ubxPVTPacket.payload[65] << 8;
  ubxNAVPVTInfo.headMot |= ubxPVTPacket.payload[66] << 16;
  ubxNAVPVTInfo.headMot |= ubxPVTPacket.payload[67] << 24;
  ubxNAVPVTInfo.pDOP = ubxPVTPacket.payload[76];
  ubxNAVPVTInfo.pDOP |= ubxPVTPacket.payload[77] << 8;
  return ubxNAVPVTInfo;
}

/********************************************************************/
void TeenyGPSEmulate::setPVTPacketDateTime(uint16_t year, uint8_t month, uint8_t day,
                                           uint8_t hour, uint8_t minute, uint8_t second) {
  ubxPVTPacket.payload[4] = year & 0xFF;
  ubxPVTPacket.payload[5] = year >> 8;
  ubxPVTPacket.payload[6] = month;
  ubxPVTPacket.payload[7] = day;
  ubxPVTPacket.payload[8] = hour;
  ubxPVTPacket.payload[9] = minute;
  ubxPVTPacket.payload[10] = second;
}

/********************************************************************/
void TeenyGPSEmulate::unsetPVTPacketDateValidFlag() {
  ubxPVTPacket.payload[11] &= ~0x01;
  // Can't have time and location valid without valid date
  unsetPVTPacketTimeValidFlag();
}
/********************************************************************/
void TeenyGPSEmulate::unsetPVTPacketTimeValidFlag() {
  ubxPVTPacket.payload[11] &= ~0x02;
  // Can't have location valid without valid time
  unsetPVTPacketLocationValidFlag();
}
/********************************************************************/
void TeenyGPSEmulate::unsetPVTPacketLocationValidFlag() {
  ubxPVTPacket.payload[21] &= ~0x01;
}

/********************************************************************/
bool TeenyGPSEmulate::sendPVTPacket() {
  calcChecksum(&ubxPVTPacket);
  if(serialPort->write((uint8_t*)&ubxPVTPacket, sizeof(ubxPVTPacket)) == sizeof(ubxPVTPacket)) {
    return true;
  }
  return false;
}

/********************************************************************/
/********************************************************************/
// checksum
/********************************************************************/
/********************************************************************/
// Given a message, calc and store the two byte "8-Bit Fletcher" checksum over the entirety of the message
// This is called before we send a command message
void TeenyGPSEmulate::calcChecksum(ubxPacket_t *pkt) {
  pkt->checksumA = 0;
  pkt->checksumB = 0;

  pkt->checksumA += pkt->messageClass;
  pkt->checksumB += pkt->checksumA;

  pkt->checksumA += pkt->messageID;
  pkt->checksumB += pkt->checksumA;

  pkt->checksumA += (pkt->payloadLength & 0xFF);
  pkt->checksumB += pkt->checksumA;

  pkt->checksumA += (pkt->payloadLength >> 8);
  pkt->checksumB += pkt->checksumA;

  for(uint16_t i = 0; i < pkt->payloadLength; i++) {
    pkt->checksumA += pkt->payload[i];
    pkt->checksumB += pkt->checksumA;
  }
}
/********************************************************************/
void TeenyGPSEmulate::calcChecksum(ubxPVTPacket_t *pkt) {
  pkt->checksumA = 0;
  pkt->checksumB = 0;

  pkt->checksumA += pkt->messageClass;
  pkt->checksumB += pkt->checksumA;

  pkt->checksumA += pkt->messageID;
  pkt->checksumB += pkt->checksumA;

  pkt->checksumA += (pkt->payloadLength & 0xFF);
  pkt->checksumB += pkt->checksumA;

  pkt->checksumA += (pkt->payloadLength >> 8);
  pkt->checksumB += pkt->checksumA;

  for(uint16_t i = 0; i < pkt->payloadLength; i++) {
    pkt->checksumA += pkt->payload[i];
    pkt->checksumB += pkt->checksumA;
  }
}

