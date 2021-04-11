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
time_t getNtpTime();
#include <Sgp4.h>
#include <Ticker.h> //https://github.com/sstaub/Ticker

Sgp4 sat;
Ticker tkSecond;  // this lib create some task at schedule time (second...)

//Ticker tickerObject(callbackFunction, 1000); 
//tickerObject.start(); //start the ticker.



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
double      dfreqRX  = 145.800;     // Nominal downlink frequency
double      dfreqTX  = 437.800;     // Nominal uplink frequency
int         iYear    = 2021;  // Set start year
int         iMonth   = 4;     // Set start month
int         iDay     = 2;     // Set start day
int         iHour    = 20;    // Set start hour
int         iMinute  = 00;    // Set start minute
int         iSecond  = 00;    // Set start second
double      dSatLAT  = 0;           // Satellite latitude
double      dSatLON  = 0;           // Satellite longitude
double      dSatAZ   = 0;           // Satellite azimuth
double      dSatEL   = 0;           // Satellite elevation
double      dSunLAT  = 0;           // Sun latitude
double      dSunLON  = 0;           // Sun longitude
double      dSunAZ   = 0;           // Sun azimuth
double      dSunEL   = 0;           // Sun elevation
int         ixQTH    = 0;           // Map pixel coordinate x of QTH
int         iyQTH    = 0;           // Map pixel coordinate y of QTH
int         ixSAT    = 0;           // Map pixel coordinate x of satellite
int         iySAT    = 0;           // Map pixel coordinate y of satellite
int         ixSUN    = 0;           // Map pixel coordinate x of sun
int         iySUN    = 0;           // Map pixel coordinate y of sun
char        acBuffer[20];           // Buffer for ASCII time
int         aiSatFP[32][2];         // Array for storing the satellite footprint map coordinates
int         aiSunFP[32][2];         // Array for storing the sunlight footprint map coordinates
int         xpos = 0;
int         xposInit = 0;
int         ypos = 200; // Top left corner ot clock text, about half way down
int         ysecs = ypos;
int         xyfont = 2;
byte xcolon = 0, xsecs = 0;
uint32_t    targetTime = 0;        // for next 1 second timeout
unsigned long unixtime = 1617906546;
//int timezone = 12 ;  //utc + 12
int framerate;
static uint8_t conv2d(const char* p); // Forward declaration needed for IDE 1.6.x
String MyTLEName = "xxxxxxxxxx";
String MyTLE1    = "xxxxxxxxxxxxxxxxxxxxxxx";
String MyTLE2    = "yyyyyyyyyyyyyyyyyyyyyyy";
//byte omm = 99, oss = 99;

unsigned int colour = 0;
unsigned long previousMillis=0 ;
unsigned long interval = 1000;

// *************************************************************
char TLE[500];       //Variable to store satellite TLEs.
int numSats = 4;
int SAT = 0;

char satname[] = "ISS (ZARYA)";
char tle_line1[] = "1 25544U 98067A   21097.40647234  .00001690  00000-0  39002-4 0  9993";  //Line one from the TLE data
char tle_line2[] = "2 25544  51.6464 332.8116 0002933 191.9452 346.3978 15.48867738277621";  //Line two from the TLE data
  
char satnames[4][30] = {"ISS (ZARYA)",
                        "NEOSSAT",
                        "M3MSAT ",
                        "SCISAT"};// Names of satellites.
char satURL[4][30] = {"/satcat/tle.php?CATNR=25544",
                      "/satcat/tle.php?CATNR=39089",
                      "/satcat/tle.php?CATNR=41605",
                      "/satcat/tle.php?CATNR=27858"}; // URL of Celestrak TLEs for satellites (In same order as names).
char TLE1[4][70]; 
char TLE2[4][70];
char server[] = "104.168.149.178";    //Web address to get TLE (CELESTRAK)

HTTPClient client;

String HourStr[10];

// Epochtime
// NTP server to request epoch time
const char* ntpServer = "pool.ntp.org";

// Variable to save current epoch time
unsigned long epochTime; 

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}


