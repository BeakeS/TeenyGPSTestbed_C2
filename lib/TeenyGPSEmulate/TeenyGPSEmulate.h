/*
TeenyGPSEmulate.h - Header file for the TeenyGPSEmulate Arduino Library.
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

#ifndef TeenyGPSEmulate_h
#define TeenyGPSEmulate_h

// Emulator Config Variables
//const uint32_t EMULATOR_BAUD_RATE = 9600;
const uint32_t EMULATOR_BAUD_RATE = 38400;

/*
poll UBX-CFG-PRT  for begin() and setOutputUBX - Return UBX-CFG-PRT and ACK
set  UBX-CFG-PRT  for setOutputUBX - Returns ACK if not changing baudRate
poll UBX-CFG-RATE for setMeasRate and setNavRate - Returns UBX-CFG-RATE and ACK
set  UBX-CFG-RATE for setMeasRate and setNavRate - Returns ACK
set  UBX-CFG-MSG  for setAutoNAVPVT - Returns ACK
set  UBX-CFG-CFG  for saveConfig - Returns ACK
poll UBX-MON-VER  for getProtocolVersion - Returns UBX-MON-VER only
*/

/********************************************************************/
// UBX Packet Frame Defines
/********************************************************************/
const uint8_t  TGPSE_COM_PORT_UART1 = 1;
const uint8_t  TGPSE_COM_TYPE_UBX = 1;
//const uint16_t TGPSE_UBX_MAXPAYLOADLENGTH = 872; // NAV-SAT message with 72 satellites
const uint16_t TGPSE_UBX_MAXPAYLOADLENGTH = 392; // NAV-SAT message with 32 tracking channels
const uint8_t  TGPSE_UBX_SYNCH_1 = 0xB5;
const uint8_t  TGPSE_UBX_SYNCH_2 = 0x62;
const uint8_t  TGPSE_UBX_CLASS_NAV = 0x01;
const uint8_t    TGPSE_UBX_NAV_PVT   = 0x07;
const uint16_t   TGPSE_UBX_NAV_PVT_PAYLOADLENGTH = 92;
const uint16_t   TGPSE_UBX_NAV_PVT_PACKETLENGTH = 100;
const uint8_t    TGPSE_UBX_NAV_SAT = 0x35;
const uint16_t   TGPSE_UBX_NAV_SAT_MINPAYLOADLENGTH = 8;
const uint16_t   TGPSE_UBX_NAV_SAT_MINPACKETLENGTH = TGPSE_UBX_NAV_SAT_MINPAYLOADLENGTH + 8;
const uint16_t   TGPSE_UBX_NAV_SAT_MAXPAYLOADLENGTH = TGPSE_UBX_MAXPAYLOADLENGTH;
const uint16_t   TGPSE_UBX_NAV_SAT_MAXPACKETLENGTH = TGPSE_UBX_NAV_SAT_MAXPAYLOADLENGTH + 8;
const uint8_t  TGPSE_UBX_CLASS_ACK = 0x05;
const uint8_t    TGPSE_UBX_ACK_NAK   = 0x00;
const uint8_t    TGPSE_UBX_ACK_ACK   = 0x01;
const uint16_t   TGPSE_UBX_ACKNAK_PAYLOADLENGTH = 2;
const uint8_t  TGPSE_UBX_CLASS_CFG = 0x06;
const uint8_t    TGPSE_UBX_CFG_PRT   = 0x00;
const uint16_t   TGPSE_UBX_CFG_PRT_PAYLOADLENGTH = 20;
const uint8_t    TGPSE_UBX_CFG_MSG   = 0x01;
const uint8_t    TGPSE_UBX_CFG_RST   = 0x04;
const uint16_t   TGPSE_UBX_CFG_RST_PAYLOADLENGTH = 4;
const uint8_t    TGPSE_UBX_CFG_RATE  = 0x08;
const uint16_t   TGPSE_UBX_CFG_RATE_PAYLOADLENGTH = 6;
const uint8_t    TGPSE_UBX_CFG_CFG   = 0x09;
const uint8_t    TGPSE_UBX_CFG_NAVX5 = 0x23;
const uint16_t   TGPSE_UBX_CFG_NAVX5_PAYLOADLENGTH = 40;
const uint8_t    TGPSE_UBX_CFG_GNSS  = 0x3E;
const uint16_t   TGPSE_UBX_CFG_GNSS_MINPAYLOADLENGTH = 4;
const uint16_t   TGPSE_UBX_CFG_GNSS_MAXPAYLOADLENGTH = 68;
const uint8_t  TGPSE_UBX_CLASS_MON = 0x0A;
const uint8_t    TGPSE_UBX_MON_VER   = 0x04;
const uint16_t   TGPSE_UBX_MON_VER_PAYLOADLENGTH = 160;
const uint8_t    TGPSE_UBX_MON_GNSS   = 0x28;
const uint16_t   TGPSE_UBX_MON_GNSS_PAYLOADLENGTH = 8;

