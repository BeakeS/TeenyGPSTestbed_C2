/*
TeenyUbloxConnect.h - Header file for the TeenyUbloxConnect Arduino Library.
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

#ifndef TeenyUbloxConnect_h
#define TeenyUbloxConnect_h

/********************************************************************/
// UBX Packet Frame Defines
/********************************************************************/
const uint8_t  COM_PORT_UART1 = 1;
const uint8_t  COM_TYPE_UBX = 1;
const uint16_t UBX_MAXPAYLOADLENGTH = 872; // NAV-SAT message with 72 satellites
const uint8_t  UBX_SYNCH_1 = 0xB5;
const uint8_t  UBX_SYNCH_2 = 0x62;
const uint8_t  UBX_CLASS_NAV = 0x01;
const uint8_t    UBX_NAV_PVT   = 0x07;
const uint16_t   UBX_NAV_PVT_PAYLOADLENGTH = 92;
const uint16_t   UBX_NAV_PVT_PACKETLENGTH = 100;
const uint8_t    UBX_NAV_SAT = 0x35;
const uint16_t   UBX_NAV_SAT_MINPAYLOADLENGTH = 8;
const uint16_t   UBX_NAV_SAT_MAXPAYLOADLENGTH = UBX_MAXPAYLOADLENGTH;
const uint16_t   UBX_NAV_SAT_MAXPACKETLENGTH = UBX_MAXPAYLOADLENGTH + 8;
const uint8_t  UBX_CLASS_ACK = 0x05;
const uint8_t    UBX_ACK_NAK   = 0x00;
const uint8_t    UBX_ACK_ACK   = 0x01;
const uint16_t   UBX_ACKNAK_PAYLOADLENGTH = 2;
const uint8_t  UBX_CLASS_CFG = 0x06;
const uint8_t    UBX_CFG_PRT   = 0x00;
const uint16_t   UBX_CFG_PRT_PAYLOADLENGTH = 20;
const uint8_t    UBX_CFG_MSG   = 0x01;
const uint8_t    UBX_CFG_RST   = 0x04;
const uint16_t   UBX_CFG_RST_PAYLOADLENGTH = 4;
const uint8_t    UBX_CFG_RATE  = 0x08;
const uint16_t   UBX_CFG_RATE_PAYLOADLENGTH = 6;
const uint8_t    UBX_CFG_CFG   = 0x09;
const uint8_t    UBX_CFG_NAVX5 = 0x23;
const uint16_t   UBX_CFG_NAVX5_PAYLOADLENGTH = 40;
const uint8_t  UBX_CLASS_MON = 0x0A;
const uint8_t    UBX_MON_VER   = 0x04;
const uint16_t   UBX_MON_VER_PAYLOADLENGTH = 160;

/********************************************************************/
// UBX Packet Struct
/********************************************************************/
typedef struct {
  uint8_t  synch1 = UBX_SYNCH_1;
  uint8_t  synch2 = UBX_SYNCH_2;
  uint8_t  messageClass;
  uint8_t  messageID;
  uint16_t payloadLength;
  uint8_t  pad00a;
  uint8_t  pad00b;
  uint8_t  payload[UBX_MAXPAYLOADLENGTH];
  uint8_t  checksumA;
  uint8_t  checksumB;
  uint8_t  rollingChecksumA;
  uint8_t  rollingChecksumB;
  uint16_t packetCounter;
  uint16_t payloadCounter;
  bool     receivingPacket;
  bool     validPacket;
  uint8_t  pad03a;
  uint8_t  pad03b;
} ubloxPacket_t;

/********************************************************************/
// UBX ACK/NAK Packet Struct
/********************************************************************/
typedef struct {
  uint8_t  synch1 = UBX_SYNCH_1;
  uint8_t  synch2 = UBX_SYNCH_2;
  uint8_t  UBX_CLASS_ACK;
  uint8_t  messageID;
  uint16_t payloadLength = UBX_ACKNAK_PAYLOADLENGTH;
  uint8_t  pad00a;
  uint8_t  pad00b;
  uint8_t  payload[UBX_ACKNAK_PAYLOADLENGTH];
  uint8_t  pad01a;
  uint8_t  pad01b;
  uint8_t  checksumA;
  uint8_t  checksumB;
  bool     validPacket;
  uint8_t  pad02a;
} ubloxACKNAKPacket_t;

