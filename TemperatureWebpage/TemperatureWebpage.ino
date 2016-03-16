#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>

#include <SPI.h>
#include <SD.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "ClockSync.h"
#include "webpage.h"

//WiFi credentials
const char* ssid = "****";
const char* password = "****";

String timeZoneIds [] = {"America/New_York", "Europe/London", "Europe/Paris", "Australia/Sydney"};
ClockSync clockSync("en", "EN", "dd.MM.yyyy", 4, timeZoneIds);
const int CurrentTimezone = 2;

const int MaxNumberOfLines = 64;

File myFile;
byte lastHour = 0;
String fileName = "th.csv";
bool isSDcard = false;

ESP8266WebServer server(80);

String getDateTime()
{
  String content = String(year());
  content += "-";
  byte d = month();
  if (d < 10)
  {
    content += "0";
  }
  content += d;
  content += "-";
  d = day();
  if (d < 10)
  {
    content += "0";
  }
  content += d;
  content += " ";

  d = hour();
  if (d < 10)
  {
    content += "0";
  }
  content += d;
  content += ":";
  d = minute();
  if (d < 10)
  {
    content += "0";
  }
  content += d;
  content += ":";
  d = second();
  if (d < 10)
  {
    content += "0";
  }
  content += d;
  return content;
}

bool InitalizeSDcard()
{
  Serial.print("Initializing SD card...");

  if (!SD.begin(SS)) {
    Serial.println("initialization failed!");
    isSDcard = false;

  }
  else
  {
    Serial.println("initialization done.");
    isSDcard = true;
  }
  return isSDcard;
}

void timeUpdate()
{
  tmElements_t tm;
  clockSync.updateTime();
  delay(1000);
  tm = clockSync.getDateTime(CurrentTimezone);
  RTC.write(tm);
}

String readLastData()
{
  String rows;
  myFile = SD.open(fileName);
  int i, numberOfLines = 0;
  if (myFile)
  {
    // read from the file until there's nothing else in it:
    while (myFile.available())
    {
      myFile.readStringUntil('\n');
      numberOfLines++;
    }

    // close the file:
    myFile.close();

    myFile = SD.open(fileName);
    while (myFile.available())
    {
      if (numberOfLines - MaxNumberOfLines >= i)
      {
        myFile.readStringUntil('\n');
      }
      else
      {
        rows += myFile.readStringUntil('\n');
      }
      i++;
    }
    myFile.close();
  }
  return rows;
}
void handleRawData()
{
  String content = RawDataDataPageHeader;

  content += TableHeader;
  content += readLastData();
  content += "</table></body></html>";

  server.send(200, "text/html", content);
}

void handleMeasuredData()
{
  String rows;
  String content = MeasuredDataPageHeader;

  content += TableHeader;
  content += readLastData();
  content += "</table></body></html>";

  server.send(200, "text/html", content);
}

//root page can be accessed only if authentification is ok
void handleRoot() {
  Serial.println("Enter handleRoot");
  //  Serial.println(h);
  long rssi = WiFi.RSSI();

  tmElements_t tm;
  RTC.read(tm);

  String header;

  String content = PageHeader;
  if (server.hasHeader("User-Agent")) {
    content += "<table><tr><td><b>The user agent used is: </b></td><td>" + server.header("User-Agent") + "</td></tr><tr><td> </td></tr>";
  }

  Wire.requestFrom(9, 4);    // request 4 bytes from slave device #9
  String i2cResult;
  while (Wire.available())
  {
    i2cResult += Wire.read();
  }

  setSyncProvider(RTC.get);
  content += "<tr><td><b>Wifi strenght:</b></td><td>";
  content += rssi;
  content += " dB</td></tr><tr><td><b>Temperature senzor:</b></td><td>";
  content += i2cResult.substring(0, 2);
  content += ".";
  content += i2cResult.substring(2, 3);
  content += " C</td></tr>";
  content += "<tr><td><b>Humidity senzor:</b></td><td>";
  content += i2cResult.substring(3, 5);
  content += ".";
  content += i2cResult.substring(5,6);
  content += " %</td></tr>";
  content += "<tr><td><b>Time:</b></td><td>";
  content += getDateTime();
  content += "</td></tr></table>";
  if (isSDcard)
    content += "<br><p>SD card is initialized. <a href=\"/measuredData\">Measured data</a>";
  content += "</p></body></html>";
  server.send(200, "text/html", content);
}

//no need authentification
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  Serial.begin(115200);
  Serial.println();
  Wire.begin(8);                // join i2c bus with address #8
  Serial.println("SENSOR ACTIVE");
  delay(50);

  Serial.print("Initializing SD card...");
  String ssidString = "";
  String passwordString = "";
  if (InitalizeSDcard())
  {
    bool isSsid = true;

    myFile = SD.open("wifi");

    if (myFile)
    {
      while (myFile.available())
      {
        char currentChar = (char)myFile.read();

        if (currentChar == '\n')
        {
          isSsid = false;
        }
        else
        {
          if (isSsid)
          {
            ssidString += currentChar;
          }
          else
          {
            passwordString += currentChar;
          }
        }
      }
      char s[ssidString.length()];
      char p[passwordString.length()];
      ssidString.toCharArray(s, ssidString.length());
      passwordString.toCharArray(p, passwordString.length() + 1);

      myFile.close();
      WiFi.begin(s, p);
    }
    else
    {
      Serial.println("Wifi file not found");
      ssidString = ssid;
      WiFi.begin(ssid, password);
    }
  }
  else
  {
    Serial.println("Wifi file not found");
    ssidString = ssid;
    WiFi.begin(ssid, password);
  }

  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssidString);

  byte i = 0;
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if (i > 30)
    {
      Serial.println();
      Serial.println("Connection failed");
      return;
    }
    i++;
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(ssidString);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  timeUpdate();

  server.on("/", handleRoot);
  server.on("/measuredData", handleMeasuredData);
  server.on("/rawData", handleRawData);

  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );
  server.begin();
  Serial.println("HTTP server started");
  Serial.println(getDateTime());
}

void loop(void) {
  server.handleClient();
  setSyncProvider(RTC.get);
  byte hours = hour();
  if (minute() == 45 && hours != lastHour)
  {
    lastHour = hours;
    if (!isSDcard)
    {
      if (!InitalizeSDcard())
        return;
    }

    myFile = SD.open(fileName, FILE_WRITE);

    if (myFile)
    {
      Wire.requestFrom(9, 10);    // request 2 bytes from slave device #8
      String i2cResult;
      while (Wire.available())
      { // slave may send less than requested
        i2cResult += Wire.read();
      }
      String content = "<tr><td>";
      content += getDateTime();
      content += "</td><td>";
      content += i2cResult.substring(0, 2);
      content += ".";
      content += i2cResult.substring(2, 3);
      content += "</td><td>";
      content += i2cResult.substring(3, 5);
      content += ".";
      content += i2cResult.substring(5,6);
      content += "</td></tr>";
      myFile.println(content);
      myFile.close();
      Serial.println(content);
    }
    else
    {
      // if the file didn't open, print an error:
      Serial.println("error opening file");
    }
  }
}

