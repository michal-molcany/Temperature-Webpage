#include "ClockSync.h"

ClockSync::ClockSync(String language, String country, String dateFormat, int numberOfTimeZones, String* timeZoneIds) {
  myLanguage = language;
  myCountry = country;
  myDateFormat = dateFormat;
  myNumberOfTimeZoneIds = numberOfTimeZones;
  myTimeZoneIds = timeZoneIds;
  timeZoneOffsetToUtcMillis = (long*) malloc(numberOfTimeZones * sizeof(long));
  dateString = (String*)malloc(numberOfTimeZones * sizeof(String));
}

void ClockSync::updateTime() {
  JsonStreamingParser parser;
  parser.setListener(this);
  WiFiClient client;

  // http://api.thingspeak.com/channels/CHANNEL_ID/feeds.json?results=2&api_key=API_KEY
  const char host[] = "oleddisplay.squix.ch";
  String url = F("/rest/time");

  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println(F("connection failed"));
    return;
  }

  Serial.print(F("Requesting URL: "));
  Serial.println(url);

  // {"language":"de","country":"CH","timeZoneIds":["Europe/Zurich", "Europe/London"],"dateFormat":"dd.MM.YYYY"}
  String timeZoneIdJson = "\"timeZoneIds\":[";
  for (int i = 0; i < myNumberOfTimeZoneIds; i++) {
    if (i > 0) {
      timeZoneIdJson += ",";
    }
    timeZoneIdJson += "\"" + myTimeZoneIds[i] + "\"";
  }
  timeZoneIdJson += "]";
  String request = "{\"language\":\""
                   + myLanguage + "\",\"country\":\""
                   + myCountry + "\","
                   + timeZoneIdJson + ",\"dateFormat\":\""
                   + myDateFormat + "\"}\r\n\r\n";
  Serial.println("Request: " + request);
  // This will send the request to the server
  client.print("POST " + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Length: " + String(request.length()) + "\r\n" +
               F("Connection: close\r\n\r\n"));

  client.println(request);

  while (!client.available()) {
    Serial.println(".");
    delay(1000);
  }

  int pos = 0;
  boolean isBody = false;
  char c;

  int size = 0;
  //  String tmpStr;
  client.setNoDelay(false);
  while (client.connected()) {
    while ((size = client.available()) > 0) {
      c = client.read();
      if (c == '{' || c == '[') {
        isBody = true;
      }
      if (isBody) {
        parser.parse(c);
      }
    }
  }
}


String ClockSync::getFormattedTime(int timeZoneIndex)
{
  return getHoursString(timeZoneIndex) + ":" + getMinutesString(timeZoneIndex) + ":" + getSecondsString(timeZoneIndex);
}

String ClockSync::getFormattedDate(int timeZoneIndex)
{
  return dateString[timeZoneIndex];
}


String ClockSync::getHoursString(int timeZoneIndex)
{
  if (millisOfDayAtUpdate == 0) {
    return "--";
  }
  int hours = ((getSecondsOfDay(timeZoneIndex)  % 86400L) / 3600) % 24;
  if (hours < 10) {
    return "0" + String(hours);
  }
  return String(hours); // print the hour (86400 equals secs per day)
}

int ClockSync::getHours(int timeZoneIndex)
{
  return ((getSecondsOfDay(timeZoneIndex)  % 86400L) / 3600) % 24;
}

String ClockSync::getMinutesString(int timeZoneIndex) 
{
  if (millisOfDayAtUpdate == 0) {
    return "--";
  }
  int minutes = ((getSecondsOfDay(timeZoneIndex) % 3600) / 60);
  if (minutes < 10 ) {
    // In the first 10 minutes of each hour, we'll want a leading '0'
    return "0" + String(minutes);
  }
  return String(minutes);
}

int ClockSync::getMinutes(int timeZoneIndex)
{
  return ((getSecondsOfDay(timeZoneIndex) % 3600) / 60);
}

String ClockSync::getSecondsString(int timeZoneIndex) 
{
  if (millisOfDayAtUpdate == 0) {
    return "--";
  }
  int seconds = getSecondsOfDay(timeZoneIndex) % 60;
  if ( seconds < 10 ) {
    // In the first 10 seconds of each minute, we'll want a leading '0'
    return "0" + String(seconds);
  }
  return String(seconds);

}

int ClockSync::getSeconds(int timeZoneIndex)
{
  return getSecondsOfDay(timeZoneIndex) % 60;
}

long ClockSync::getSecondsOfDay(int timeZoneIndex) 
{
  return (millisOfDayAtUpdate + millis() - localMillisAtUpdate + timeZoneOffsetToUtcMillis[timeZoneIndex]) / 1000;
}

tmElements_t ClockSync::getDateTime(int timeZoneIndex)
{
  tmElements_t tm;
  int Day, Month, Year;
  String tmpStr = getFormattedDate(timeZoneIndex);
  Day =  tmpStr.substring(0, 2).toInt();
  Month = tmpStr.substring(3, 5).toInt();
  Year = CalendarYrToTm(tmpStr.substring(6).toInt());

  tm.Hour = getHours(timeZoneIndex);
  tm.Minute = getMinutes(timeZoneIndex);
  tm.Second = getSeconds(timeZoneIndex);
  tm.Day =  Day;
  tm.Month = Month;
  tm.Year = Year;
  Serial.print("Setted date: ");
  Serial.println(tmpStr);
  return tm;
}

void ClockSync::whitespace(char c) 
{
}

void ClockSync::startDocument() 
{
}

void ClockSync::key(String key) 
{
  currentKey = key;
}

void ClockSync::value(String value)
{
  Serial.println(currentKey + ": " + value);
  if (currentKey == "millisOfDayUtc")
  {
    millisOfDayAtUpdate = value.toInt();
    localMillisAtUpdate = millis();
  }
  else if (currentKey == "index")
  {
    currentTimeZoneIndex = value.toInt();
    Serial.println("\n-->Current index: " + String(currentTimeZoneIndex));
  }
  else if (currentKey == "timeZoneOffsetToUtcMillis")
  {
    Serial.println("\n-->Index: " + String(currentTimeZoneIndex));
    Serial.println("\n-->value: " + value);
    timeZoneOffsetToUtcMillis[currentTimeZoneIndex] = value.toInt();
  }
  else if (currentKey == "formattedDate")
  {
    Serial.println("\n-->Date: " + value);
    dateString[currentTimeZoneIndex] = value;
  }
}

void ClockSync::endArray()
{
}

void ClockSync::endObject() 
{
}

void ClockSync::endDocument() 
{
}

void ClockSync::startArray() 
{
}

void ClockSync::startObject() 
{
}
