# SatTracker_esp32
a Sat tracker with ESP32

note on 2021 05 01 : I have uploaded the "multi" code able to follow many sat with colored traces on screen

note on 2021 06 18 : a complete V2 version is on another repo (Wiimanager and ST7789v screen)



![Sat_esp32_Map.jpg](/Pict/Sat_esp32_Map.jpg)


ILI9341 Screen

library for TLE math 

https://github.com/Hopperpop/Sgp4-Library.git

Screen

https://github.com/Bodmer/TFT_eSPI.git

Connection have to be done following the lib instructions on setup.h file

My setup.h is :

- #if defined(ESP32)
- // For ESP32 Dev board (only tested with ILI9341 display)
- // The hardware SPI can be mapped to any pins
- #define TFT_MISO 19
- #define TFT_MOSI 23
- #define TFT_SCLK 18
- #define TFT_CS   15  // Chip select control pin
- #define TFT_DC   2   // Data Command control pin
- #define TFT_RST  4   // Reset pin (could connect to RST pin)


![Sat_esp32Vroom30pin.jpg](/Pict/Sat_esp32Vroom30pin.jpg)

Pin board
![tft_ILI9341_with_touch.jpg](/Pict/tft_ILI9341_with_touch.jpg)


Note : all images (BMP format) are a bit touchy to convert to be compatible with the libray

they are all uploaded in SFPIFFS memory and you need ESP add-on for this

You can find more about this here : https://randomnerdtutorials.com/esp32-web-server-spiffs-spi-flash-file-system


Begin with provided example.

have Fun !

Stéphan
