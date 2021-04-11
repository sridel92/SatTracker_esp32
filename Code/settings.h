/*
 *
 *
 *
*/

// ******************** //
// ***** Setup    ***** //
// ******************** //

#include <Arduino.h>
#include "TZ.h"
#define WIFI_HOSTNAME "SRID-weather-station-color"
#define DEBUG                 //Enable serial output.

const char* ssid = "PierreJoigneaux124";       //  your network SSID (name)
const char* password = "LaPetiteCabane123";        // your network password

///////////////#define WIFI_HOSTNAME "SRID-weather-station-color"
// NTP Servers:
static const char ntpServerName[] = "pool.ntp.org";

const int timeZone = 2;     // Central European Time

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

#define MAP_MAXX   320
#define MAP_MAXY   240

const char  *pcMyName = "OriginalStefRid";    // Observer name

// Gps coordinate of observer
double       dMyLAT   =  48.91052;  // Latitude  - Asnières (France): N -> +, S -> -
double       dMyLON   =   2.29100;  // Longitude - Asnières (France): E -> +, W -> -
double       dMyALT   =   20.0;       // Altitude ASL (m)

/***************************
 * End Settings
 **************************/
