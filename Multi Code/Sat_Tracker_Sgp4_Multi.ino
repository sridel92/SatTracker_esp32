 /* ====================================================================
   Copyright (c) 2019 Thorsten Godau (dl9sec). All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
   3. Neither the name of the author(s) nor the names of any contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.
   THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR(S) OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
   SUCH DAMAGE.
   ====================================================================*/
//
//https://github.com/Hopperpop/Sgp4-Library/tree/master/examples
//
// Call up the SPIFFS FLASH filing system this is part of the ESP Core
#define FS_NO_GLOBALS
#include <FS.h>

// for the BMP files
// So I used image magick with the following settings to get the proper color bit depth and compression:
// magick convert -depth 24 -compress none -background black -alpha remove image.bmp new-image.bmp

#ifdef ESP32
  #include "SPIFFS.h"  // For ESP32 only
#endif

#include <WiFi.h>
#include <HTTPClient.h>
#include "time.h"
#include <TimeLib.h>
#include <WiFiUdp.h>
#include "settings.h" // Important: see settings.h to configure your settings!!!
#include <../TFT_eSPI_Setups/User_Setup_esp32.h>
time_t getNtpTime();
#include <Sgp4.h>
#include <Ticker.h>   //https://github.com/sstaub/Ticker

Sgp4 sat;
Ticker tkSecond;  // this lib create some task at schedule time (second...)

#include "Free_Fonts.h" // Include the header file attached to this sketch
#include <TFT_eSPI.h>         // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();    // Invoke custom library

// Include the header files that contain the icons
#include "Info.h"

int year_;
int mon_; 
int day_; 
int hr_; 
int min_; 
double sec_;
long        count    = 0;           // Loop count
double      dSatLAT  = 0;           // Satellite latitude
double      dSatLON  = 0;           // Satellite longitude
double      dSatAZ   = 0;           // Satellite azimuth
double      dSatEL   = 0;           // Satellite elevation
double      dSunLAT  = 0;           // Sun latitude
double      dSunLON  = 0;           // Sun longitude
double      dSunAZ   = 0;           // Sun azimuth
double      dSunEL   = 0;           // Sun elevation
char        acBuffer[20];           // Buffer for ASCII time
int         xpos      = 160;        // Position of watch
int         xposInit  = 0;
int         ypos = 200;             // Position of watch
int         ysecs = ypos;
int         xyfont = 4;
byte xcolon = 0, xsecs = 0;
uint32_t    targetTime = 0;                 // for next 1 second timeout
unsigned long unixtime = 1617906546;
//int timezone = 12 ;                       //utc + 12
int framerate;
static uint8_t conv2d(const char* p);       // Forward declaration needed for IDE 1.6.x

const int ledPin =  34;// the number of the LED pin
// Variables will change:
int ledState = LOW;             // ledState used to set the LED

unsigned int colour = 0;
const long Refresh = 1000*60*60*5;          // 5 hours = time to refresh screen (milliseconds)
const long Blink = 1000;                    // time to refresh screen (milliseconds)
unsigned long RefreshPreviousMillis = 0;           // will store last time screen was was refreshed
unsigned long BlinkPreviousMillis = 0;           // will store last time screen was was refreshed
unsigned long previousMillis = 0;        
unsigned long interval = 1000;


// *************************************************************
char TLE[500];       //Variable to store satellite TLEs.
//int numSats = 4;

String CelestrakUrl = "http://104.168.149.178";      //Web address to get TLE (CELESTRAK)

char TLENameChar[][21] =  { "sat0", "sat1", "sat2", "sat3", "sat3", "sat3" };
char TLE1Char[][71] =     { "sat0", "sat1", "sat2", "sat3", "sat3", "sat3" };
char TLE2Char[][71] =     { "sat0", "sat1", "sat2", "sat3", "sat3", "sat3" };

int Sat = 6;        //  Enter the number of tracked satellite
// Enter the code of the tracked satellites from Celestrak
char *SatTleURL[] = {
    "/satcat/tle.php?CATNR=25544",
    "/satcat/tle.php?CATNR=48209",
    "/satcat/tle.php?CATNR=28654",
    "/satcat/tle.php?CATNR=33591",
    "/satcat/tle.php?CATNR=43013",
    "/satcat/tle.php?CATNR=43560",
    };

/*
 * 25544 : ISS
 * 48209 : Crew dragon2
 * 42982 : KESTREL EYE KE2M
 * 28654 : NOAA 18 (B)
 * 33591 : NOAA 19
 * 43013 : NOAA 20
 * 11060 : TIROS N
 */

