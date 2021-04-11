# SatTracker_esp32
a Sat tracker with ESP32

![Sat_esp32_Map.jpg](/Pict/Sat_esp32_Map.jpg)


ILI9341 Screen

library for TLE math 

https://github.com/Hopperpop/Sgp4-Library.git

Screen

https://github.com/Bodmer/TFT_eSPI.git

Connection have to be done following the lib instructions on setup.h file

// Parallel bus is only supported on ESP32
// Uncomment line below to use ESP32 Parallel interface instead of SPI

//#define ESP32_PARALLEL

// The ESP32 and TFT the pins used for testing are:
//#define TFT_CS   33  // Chip select control pin (library pulls permanently low
//#define TFT_DC   15  // Data Command control pin - use a pin in the range 0-31
//#define TFT_RST  32  // Reset pin, toggles on startup

//#define TFT_WR    4  // Write strobe control pin - use a pin in the range 0-31
//#define TFT_RD    2  // Read strobe control pin  - use a pin in the range 0-31

//#define TFT_D0   12  // Must use pins in the range 0-31 for the data bus
//#define TFT_D1   13  // so a single register write sets/clears all bits.
//#define TFT_D2   26  // Pins can be randomly assigned, this does not affect
//#define TFT_D3   25  // TFT screen update performance.
//#define TFT_D4   17
//#define TFT_D5   16
//#define TFT_D6   27
//#define TFT_D7   14


![Sat_esp32Vroom30pin.jpg](/Pict/Sat_esp32Vroom30pin.jpg)



Begin with provided example.

have Fun !

Stéphan