void setup()
{
  Serial.begin(115200);
  
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nSPIFFS initialised.");

  delay(250);
  tft.begin();
  tft.setRotation(1);  // landscape
  tft.fillScreen(TFT_BLACK);

  // Slapsh screen
  drawBmp("/ISS_20years.bmp", 0, 0); // 320x106
  delay(500);
  tft.setFreeFont(TT1);     // Select the orginal small TomThumb font
  tft.drawString("ISS tracker", 20, 120, FONT2);

  delay(2000);
  
  tft.drawString("Connecting to my Wifi...", 20, 140, FONT2);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("_>");
    tft.drawString("Connexion...", 20, 160, FONT2);
  }
  Serial.println("Connected to the WiFi ");
  tft.drawString("Connexion success....", 20, 180, FONT2);
  delay(2000);
  #ifdef DEBUG
    Serial.print("IP number assigned by DHCP is ");
    Serial.println(WiFi.localIP());
  #endif
  //delay(100);

  tft.fillRect(0,107, 320,240, TFT_BLACK);
  delay(500);
  tft.fillRect(0,0, 320,106, TFT_WHITE);
  delay(500);

  drawBmp("/Ntp.bmp", 20, 10); // 100x72 
  tft.drawString("I'm getting time from NTP server...", 10, 130, FONT2);
  
  // printing epochTime
  // (how many second from Jan 1st, 1970 )
  // this is usefull for calculating the Sat track
  
  configTime(0, 0, ntpServer);
  epochTime = getTime();
  
  for (int i = 0; i < 5; i++) {
  tft.drawNumber(epochTime, 10, 170, FONT4);  //ok
  epochTime = getTime();
  delay(1000);  
  }

  tft.drawString("This is the Unixtime", 100, 210, FONT2);

  delay(2000);

  tft.fillRect(0,107, 320,240, TFT_BLACK);
  delay(500);

  drawBmp("/Celestrak.bmp", 0, 0);
  delay(500);
  tft.drawString("Connecting to Celestrak", 20, 140, FONT2);
  delay(500);
  tft.drawString("Loocking for the lastest TLE", 20, 160, FONT2);

  GetTLE();
  //TLE1 and 2 received
  tft.drawString("TLE1 and TLE2 uplaoded...", 20, 180, FONT2);
  delay(1000);

  tft.fillRect(0,0, 320,120, TFT_WHITE);
  delay(500);
  tft.fillRect(0,121, 320,240, TFT_BLACK);
  delay(500);


  drawBmp("/Nasa.bmp", 0, 122);  // 120x102
  delay(500);

  drawBmp("/Esa.bmp", 130, 125);   // 132x60
  delay(500);

  drawBmp("/CsaAsc.bmp", 0, 5);   // 110x106
  delay(500);

  drawBmp("/Jaxa.bmp", 120, 10);    // 120x77
  delay(500);

  tft.fillRect(0,0, 320,120, TFT_WHITE);
  delay(500);
  tft.fillRect(0,121, 320,240, TFT_BLACK);
  delay(200);

  drawBmp("/MissionAlpha.bmp", 20, 70);
  delay(2000);

  drawBmp("/Crew2.bmp", 0, 0);
  delay(2000);
  
  drawIcon(info, 10, 10, infoWidth, infoHeight);
  int i = 0;
  sat.site(dMyLAT, dMyLON  ,dMyALT); //set site latitude[°], longitude[°] and altitude[m]

delay(2000);

  sat.init(satname,tle_line1,tle_line2);     //initialize satellite parameters 
  
  //Display TLE epoch time
  double jdC = sat.satrec.jdsatepoch;
  invjday(jdC , timeZone, true, year_, mon_, day_, hr_, min_, sec_);
  Serial.println("Epoch: " + String(day_) + '/' + String(mon_) + '/' + String(year_) + ' ' + String(hr_) + ':' + String(min_) + ':' + String(sec_));
  Serial.println();

  tkSecond.attach(1,Second_Tick);
  
  Serial.println("go");
  //Serial.setDebugOutput(false);   // Comment out for UNO

  targetTime = millis() + 1000;
    
  tft.fillScreen(TFT_BLACK);

  // map is 320 x 160
  drawBmp("/Map4.bmp", 0, 0);


} // EndSetup
// *************************