HTTPClient client;

String HourStr[10];

// Epochtime

const char* ntpServer = "pool.ntp.org";       // NTP server to request epoch time
unsigned long epochTime;                      // Variable to save current epoch time
unsigned long getTime() {                     // Function that gets current epoch time
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;

  //Ticker tickerObject(callbackFunction, 1000, 0, MILLIS)
  //tickerObject.start(); //start the ticker.
  Ticker tkSecond;  // this lib create some task at schedule time (second...)
}

void setup()
{
  Serial.begin(115200);
  
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield();        // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nSPIFFS initialised.");

  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);

  delay(50);
  tft.begin();
  tft.setRotation(1);                 // landscape
  
  tft.fillScreen(TFT_BLACK);

  // Slapsh screen
  drawBmp("/ISS_20years.bmp", 0, 0);  // 320x106
  delay(500);
  tft.setFreeFont(TT1);               // Select the orginal small TomThumb font
  tft.drawString("Satellite tracker", 20, 120, FONT2);
  delay(1000);
  
  tft.drawString("Connecting to my Wifi...", 20, 140, FONT2);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print("_>");
    tft.drawString("Connexion...", 20, 160, FONT2);
  }
  #ifdef DEBUG
  Serial.println("Connected to the WiFi ");
  #endif

  tft.drawString("Connexion success....", 20, 180, FONT2);
  delay(500);
  #ifdef DEBUG
    Serial.println("IP number assigned by DHCP is ");
    Serial.println(WiFi.localIP());
  #endif

  tft.fillRect(0,107, 320,240, TFT_BLACK);
  delay(500);
  tft.fillRect(0,0, 320,106, TFT_WHITE);
  delay(500);

  drawBmp("/Ntp.bmp", 20, 10); // 100x72 
  tft.drawString("I'm getting time from NTP server...", 10, 130, FONT2);
  
  configTime(0, 0, ntpServer);
  epochTime = getTime();

  for (int i = 0; i < 4; i++) {
    tft.drawNumber(epochTime, 10, 170, FONT4);  //ok
    epochTime = getTime();
    delay(1000);  
  }

  tft.drawString("This is the Unixtime", 100, 210, FONT2);
  delay(1000);

  tft.fillRect(0,107, 320,240, TFT_BLACK);
  delay(500);

  drawBmp("/Celestrak.bmp", 0, 0);
  delay(500);
  tft.drawString("Connecting to Celestrak", 20, 120, FONT2);
  delay(500);
  tft.drawString("Loocking for the lastest TLE", 20, 135, FONT2);

// Here we are going the download the TLE definition of each Satellite
for (int i = 0; i < Sat; i++) {
    #ifdef DEBUG
    Serial.println("");
    Serial.print("Main : Satellite number ");
    Serial.println(i);
    Serial.println("");
    Serial.println("Main ----------> go to GetTLE <------------------");
    #endif
    GetTLE(i);
    delay(100);
  }
  
  tft.drawString("TLE1 & 2 uplaoded...", 160, 210, FONT2);
  delay(500);
  tft.fillRect(0,0, 320,120, TFT_WHITE);
  delay(500);
  tft.fillRect(0,121, 320,240, TFT_BLACK);
  delay(500);

  drawBmp("/Nasa.bmp", 0, 122);     // 120x102
  delay(200);
  drawBmp("/Esa.bmp", 130, 125);    // 132x60
  delay(200);
  drawBmp("/CsaAsc.bmp", 0, 5);     // 110x106
  delay(200);
  drawBmp("/Jaxa.bmp", 120, 10);    // 120x77
  delay(200);
  tft.fillRect(0,0, 320,120, TFT_WHITE);
  delay(200);
  tft.fillRect(0,121, 320,240, TFT_BLACK);
  delay(200);
  drawBmp("/MissionAlpha.bmp", 20, 70);
  delay(1000);
  drawBmp("/Crew2.bmp", 0, 0);
  delay(2000);

  //the drawIcon function is not enabled but embedded on code
  //drawIcon(info, 10, 10, infoWidth, infoHeight);
  int i = 0;

  sat.site(dMyLAT, dMyLON  ,dMyALT); //set site latitude[°], longitude[°] and altitude[m]
  delay(100);
  
  //Display TLE epoch time
  double jdC = sat.satrec.jdsatepoch;
  invjday(jdC , timeZone, true, year_, mon_, day_, hr_, min_, sec_);
 
#ifdef DEBUG
  Serial.println("Epoch: " + String(day_) + '/' + String(mon_) + '/' + String(year_) + ' ' + String(hr_) + ':' + String(min_) + ':' + String(sec_));
