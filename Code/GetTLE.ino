
void GetTLE() {
// connecting to Cletrak for getting TwoLine element (TLE)
// HTTP request

HTTPClient http;
http.begin("http://104.168.149.178/satcat/tle.php?CATNR=25544");
int httpCode = http.GET();

if (httpCode > 0) { //Check for the returning code
  String payload = http.getString();
  
  //Serial.println(httpCode);
  //Serial.println(payload); 

  //myString.substring(from, to)
  //TLE line 1 spans characters 26 - 96
  //TLE line 2 spans characters 97 - 167

  String MyTLEName = payload.substring(0,20);
  String MyTLE1 = payload.substring(26,96);
  String MyTLE2 = payload.substring(97,167);

  // myString.toCharArray(buf, len)
  MyTLEName.toCharArray(satname,20);
  MyTLE1.toCharArray(tle_line1,70);
  MyTLE2.toCharArray(tle_line2,70);

  Serial.println("array..."); 
  Serial.println(satname);
  Serial.println(tle_line1); 
  Serial.println(tle_line2);
  

  }else {
      Serial.println("Error on HTTP request");
  }

http.end(); //Free the resources
}
