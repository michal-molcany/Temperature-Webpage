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


//Variables
const String url = "/rest/data/";
const char* host = "eskclimate.azurewebsites.net";
const String SerialNumber = "1";
File myFile;
File logFile;
byte lastHour = 0;
String timeZoneIds [] = {"America/New_York", "Europe/London", "Europe/Paris", "Australia/Sydney"};
const int CurrentTimezone = 2;
const int MaxNumberOfLines = 64;
String fileName = "th.csv";
String logFileName = "log.txt";
bool isSDcard = false;
String temperature;
String humidity;


//Functions 
void readI2C();
void handleLogData();
void handleNotFound();
void handleRoot();
void handleMeasuredData();
void handleRawData();
String readLastData();
void writeLogEntry(const String data);
String getDateTime();
bool InitalizeSDcard();
void timeUpdate();
void writeDataOnServer();
