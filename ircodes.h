
//Protocol  : NEC                                                                                                                                                                                                                               
//Code      : 0x61A0F00F (32 Bits)                                                                                                                                                                                                              
// uint16_t rawData[71] = {8910, 4474,  536, 592,  484, 1746,  508, 1724,  508, 594,  508, 594,  510, 592,  510, 542,  560, 1722,  510, 1720,  510, 592,  510, 1720,  510, 564,  540, 588,  512, 564,  538, 540,  560, 540,  562, 1720,  512, 1716,  514, 1716,  514, 1692,  538, 590,  514, 560,  540, 562,  540, 590,  512, 536,  564, 562,  538, 540,  560, 566,  536, 1696,  536, 1668,  564, 1716,  514, 1692,  540, 43984,  8932, 2246,  532};  // NEC 61A0F00F                                                                                                                                                                                                    
// uint32_t address = 0x586;                                                                                                                                                                                                                     
// uint32_t command = 0xF;                                                                                                                                                                                                                       
const uint64_t SamsungPowerToggle = 0xE0E040BF;

// Let's make it change backlight and brightness

const uint64_t sMenu = 0xE0E058A7;
const uint64_t sDown = 0xE0E08679;
const uint64_t sRight = 0xE0E046B9;
const uint64_t sLeft = 0xE0E0A659;
const uint64_t sExit = 0xE0E0B44B;
const uint64_t sEnter = 0xE0E016E9;
const uint64_t sMute = 0xE0E0F00F;
const uint64_t sVolUp = 0xE0E0E01F;
const uint64_t sVolDown = 0xE0E0D02F;
const uint64_t sSource = 0xE0E0807F;

// Arrays (reduce then raise)
uint64_t nightLight[37] = {
  sMenu,
  sRight,
  sDown,
  sRight,

  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,

  // OK Backlight is UP

  sDown,
  sDown,

  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,
  sLeft,

  sExit
};


uint64_t dayLight[37] = {
  sMenu,
  sRight,
  sDown,
  sRight,

  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,

  // OK Backlight is UP

  sDown,
  sDown,

  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,
  sRight,

  sExit
};
