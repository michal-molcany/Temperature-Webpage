#include <LiquidCrystal.h>
#include <DHT22.h>
#include <Wire.h>

#define DHT22_PIN 2
DHT22 myDHT22(DHT22_PIN);

float lastSampleHumid;

float h = 0;
float t = 0;
LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);

byte customChar[8] = {
  0b00100,
  0b01010,
  0b00100,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

void setup() {
  Wire.begin(9);                // join i2c bus with address #9
  Wire.onRequest(requestEvent); // register event
  Serial.begin(115200);
  Serial.println("DHT init");
  lcd.createChar(0, customChar);
  lcd.begin(16, 2);
  lcd.print("DHT init");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("Requesting data");
  delay(1500);
}

void loop() {
  DHT22_ERROR_t errorCode;
  errorCode = myDHT22.readData();
  switch (errorCode)
  {
    case DHT_ERROR_NONE:

      {
        float tempHum = myDHT22.getHumidity();
        t = myDHT22.getTemperatureC();

        if (tempHum > 55.1 && tempHum < 55.3)
          {
            Serial.println("Humidity sensor error.");
            break;
          }

        h = tempHum;
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
  lcd.setCursor(0, 0);
  lcd.print("T: ");
  lcd.print(t);
  lcd.write((uint8_t)0);
  lcd.print("C       ");
  lcd.setCursor(0, 1);
  lcd.print("H: ");
  lcd.print(h);
  lcd.print(" %       ");
  Serial.print("T: ");
  Serial.println(t);
  Serial.print("H: ");
  Serial.println(h);

  delay(2000);
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
