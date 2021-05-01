
// *******************************************************************
// Draw a + mark centred on x,y
void drawMarker(int x, int y)
{
  tft.drawLine(x - 1, y, x + 1, y, TFT_BLUE);
  tft.drawLine(x, y - 1, x, y + 1, TFT_BLUE);
}

// *******************************************************************
// Draw a + mark centred on x,y
// i is the sat number and set the color of the mark
//
void drawMarkerColor(int x, int y, int i)
{
  int xd = 0;
  int yd = 185;
  
  switch (i) {
    case 0:
      tft.drawLine(x - 1, y, x + 1, y, TFT_BLUE);
      tft.drawLine(x, y - 1, x, y + 1, TFT_BLUE);
      tft.fillRect(0, yd, 25, 3, TFT_BLUE);            // fillRect(0, 0, _width, _height, color);
      tft.drawString(TLENameChar[i], 30, yd);
      tft.drawNumber(x, 90, yd);
      tft.drawNumber(y, 110, yd);
      break;
    
    case 1:
      tft.drawLine(x -1, y, x+1, y, TFT_RED);
      tft.drawLine(x, y-1, x, y+1, TFT_RED);
      tft.fillRect(0, yd+15, 25, 3, TFT_RED);
      tft.drawString(TLENameChar[i], 30, yd+15);
      tft.drawNumber(x, 90, yd+15);
      tft.drawNumber(y, 110, yd+15);
      break;
    
    case 2:
      tft.drawLine(x, y, x, y, TFT_YELLOW);
      tft.drawLine(x, y, x, y, TFT_YELLOW);
      tft.fillRect(0, yd+30, 25, 3, TFT_YELLOW);
      tft.drawString(TLENameChar[i], 30, yd+30);
      tft.drawNumber(x, 90, yd+30);
      tft.drawNumber(y, 110, yd+30);  
      break;
      
    case 3:
      tft.drawLine(x, y, x, y, TFT_ORANGE);
      tft.drawLine(x, y, x, y, TFT_ORANGE);
      tft.fillRect(0, yd+45, 25, 3, TFT_ORANGE);
      tft.drawString(TLENameChar[i], 30, yd+45);
      tft.drawNumber(x, 90, yd+45);
      tft.drawNumber(y, 110, yd+45);
      break;
      
    case 4:
      tft.drawLine(x, y, x, y, TFT_RED);
      tft.drawLine(x, y, x, y, TFT_RED);
    break;
    
    case 5:
      tft.drawLine(x, y, x, y, TFT_WHITE);
      tft.drawLine(x, y, x, y, TFT_WHITE);
    break;
    
    case 6:
      tft.drawLine(x, y, x, y, TFT_WHITE);
      tft.drawLine(x, y, x, y, TFT_WHITE);
    break;

    default:
    // if nothing else matches, do the default
    // default is optional
    break;
  }
}


// *******************************************************************
void DrawMap(){
  
  Serial.print("***refresh*********************");

  // map is 320 x 160 this is the main wolrd MAP on memory
  drawBmp("/Map4.bmp", 0, 0);

  // mark my position with a circle
  int xMyPos = 160+dMyLON; //
  int yMyPos = 90-dMyLAT; //
  //drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color),
  tft.drawCircle(xMyPos, yMyPos, 2, TFT_GREEN);
  tft.drawCircle(xMyPos, yMyPos, 6, TFT_GREEN);

  //test pointing some known aera on the map
  tft.drawCircle(160, 90, 4, TFT_RED);
  tft.drawCircle(160+19, 90+34, 4, TFT_RED); //magellan
  tft.drawCircle(160-67, 90+54, 4, TFT_RED); //horn

// Draw 2 lines for lat 0 longitude 0
  tft.drawLine(160, 0, 160, 180, TFT_RED);
  tft.drawLine(0, 90, 320, 90, TFT_RED);

}


// *******************************************************************
// ***** Mark a dot when ISS is above my head
//
void SatAlert()
{
//dMyLAT  =  48.91052;  // Latitude  - Asnières (France): N -> +, S -> -
//dMyLON  =   2.29100;  // Longitude - Asnières (France): E -> +, W -> -
//sat.satLon
//sat.satLat
/*
if (digitalRead(2) == HIGH && digitalRead(3) == HIGH) { // if BOTH the switches read HIGH
*/

//       47     <     50 + 5  et      47 >          50 - 5          
if ( sat.satLon < 50  )
    {
      //tft.drawCircle(200, 200, 20, TFT_YELLOW);
      Serial.println("yellow");
    }
    else
    {
      //tft.drawCircle(200, 200, 20, TFT_RED);
      Serial.println("red.....");
    }
}
