#include <JsonListener.h>
#include <JsonStreamingParser.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Time.h>

class ClockSync: public JsonListener {
  private:
    long millisOfDayAtUpdate = 0;
    long localMillisAtUpdate;
    boolean isHeader = true;
    String currentKey = "";
    String myLanguage;
    String myCountry;
    String* myTimeZoneIds;
    int myNumberOfTimeZoneIds;
    String myDateFormat;

    int currentTimeZoneIndex;
    String* dateString;
    long* timeZoneOffsetToUtcMillis;
    //char* dateString[10];

  public:
    ClockSync(String language, String country, String dateFormat, int numberOfTimeZones, String* timeZoneIds);

    void updateTime();

    String getFormattedTime(int timeZoneIndex);

    String getFormattedDate(int timeZoneIndex);

    String getHoursString(int timeZoneIndex);

    String getMinutesString(int timeZoneIndex);

    String getSecondsString(int timeZoneIndex);

    int getHours(int timeZoneIndex);

    int getMinutes(int timeZoneIndex);

    int getSeconds(int timeZoneIndex);

    long getSecondsOfDay(int timeZoneIndex);

    tmElements_t getDateTime(int timeZoneIndex);

    virtual void whitespace(char c);

    virtual void startDocument();

    virtual void key(String key);

    virtual void value(String value);

    virtual void endArray();

    virtual void endObject();

    virtual void endDocument();

    virtual void startArray();

    virtual void startObject();
};