#endif

  tkSecond.attach(1,Second_Tick);  
  targetTime = millis() + 1000;
  tft.fillScreen(TFT_BLACK);

  DrawMap();

} // EndSetup
// *************************

void loop()
{
  tft.setFreeFont(TT1);     // Select the orginal small TomThumb font
  // check if the time to refresh the screen is pass
  unsigned long currentMillis = millis();
  if (currentMillis - RefreshPreviousMillis >= Refresh) {
    DrawMap();
    RefreshPreviousMillis = currentMillis;
    delay(100);
  }

  epochTime = getTime();
  
  // sublopp for each satellite and get x, y
  for (int i = 0; i < Sat; i++) { 
    sat.init(TLENameChar[i],TLE1Char[i],TLE2Char[i]);     // initialize satellite parameters 
    sat.findsat(epochTime);
    float xx = ( ( sat.satLon + 180 ) *  320 ) / 360 ;    // Longitude is 360 wide but screen 320 pixels
    int xTft = xx;
    int yTft = (90 - sat.satLat);


    drawMarkerColor( xTft, yTft, i);    // i is the sat number
    // red dot when ISS is near my location (assuming that ISS is the first sat on the list.
    if (i == 0){

      if (  (sat.satLon > -10) && (sat.satLon < 10) && (sat.satLat > 45) && (sat.satLat < 50)) {
        // Sat O is above
        Serial.println(".....Alert.....");
        tft.fillCircle(290, 210, 20, TFT_RED);
        //blink led
        unsigned long currentMillis = millis();
        if ( currentMillis - BlinkPreviousMillis >= Blink ) {
          // save the last time you blinked the LED
          BlinkPreviousMillis = currentMillis;
          // if the LED is off turn it on and vice-versa:
          if (ledState == LOW) {
            ledState = HIGH;
          } else {
            ledState = LOW;
          }
          // set the LED with the ledState of the variable:
          digitalWrite(ledPin, ledState);
          }
      }
      else
      {
        tft.fillCircle(290, 210, 20, TFT_BLACK);
        //Serial.println("no alert.....");
      }
    }
    
    #ifdef DEBUG
      Serial.print("Sat : ");
      Serial.print(i);
      Serial.print(" -> Sat x : ");
      Serial.print(xTft);
      Serial.print(" - Sat y : ");
      Serial.println(yTft);

      Serial.print("Sat.Lat : ");
      Serial.print(sat.satLat);
      Serial.print(" - Sat.Lon : ");
      Serial.println(sat.satLon);
      Serial.println("-------------------------------------");
    #endif
  //delay(10);
  } // end of subloop


  DrawWatch();
  framerate += 1;
  // display tle informations

} // end of loop

// *******************************************************************
// This is the function to draw the icon stored as an array in program memory (FLASH)

// To speed up rendering we use a 64 pixel buffer
#define BUFF_SIZE 64
// Draw array "icon" of defined width and height at coordinate x,y
// Maximum icon size is 255x255 pixels to avoid integer overflow
void drawIcon(const unsigned short* icon, int16_t x, int16_t y, uint16_t width, uint16_t height) {

  uint16_t  pix_buffer[BUFF_SIZE];   // Pixel buffer (16 bits per pixel)

  // Set up a window the right size to stream pixels into
  tft.setWindow(x, y, x + width - 1, y + height - 1);

  // Work out the number whole buffers to send
  uint16_t nb = ((uint16_t)height * width) / BUFF_SIZE;

  // Fill and send "nb" buffers to TFT
  for (int i = 0; i < nb; i++) {
    for (int j = 0; j < BUFF_SIZE; j++) {
      pix_buffer[j] = pgm_read_word(&icon[i * BUFF_SIZE + j]);
    }
    tft.pushColors(pix_buffer, BUFF_SIZE);
  }

  // Work out number of pixels not yet sent
  uint16_t np = ((uint16_t)height * width) % BUFF_SIZE;

  // Send any partial buffer left over
  if (np) {
    for (int i = 0; i < np; i++) pix_buffer[i] = pgm_read_word(&icon[nb * BUFF_SIZE + i]);
    tft.pushColors(pix_buffer, np);
  }
}



// *******************************************************************
void connectWifi() {
  if (WiFi.status() == WL_CONNECTED) return;
  //Manual Wifi
  Serial.printf("Connecting to WiFi %s/%s", ssid);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.hostname(WIFI_HOSTNAME);
  WiFi.begin(ssid, password);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (i > 80) i = 0;
    drawProgress(i, "Connecting to WiFi '" + String(ssid) + "'");
    i += 10;
    Serial.print(".");
  }
  drawProgress(100, "Connected to WiFi '" + String(ssid) + "'");
  Serial.println("connected.");
}

