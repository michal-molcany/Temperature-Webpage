#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>
#include <SPI.h>
#include <SD.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "SSD1306.h"
#include "SSD1306Ui.h"
#include "ClockSync.h"
#include "webpage.h"
#include "images.h"


//WiFi credentials
const char* ssid = "****";
const char* password = "****";


//Variables
const String url = "/rest/data/";
const char* host = "eskclimate.azurewebsites.net";
const String SerialNumber = "1";
File myFile;
File logFile;
byte lastHour = 0;
byte lastSecond = lastSecond;
String timeZoneIds [] = {"America/New_York", "Europe/London", "Europe/Paris", "Australia/Sydney"};
const int CurrentTimezone = 2;
const int MaxNumberOfLines = 50;
String fileName = "th.csv";
String logFileName = "log.txt";
bool isSDcard = false;
String temperature;
String humidity;


//OLED display init
SSD1306   display(0x3c, D3, D4);
SSD1306Ui ui     ( &display );
int frameCount = 2;


//Functions
void readI2C();
void handleLogData();
void handleNotFound();
void handleRoot();
void handleMeasuredData();
void handleRawData();
String readLastData();
void writeLogEntry(const String data);
String getDate();
String getTime();
String getDateTime();
bool InitalizeSDcard();
void timeUpdate();
void writeDataOnServer();

bool drawFrame1(SSD1306 *display, SSD1306UiState* state, int x, int y);
bool drawFrame2(SSD1306 *display, SSD1306UiState* state, int x, int y);
bool (*frames[])(SSD1306 *display, SSD1306UiState* state, int x, int y) = { drawFrame1, drawFrame2 };
