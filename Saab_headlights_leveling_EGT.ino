#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "max6675.h"

// Display settings
const int screenWidth = 128; // OLED display width, in pixels
const int screenHeight = 64; // OLED display height, in pixels
const int oledReset = -1;
Adafruit_SSD1306 display(screenWidth, screenHeight, &Wire, oledReset);

// EGT sensor pins
#define SO_PIN 4
#define CS_PIN 3
#define SCK_PIN 2

const int egtLimit = 940;
const int egtRefreshInterval = 350;
unsigned long startMillisEgt;
unsigned long currentMillisEgt;

int egt = 0;
int maxEgt = 0;

MAX6675 thermocouple(SCK_PIN, CS_PIN, SO_PIN);

void setup()
{
  Serial.begin(115200);
  Serial.println("begin");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
}

void loop()
{
  RefreshEgt();

  delay(10);
}

void RefreshEgt()
{
  currentMillisEgt = millis();
  if (currentMillisEgt - startMillisEgt >= egtRefreshInterval)
  {
    startMillisEgt = currentMillisEgt;

    egt = thermocouple.readCelsius();

    if (egt > maxEgt) {
      maxEgt = egt;
    }

    drawHeader(maxEgt);

    drawBody(egt);

    if (egt > egtLimit) {
      showWarning();
    }
  }
}

void drawHeader(int maxValue) {

  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);

  display.setCursor(5, 0);
  display.print("EGT");

  display.setCursor(70, 0);
  display.print(maxValue);
  // temp symbol
  display.setTextSize(1);
  display.print((char)247);

  display.println();
  display.println();
}

void drawBody(int value) {

  // If the EGT has 4 symbols don't shopw the temp symbol
  if (value >= 1000) {
    display.setCursor(0, 23);
    display.setTextSize(5);
    display.print(value);
  }
  else {
    display.setCursor(15, 23);
    display.setTextSize(5);
    display.print(value);
    // temp symbol
    display.setTextSize(2);
    display.print((char)247);
  }

  display.display();
}

void showWarning(void) {

  display.invertDisplay(true);
  delay(150);
  display.invertDisplay(false);
}