/********************************************************************/
// UBX Packet Struct
/********************************************************************/
typedef struct {
  uint8_t  synch1 = TGPSE_UBX_SYNCH_1;
  uint8_t  synch2 = TGPSE_UBX_SYNCH_2;
  uint8_t  messageClass;
  uint8_t  messageID;
  uint16_t payloadLength;
  uint8_t  pad00a;
  uint8_t  pad00b;
  uint8_t  payload[TGPSE_UBX_MAXPAYLOADLENGTH];
  uint8_t  checksumA;
  uint8_t  checksumB;
  uint8_t  rollingChecksumA;
  uint8_t  rollingChecksumB;
  uint16_t packetCounter;
  uint16_t payloadCounter;
  bool     receivingPacket;
  bool     validPacket;
  uint8_t  pad01a;
  uint8_t  pad01b;
} ubxPacket_t;

/********************************************************************/
// UBX NAVPVT Packet Struct
/********************************************************************/
typedef struct {
  uint8_t  synch1        = TGPSE_UBX_SYNCH_1;
  uint8_t  synch2        = TGPSE_UBX_SYNCH_2;
  uint8_t  messageClass  = TGPSE_UBX_CLASS_NAV;
  uint8_t  messageID     = TGPSE_UBX_NAV_PVT;
  uint16_t payloadLength = TGPSE_UBX_NAV_PVT_PAYLOADLENGTH;
  uint8_t  payload[TGPSE_UBX_NAV_PVT_PAYLOADLENGTH];
  uint8_t  checksumA;
  uint8_t  checksumB;
} ubxNAVPVTPacket_t;

/********************************************************************/
// UBX NAVPVT Info Struct
/********************************************************************/
typedef struct {
  uint16_t year;
  uint8_t  month;
  uint8_t  day;
  uint8_t  hour;
  uint8_t  minute;
  uint8_t  second;
  bool     dateValid;
  bool     timeValid;
  uint8_t  pad00a;
  uint8_t  pad00b;
  uint8_t  pad00c;
  uint32_t tAcc;
  uint8_t  fixType;
  bool     locationValid;
  uint8_t  numSV;
  uint8_t  pad01a;
  int32_t  longitude;
  int32_t  latitude;
  int32_t  altitudeMSL;
  uint32_t hAcc;
  uint32_t vAcc;
  int32_t  headMot;
  uint16_t pDOP;
  uint8_t  pad02a;
  uint8_t  pad02b;
} ubxNAVPVTInfo_t;

/********************************************************************/
// UBX NAVSAT Packet Struct
/********************************************************************/
typedef struct {
  uint8_t  synch1        = TGPSE_UBX_SYNCH_1;
  uint8_t  synch2        = TGPSE_UBX_SYNCH_2;
  uint8_t  messageClass  = TGPSE_UBX_CLASS_NAV;
  uint8_t  messageID     = TGPSE_UBX_NAV_SAT;
  uint16_t payloadLength = TGPSE_UBX_NAV_SAT_MAXPAYLOADLENGTH;
  uint8_t  payload[TGPSE_UBX_NAV_SAT_MAXPAYLOADLENGTH];
  uint8_t  checksumA;
  uint8_t  checksumB;
} ubxNAVSATPacket_t;

