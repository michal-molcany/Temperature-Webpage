#include <Wire.h>
#include "SSD1306.h"
#include "SSD1306Ui.h"
#include "images.h"


SSD1306   display(0x3c, D14, D15);
SSD1306Ui ui     ( &display );
bool drawFrame1(SSD1306 *display, SSD1306UiState* state, int x, int y);
bool drawFrame2(SSD1306 *display, SSD1306UiState* state, int x, int y);


bool (*frames[])(SSD1306 *display, SSD1306UiState* state, int x, int y) = { drawFrame1, drawFrame2 };

int frameCount = 2;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Begin");
  int counter = 0;

  display.init();
  display.clear();
  display.display();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setContrast(255);

  display.drawString(64, 10, "Testing");
  display.drawXbm(34, 30, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
  display.display();
  delay(5000);

  ui.setTargetFPS(30);

  ui.setActiveSymbole(activeSymbole);
  ui.setInactiveSymbole(inactiveSymbole);

  ui.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  ui.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  ui.setFrames(frames, frameCount);

  // Inital UI takes care of initalising the display too.
  ui.init();

  display.flipScreenVertically();
}

void loop()
{
  int remainingTimeBudget = ui.update();

  if (remainingTimeBudget > 0)
  {
    delay(remainingTimeBudget);
  }
}

bool drawFrame1(SSD1306 *display, SSD1306UiState* state, int x, int y) {
  display->drawXbm(x, y, 37, 60, tempIcon);

  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_24);
  display->drawString(50 + x, 0 + y, "T: ");
  display->drawString(75 + x, 0 + y, "0.0");
  return false;
}

bool drawFrame2(SSD1306 *display, SSD1306UiState* state, int x, int y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_24);
  display->drawString(0 + x, 0 + y, "H:");
  display->drawString(25 + x, 0 + y, "0.0");
  return false;
}
