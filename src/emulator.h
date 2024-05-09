
/********************************************************************/
// EMULATOR
bool emulatorEnabled;
#include "TeenyGPSEmulate.h"
TeenyGPSEmulate emulator;
uint8_t emulatorColdStartPacketCount = 0;
bool emulatorLoopEnabled = false;

/********************************************************************/
bool emulator_setup(HardwareSerial &serialPort, uint32_t baudRate, tgpse_ubx_module_type_t ubxModuleType) {
  if(emulator.init(serialPort, baudRate, ubxModuleType)) {
    emulatorEnabled = true;
    emulatorColdStartPacketCount = 0;
    emulatorLoopEnabled = false;
    // TBD - Open optional SD card emulation loop file
  } else {
    emulatorEnabled = false;
  }
  return emulatorEnabled;
}

/********************************************************************/
void emulator_end() {
  // TBD - Close optional SD card emulation loop file
}