void loop()
{

  epochTime = getTime();
// we use Ticker lib so we can't use delay function.

  if( millis() - previousMillis >= interval) {
    #ifdef DEBUG
      Serial.println("---------------------> millis > 1000 ------");
    #endif
    previousMillis = millis();
    DrawWatch();
    Serial.print("Epoch Time: ");
    Serial.println(epochTime);
    }

    DisplayInformation1();

    sat.findsat(epochTime);

framerate += 1;

// disply tle informations
tft.setFreeFont(TT1);     // Select the orginal small TomThumb font
tft.drawString(satname, 40, 173, GFXFF);
tft.drawString(tle_line1, 40, 183, GFXFF);
tft.drawString(tle_line2, 40, 193, GFXFF);


//epoch

tft.drawNumber(epochTime, 0, 205, FONT2);  //ok
tft.drawString("Where is the ISS ?", 0, 225, FONT2);
tft.drawString("Lat :", 150, 225, FONT2);
tft.drawNumber(sat.satLat, 190, 225, FONT2);  //ok
tft.drawString("Long :", 225, 225, FONT2);
tft.drawNumber(sat.satLon, 270, 225, FONT2);

// x, y center on lat 0, Long 0
//
// screen is 320 x 240 pixels
// map is 320 x 160
// center of map is at x = 160, y = 80
// Longitude calculated is from -180 to 180 ( sat.satLon ) 
// Latitude  calculated is from -90 to 90   ( sat.satLat )
//
// Drawing coef
float xx = ( ( sat.satLon + 180 ) *  320 ) / 360 ;
int xTft = xx;       //  ( ( sat.satLon + 180 ) * 320 ) / 360  ; //
//  int xTft = sat.satLon + 180; //
  int yTft = (90 - sat.satLat); // (220/180)); //ok
// draw SAT marker on screen (one dot drawing a line)
  drawMarker( xTft, yTft);
  
// draw my location marker on screen (one dot drawing a line)
  //drawMarker( (dMyLON + 180 * (320/360)) + 160, (dMyLAT + 90 * (220/180)) + 120 );

// mark my position with a circle
  int xMyPos = 160+dMyLON; //
  int yMyPos = 90-dMyLAT; //

//drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color),
  tft.drawCircle(xMyPos, yMyPos, 2, TFT_GREEN);
  tft.drawCircle(xMyPos, yMyPos, 6, TFT_GREEN);

//test
  tft.drawCircle(160, 90, 4, TFT_RED);
  tft.drawCircle(160+19, 90+34, 4, TFT_RED); //magellan
  tft.drawCircle(160-67, 90+54, 4, TFT_RED); //horn
  


// Draw 2 lines for lat 0 longitude 0
  tft.drawLine(160, 0, 160, 240, TFT_RED);
  tft.drawLine(0, 90, 320, 90, TFT_RED);

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
// Draw a + mark centred on x,y
void drawMarker(int x, int y)
{
  tft.drawLine(x - 1, y, x + 1, y, TFT_BLUE);
  tft.drawLine(x, y - 1, x, y + 1, TFT_BLUE);
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
      // Update digital time
      tft.setFreeFont(FF18);                // Select the font
      tft.setFreeFont(FF1);                 // Select the font
      // Draw hours and minutes
      xposInit = xpos;
      if (hour() < 10) xpos += tft.drawChar('0', xpos, ypos, xyfont);   // Add hours leading zero for 24 hr clock
      xpos += tft.drawNumber(hour(), xpos, ypos, xyfont);               // Draw hours
      xcolon = xpos;                                                    // Save colon coord for later to flash on/off later
      xpos += tft.drawChar(':', xpos, ypos, xyfont);                    // adding a char space for ':' in xpos
      if (minute() < 10) xpos += tft.drawChar('0', xpos, ypos, xyfont); // Add minutes leading zero
      xpos += tft.drawNumber(minute(), xpos, ypos, xyfont);             // Draw minutes
      xpos += tft.drawChar(':', xpos, ypos, xyfont);                    // adding a char space for ':' in xpos
      xsecs = xpos;                                                     // Save seconds 'x' position for later display updates
      if (second() < 10) xpos += tft.drawChar('0', xpos, ypos, xyfont); // Add leading zero
      tft.drawNumber(second(), xpos, ypos, xyfont);                     // Draw seconds
      xpos = xposInit;
}





// ******************************************************************
void DisplayInformation1()
{
//tft.setFreeFont(FF18);                // Select the font
tft.setFreeFont(FF1);                 // Select the font
tft.drawString("Time",     210, 0, 2);
tft.drawNumber(hour(),     260, 0, 2); 
tft.drawNumber(minute(),   280, 0, 2); 
tft.drawNumber(second(),   300, 0, 2);

tft.drawString("millis()",      210, 15, 2);
tft.drawNumber(millis(),        260, 15, 2);
tft.drawNumber(previousMillis,  280, 30, 2);
}



// *******************************************************************
void Second_Tick()
{
  unixtime += 1;
       
  invjday(sat.satJd , timeZone,true, year_, mon_, day_, hr_, min_, sec_);
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
                  //tft.setFreeFont(FF1);                 // Select the font
                  //tft.drawString("Visible :", 150, 210, 2);
                  //tft.drawString(String(sat.satVis), 190, 210, 2);
                  break;
          }

  Serial.println("Framerate: " + String(framerate) + " calc/sec");
  Serial.println();
     
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