/********************************************************************/
// UBX-NAV-PVT Info Struct
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
} ubloxNAVPVTInfo_t;

/********************************************************************/
// UBX-NAV-SAT Info Struct
/********************************************************************/
typedef struct {
  uint8_t  gnssId;
  char     gnssIdType;
  uint8_t  svId;
  uint8_t  cno;
  int16_t  azim;
  int16_t  prRes;
  int8_t   elev;
  bool     healthy;
  bool     ephValid;
  bool     svUsed;
} ubloxNAVSATSVInfo_t;
/********************************************************************/
typedef struct {
  uint8_t  numSvs;
  uint8_t  numSvsHealthy;
  uint8_t  numSvsEphValid;
  uint8_t  numSvsHealthyAndEphValid;
  uint8_t  numSvsUsed;
  uint8_t  pad00a;
  uint8_t  pad00b;
  uint8_t  pad00c;
  ubloxNAVSATSVInfo_t svSortList[32];
} ubloxNAVSATInfo_t;

/********************************************************************/
// UBX-CFG-RST Payloads
/********************************************************************/
const uint8_t UBX_CFG_RST_COLDSTART_PAYLOAD[UBX_CFG_RST_PAYLOADLENGTH] = {
  0xFF,0xFF,0x01,0x00
};
const uint8_t UBX_CFG_RST_WARMSTART_PAYLOAD[UBX_CFG_RST_PAYLOADLENGTH] = {
  0x01,0x00,0x01,0x00
};
const uint8_t UBX_CFG_RST_HOTSTART_PAYLOAD[UBX_CFG_RST_PAYLOADLENGTH] = {
  0x00,0x00,0x01,0x00
};

/********************************************************************/
// TeenyUbloxConnect Class
/********************************************************************/
class TeenyUbloxConnect {

  public:

    // Constructor / destructor / disallow copy and move
    TeenyUbloxConnect();
    virtual ~TeenyUbloxConnect();
    TeenyUbloxConnect(const TeenyUbloxConnect&);
    TeenyUbloxConnect& operator=(const TeenyUbloxConnect&);

// A default of 1100ms for maxWait Serial seems reasonable givin auto packet processing
#ifndef defaultMaxWait // Let's allow the user to define their own value if they want to
#define defaultMaxWait 1100
#endif

    // Ublox setup
    bool    begin(Stream &serialPort_, uint16_t maxWait_ = defaultMaxWait);

    // Ublox command methods
    bool    pollUART1Port(uint16_t maxWait_ = defaultMaxWait);
    void    setSerialRate(uint32_t baudrate_, uint8_t uartPort_ = COM_PORT_UART1, uint16_t maxWait_ = defaultMaxWait);
    void    coldStart();
    void    warmStart();
    void    hotStart();
    bool    saveConfiguration(uint16_t maxWait_ = defaultMaxWait);
    bool    getProtocolVersion(uint16_t maxWait_ = defaultMaxWait);
    uint8_t getProtocolVersionHigh(uint16_t maxWait_ = defaultMaxWait);
    uint8_t getProtocolVersionLow(uint16_t maxWait_ = defaultMaxWait);
    bool    setPortOutput(uint8_t portID_, uint8_t comSettings_, uint16_t maxWait_ = defaultMaxWait);
    bool    setMeasurementRate(uint16_t rate_, uint16_t maxWait_ = defaultMaxWait);
    bool    setNavigationRate(uint16_t rate_, uint16_t maxWait_ = defaultMaxWait);
    bool    setAutoNAVPVT(bool enable_, uint16_t maxWait_ = defaultMaxWait);
    bool    setAutoNAVPVTRate(uint8_t rate_, uint16_t maxWait_ = defaultMaxWait);
    bool    setAutoPVT(bool enable_, uint16_t maxWait_ = defaultMaxWait); // Same as setAutoNAVPVT
    bool    setAutoPVTRate(uint8_t rate_, uint16_t maxWait_ = defaultMaxWait); // Same as setAutoNAVPVTRate
    bool    setAutoNAVSAT(bool enable_, uint16_t maxWait_ = defaultMaxWait);
    bool    setAutoNAVSATRate(uint8_t rate_, uint16_t maxWait_ = defaultMaxWait);

