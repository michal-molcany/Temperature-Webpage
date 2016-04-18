#include <DHT22.h>
#include <Wire.h>

#define DHT22_PIN 2
DHT22 myDHT22(DHT22_PIN);
void readSensorData();

float lastSampleHumid;

float h = 0;
float t = 0;


void setup() {
  Wire.begin(9);                // join i2c bus with address #9
  Wire.onRequest(requestEvent); // register event
  Serial.begin(9600);
  Serial.println("DHT init");

  delay(2000);
}

void loop()
{
  readSensorData();
  delay(2000);
}

void readSensorData()
{
  DHT22_ERROR_t errorCode;
  errorCode = myDHT22.readData();
  switch (errorCode)
  {
    case DHT_ERROR_NONE:
      {
        float tempHum = myDHT22.getHumidity();
        t = myDHT22.getTemperatureC();

        if (tempHum <= 55.1 || tempHum >= 55.3)
        {
          h = tempHum;
        }
        else
        {
          Serial.println("Humidity sensor error.");
        }
      }
      break;
    case DHT_ERROR_CHECKSUM:
      Serial.println("Checksum error.");
      break;
    case DHT_BUS_HUNG:
      Serial.println("BUS Hung ");
      break;
    case DHT_ERROR_NOT_PRESENT:
      Serial.println("Not Present ");
      break;
    case DHT_ERROR_ACK_TOO_LONG:
      Serial.println("ACK time out ");
      break;
    case DHT_ERROR_SYNC_TIMEOUT:
      Serial.println("Sync Timeout ");
      break;
    case DHT_ERROR_DATA_TIMEOUT:
      Serial.println("Data Timeout ");
      break;
    case DHT_ERROR_TOOQUICK:
      Serial.println("Polled to quick ");
      break;
  }
  Serial.print("T: ");
  Serial.println(t);
  Serial.print("H: ");
  Serial.println(h);
}

void requestEvent()
{
  char data[4];
  data[0] = (char)t;
  data[1] = (t - (char)t) * 10;
  data[2] = (char)h;
  data[3] = (h - (char)h) * 10;
  Wire.write(data);
}
