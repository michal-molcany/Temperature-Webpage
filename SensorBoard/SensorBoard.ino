#include <LiquidCrystal.h>

#include <DHT.h>
#include <Wire.h>

#define DHTPIN 2
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
float h = 0;
float t = 0;
LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);

void setup() {
  Wire.begin(9);                // join i2c bus with address #9
  Wire.onRequest(requestEvent); // register event
  Serial.begin(115200);
  Serial.println("DHTxx test!");
  lcd.begin(16, 2);
  dht.begin();
}

void loop() {
  h = dht.readHumidity();
  t = dht.readTemperature();

  lcd.clear();
  lcd.print("T: ");
  lcd.print(t);
  lcd.setCursor(0, 1);
  lcd.print("H: ");
  lcd.print(h);
  Serial.print("T: ");
  Serial.println(t);
  Serial.print("H: ");
  Serial.println(h);
  delay(2000);
}

void requestEvent()
{
  char data[2];
  data[0] = (char)t;
  data[1] = (char)h;
  Wire.write(data);
}