/********************************************************************/
// UBX NAVSAT Info Struct
/********************************************************************/
typedef struct {
  uint8_t  numSvs;
  uint8_t  pad00a;
  uint8_t  pad00b;
  uint8_t  pad00c;
} ubxNAVSATInfo_t;

/********************************************************************/
// UBX Packet Payload Defaults
/********************************************************************/
const uint8_t TGPSE_UBX_CFG_PRT_PAYLOAD[TGPSE_UBX_CFG_PRT_PAYLOADLENGTH] = {
  0x01,0x00,0x00,0x00,0xC0,0x08,0x00,0x00,0x00,0x96,
  0x00,0x00,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x00
};
const uint8_t TGPSE_UBX_CFG_RATE_PAYLOAD[TGPSE_UBX_CFG_RATE_PAYLOADLENGTH] = {
  0xE8,0x03,0x01,0x00,0x01,0x00
};
const uint8_t TGPSE_UBX_MON_VER_PAYLOAD[TGPSE_UBX_MON_VER_PAYLOADLENGTH] = {
  0x52,0x4F,0x4D,0x20,0x43,0x4F,0x52,0x45,0x20,0x33,
  0x2E,0x30,0x31,0x20,0x28,0x31,0x30,0x37,0x38,0x38,
  0x38,0x29,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x30,0x30,0x30,0x38,0x30,0x30,0x30,0x30,0x00,0x00,
  0x46,0x57,0x56,0x45,0x52,0x3D,0x53,0x50,0x47,0x20,
  0x33,0x2E,0x30,0x31,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x50,0x52,0x4F,0x54,0x56,0x45,0x52,0x3D,0x31,0x38,
  0x2E,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x47,0x50,0x53,0x3B,0x47,0x4C,0x4F,0x3B,0x47,0x41,
  0x4C,0x3B,0x42,0x44,0x53,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x53,0x42,0x41,0x53,0x3B,0x49,0x4D,0x45,0x53,0x3B,
  0x51,0x5A,0x53,0x53,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const uint8_t TGPSE_UBX_NAV_PVT_COLD_PAYLOAD[TGPSE_UBX_NAV_PVT_PAYLOADLENGTH] = {
  0x10,0x27,0x00,0x00,0xD0,0x07,0x01,0x01,0x00,0x00,
  0x00,0xF0,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,
  0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x4E,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,0x27,0x00,0x00,
  0xE0,0x4A,0x23,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00
};
const uint8_t TGPSE_UBX_NAV_SAT_COLD_PAYLOAD[TGPSE_UBX_NAV_SAT_MINPAYLOADLENGTH] = {
  0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00
};

/********************************************************************/
// Emulator Settings
/********************************************************************/
typedef struct {
  uint32_t baudRate = 38400;
  bool     outputUBX = true;
  uint8_t  pad00a;
  uint8_t  pad00b;
  uint8_t  pad00c;
  uint16_t measureRate = 1000;
  uint8_t  pad01a;
  uint8_t  pad01b;
  uint16_t navigationRate = 1;
  uint8_t  pad02a;
  uint8_t  pad02b;
  uint8_t  autoNAVPVTRate = 0;
  uint8_t  pad03a;
  uint8_t  pad03b;
  uint8_t  pad03c;
  uint8_t  autoNAVSATRate = 0;
  uint8_t  pad04a;
  uint8_t  pad04b;
  uint8_t  pad04c;
} emulatorSettings_t;

/********************************************************************/
// TeenyGPSEmulate Class
/********************************************************************/
class TeenyGPSEmulate {

  public:

    // Constructor / destructor / disallow copy and move
    TeenyGPSEmulate();
    virtual ~TeenyGPSEmulate();
    TeenyGPSEmulate(const TeenyGPSEmulate&);
    TeenyGPSEmulate& operator=(const TeenyGPSEmulate&);

    // Setup
    bool init(HardwareSerial &serialPort_, uint32_t baudRate_);
    bool reset();

    // Methods for process incoming commands/requests from host
    void    tick(); // can do in ISR - rate depends on serial read hardware queue
    void    processIncomingPacket(); // do not call in ISR - uses serial read and write
    uint8_t getLostRxPacketCount();

    // Methods to access internal state
    uint32_t        getBaudRate();
    bool            getOutputUBX();
    uint16_t        getMeasurementRate();
    uint16_t        getNavigationRate();
    uint8_t         getAutoNAVPVTRate();
    uint8_t         getAutoNAVSATRate();

    // Methods for manual and auto NAVPVT packet transmission
    uint32_t        getNAVPVTTransmissionRate();
    bool            isNAVPVTPacketRequested();
    uint8_t         getLostNAVPVTRequestCount();
    void            setAutoNAVPVTRate(uint8_t rate=1); // Debug use only //
    bool            isAutoNAVPVTEnabled();
    bool            isNAVPVTPacket(const uint8_t *buf, size_t size);
    bool            setNAVPVTPacket(const uint8_t *buf, size_t size);
    void            setNAVPVTColdPacket();
    ubxNAVPVTInfo_t getNAVPVTPacketInfo();
    void            setNAVPVTPacketDateTime(uint16_t year, uint8_t month, uint8_t day,
                                            uint8_t hour, uint8_t minute, uint8_t second);
    void            unsetNAVPVTPacketDateValidFlag();
    void            unsetNAVPVTPacketTimeValidFlag();
    void            unsetNAVPVTPacketLocationValidFlag();
    bool            sendNAVPVTPacket(); // do not call in ISR - uses serial write and sdcard

    // Methods for manual and auto NAVSAT packet transmission
    uint32_t        getNAVSATTransmissionRate();
    bool            isNAVSATPacketRequested();
    uint8_t         getLostNAVSATRequestCount();
    void            setAutoNAVSATRate(uint8_t rate=10); // Debug use only //
    bool            isAutoNAVSATEnabled();
    bool            isNAVSATPacket(const uint8_t *buf, size_t size);
    bool            setNAVSATPacket(const uint8_t *buf, size_t size);
    void            setNAVSATColdPacket();
    ubxNAVSATInfo_t getNAVSATPacketInfo();
    bool            sendNAVSATPacket(); // do not call in ISR - uses serial write and sdcard

    // Methods for setting cold and emulation loop output packets
    void            setEmuColdOutputPackets();
    bool            setEmuLoopOutputPackets();

    // PUBLIC FOR DEBUG - so we can display contents 
    ubxPacket_t       incomingPacket;
    ubxPacket_t       receivedPacket;
    ubxPacket_t       responsePacket;
    ubxPacket_t       acknowledgePacket;
    ubxPacket_t       unknownPacket;
    ubxNAVPVTPacket_t ubxNAVPVTPacket;
    ubxNAVPVTInfo_t   ubxNAVPVTInfo;
    ubxNAVSATPacket_t ubxNAVSATPacket;
    ubxNAVSATInfo_t   ubxNAVSATInfo;

  private:

    HardwareSerial *serialPort;
    emulatorSettings_t emulatorSettings;
    uint32_t baudRate;
    uint32_t requestedBaudRate;
    uint8_t  lostRxPacketCount;
    void     processIncomingByte(uint8_t incomingByte);
    void     buildAcknowledgePacket(uint8_t messageClass, uint8_t messageID, bool ack);
    void     sendPackets();
    void     sendPacket(ubxPacket_t *pkt);

    bool     requestNAVPVTPacket;
    uint8_t  lostNAVPVTRequestCount;
    bool     requestNAVSATPacket;
    uint8_t  lostNAVSATRequestCount;

    uint32_t loopPacketIndex;
    uint16_t getLoopPacketLength();

    void     calcChecksum(ubxPacket_t *pkt);
    void     calcChecksum(ubxNAVPVTPacket_t *pkt);
    void     calcChecksum(ubxNAVSATPacket_t *pkt);

};

#endif //TeenyGPSEmulate_h