// *******************************************************************
void printDigits(int digits)
{
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


// *******************************************************************
// Progress bar helper
void drawProgress(uint8_t percentage, String text) {
"xxx";
}


// *******************************************************************
/*-------- NTP code ----------*/
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// *******************************************************************
// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

// *******************************************************************
// Function to extract numbers from compile time string
static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

// *******************************************************************
void DrawWatch(){    
  tft.setFreeFont(FF8);                 // Select the font
  // Draw hours and minutes
  xposInit = xpos;
    if (hr_ < 10) xpos += tft.drawChar('0', xpos, ypos, xyfont);    // Add hours leading zero for 24 hr clock
      xpos += tft.drawNumber(hr_, xpos, ypos, xyfont);                // Draw hours
      xcolon = xpos;                                                  // Save colon coord for later to flash on/off later
      xpos += tft.drawChar(':', xpos, ypos, xyfont);                  // adding a char space for ':' in xpos
    if (min_ < 10) xpos += tft.drawChar('0', xpos, ypos, xyfont);   // Add minutes leading zero
      xpos += tft.drawNumber(min_, xpos, ypos, xyfont);               // Draw minutes
      xpos += tft.drawChar(':', xpos, ypos, xyfont);                  // adding a char space for ':' in xpos
      xsecs = xpos;                                                   // Save seconds 'x' position for later display updates
    if (sec_ < 10) xpos += tft.drawChar('0', xpos, ypos, xyfont);   // Add leading zero
      tft.drawNumber(sec_, xpos, ypos, xyfont);                       // Draw seconds
      xpos = xposInit;
}


// ******************************************************************
void DisplayInformation1() {
  tft.setFreeFont(FF1);                 // Select the font
  tft.drawString("Time",     210, 0, 2);
  tft.drawNumber(hr_,     260, 0, 2); 
  tft.drawNumber(min_,   280, 0, 2); 
  tft.drawNumber(sec_,   300, 0, 2);
  tft.drawString("millis()",      210, 15, 2);
  tft.drawNumber(millis(),        260, 15, 2);
  tft.drawNumber(previousMillis,  280, 30, 2);
}


// *******************************************************************
void Second_Tick() {
  unixtime += 1;      
  invjday(sat.satJd , timeZone,true, year_, mon_, day_, hr_, min_, sec_);
  #ifdef DEBUG
  Serial.println(String(day_) + '/' + String(mon_) + '/' + String(year_) + ' ' + String(hr_) + ':' + String(min_) + ':' + String(sec_));
  Serial.println("azimuth = " + String( sat.satAz) + " elevation = " + String(sat.satEl) + " distance = " + String(sat.satDist));
  Serial.println("latitude = " + String( sat.satLat) + " longitude = " + String( sat.satLon) + " altitude = " + String( sat.satAlt));
  switch(sat.satVis){
    case -2:
      Serial.println("Visible : Under horizon");
      //tft.setFreeFont(FF1);                 // Select the font
      //tft.drawString("Visible : Under horizon", 150, 210, 2);
    break;
    case -1:
      Serial.println("Visible : Daylight");
      //tft.setFreeFont(FF1);                 // Select the font
      //tft.drawString("Visible : Daylight", 150, 210, 2);
      break;
    default:
      Serial.println("Visible : " + String(sat.satVis));   //0:eclipsed - 1000:visible
      //tft.setFreeFont(FF1);                                // Select the font
      //tft.drawString("*** Visible ***", 200, 230, 2);
      //tft.drawFloat(sat.satVis, 190, 210, 2);
      break;
  }

  Serial.println("Framerate: " + String(framerate) + " calc/sec");
  Serial.println();
  #endif
  framerate=0;
}


// *******************************************************************
// There follows a crude way of flagging that this example sketch needs fonts which
// have not been enbabled in the User_Setup.h file inside the TFT_HX8357 library.
//
// These lines produce errors during compile time if settings in User_Setup are not correct
//
// The error will be "does not name a type" but ignore this and read the text between ''
// it will indicate which font or feature needs to be enabled
//
// Either delete all the following lines if you do not want warnings, or change the lines
// to suit your sketch modifications.

#ifndef LOAD_GLCD
//ERROR_Please_enable_LOAD_GLCD_in_User_Setup
#endif

#ifndef LOAD_GFXFF
ERROR_Please_enable_LOAD_GFXFF_in_User_Setup!
#endif