    // Host methods for process incoming responses/acknowledges from ublox receiver
    // can do in ISR - depends on serial read hardware queue
    void    checkUblox();

    // Get the latest Position/Velocity/Time solution and fill all global variables
    // Returns true when a packet has been received
    bool    getNAVPVT(); // Use only when autoNAVPVTRate > 0
    bool    getPVT();    // Same as getNAVPVT() (Kept for SparkFun compatability)
    bool    pollNAVPVT(uint16_t maxWait_ = defaultMaxWait); // Use only when autoPVTRate = 0

    // Get the latest satellite information
    //Returns true when a packet has been received
    bool    getNAVSAT(); // Use only when autoNAVSATRate > 0
    bool    pollNAVSAT(uint16_t maxWait_ = defaultMaxWait); // Use only when autoNAVSATRate = 0

    // Ublox navpvt data access
    void     getNAVPVTPacket(uint8_t *packet_); // Get the full NAV-PVT packet
    uint16_t getYear();
    uint8_t  getMonth();
    uint8_t  getDay();
    uint8_t  getHour();
    uint8_t  getMinute();
    uint8_t  getSecond();
    bool     getDateValid();
    bool     getTimeValid();
    uint32_t getTimeAccEst();
    uint8_t  getFixType();
    bool     getGnssFixOk();
    uint8_t  getSIV();
    int32_t  getLongitude();
    int32_t  getLatitude();
    int32_t  getAltitudeMSL();
    uint32_t getHorizontalAccEst();
    uint32_t getVerticalAccEst();
    int32_t  getHeading();
    uint16_t getPDOP();

    // Ublox navsat data access
    void     getNAVSATPacket(ubloxPacket_t &packet_); // Get the full NAV-SAT packet
    void     getNAVSATInfo(ubloxNAVSATInfo_t &info_); // summary and sorted sat details

    // Access lost packet counts
    uint8_t  getLostRxPacketCount();
    uint8_t  getLostNAVPVTPacketCount();
    uint8_t  getLostNAVSATPacketCount();

  private:
    Stream   *serialPort;

    ubloxPacket_t       commandPacket;
    ubloxPacket_t       incomingPacket;
    ubloxPacket_t       receivedPacket;
    ubloxPacket_t       responsePacket;
    ubloxACKNAKPacket_t acknowledgePacket;
    ubloxPacket_t       ubloxNAVPVTPacketBuffer;
    uint8_t             ubloxNAVPVTPacket[UBX_NAV_PVT_PACKETLENGTH];
    ubloxNAVPVTInfo_t   ubloxNAVPVTInfo;
    ubloxPacket_t       ubloxNAVSATPacketBuffer;
    ubloxPacket_t       ubloxNAVSATPacket;
    ubloxNAVSATInfo_t   ubloxNAVSATInfo;

    void     calcCommandPacketChecksum();
    bool     sendCommandPacket(bool expectResp_, bool expectAck_, uint16_t maxWait_);
    volatile bool processingUbloxCommand;
    void     checkUbloxInternal();
    void     processIncomingByte(uint8_t incomingByte_);
    // Do not call processIncomingPacket() in ISR - uses serial.write
    void     processIncomingPacket(uint8_t requestedClass_ = 0, uint8_t requestedID_ = 0);
    uint8_t  lostRxPacketCount;
    uint8_t  protocolVersionHigh;
    uint8_t  protocolVersionLow;
    uint8_t  lostNAVPVTPacketCount;
    bool     processNAVPVTPacket();
    void     setNAVPVTPacketInfo();
    uint8_t  lostNAVSATPacketCount;
    bool     processNAVSATPacket();
    void     setNAVSATPacketInfo();

};

#endif //TeenyUbloxConnect_h

