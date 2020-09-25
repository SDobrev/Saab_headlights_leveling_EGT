#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "max6675.h"

// Headlights
int mosfetTriggerPin = 9;
int reqestedPosition = 0;
int oppositePosition = 0;
//For mosfet usage it is required to revert the HIGH and the LOW.
uint8_t HIGH_O = LOW;
uint8_t LOW_O = HIGH;

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

  pinMode(mosfetTriggerPin, OUTPUT);
  pinMode(A0, INPUT);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
}

void loop()
{
  getRequestedHeadlightPosition();
  refreshEgt();

  delay(20);
}

void transmitionStart()
{
  digitalWrite(mosfetTriggerPin, HIGH_O);
  delay(2);
  digitalWrite(mosfetTriggerPin, LOW_O);
  delay(5);
  digitalWrite(mosfetTriggerPin, HIGH_O);
  delayMicroseconds(1000);

}

void transmitionData()
{
  digitalWrite(mosfetTriggerPin, HIGH_O);
  delayMicroseconds(reqestedPosition);
  digitalWrite(mosfetTriggerPin, LOW_O);
  delayMicroseconds(oppositePosition);
}

void transmitionEnd()
{
  digitalWrite(mosfetTriggerPin, LOW_O);
  delayMicroseconds(1000);
  digitalWrite(mosfetTriggerPin, HIGH_O);
  delayMicroseconds(1000);
  digitalWrite(mosfetTriggerPin, LOW_O);
  delayMicroseconds(500);
  digitalWrite(mosfetTriggerPin, HIGH_O);
  delayMicroseconds(500);
  digitalWrite(mosfetTriggerPin, LOW_O);
  delay(4);
}

void getRequestedHeadlightPosition()
{
  reqestedPosition = analogRead(A0)* 2.92;
  oppositePosition = 3000 - reqestedPosition;
  
  if (reqestedPosition <= 0)
  {
    reqestedPosition = 0;
  }
  
  if (oppositePosition <= 0)
  {
    oppositePosition = 0;
  }
  
  Serial.println(reqestedPosition);
  Serial.println(oppositePosition);
  
  transmitionStart();
  transmitionData();
  transmitionEnd();
}

void refreshEgt()
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

  // TODO: zumer
}
