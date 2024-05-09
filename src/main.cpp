#include <Arduino.h>
#include "Mouse.h" 
#include "Keyboard.h"
#include "Adafruit_LIS3DH.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_LIS3DH lis = Adafruit_LIS3DH();
const int CLICK = 8;
const int CLEAR = 7;
unsigned long clickStamp = 0;
unsigned long clearStamp = 0;
const int MODEL_NUM = 16;
const char* models[MODEL_NUM] = {
  "bee", "octopus", "spider", "the_mona_lisa", "garden", "cat", "rabbit", "radio",
  "rhinoceros", "strawberry", "tiger", "rifle", "mosquito", "bear", "eye", "frogsofa"
};

int prevModelIndex = 0;

void setup(void) {
  Serial.begin(9600);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("LIS3DH test!");

  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1) yield();
  }
  Serial.println("LIS3DH found!");

  lis.setRange(LIS3DH_RANGE_2_G);   // 2, 4, 8 or 16 G!

  Serial.print("Range = "); Serial.print(2 << lis.getRange());
  Serial.println("G");

  // lis.setDataRate(LIS3DH_DATARATE_50_HZ);
  Serial.print("Data rate set to: ");
  switch (lis.getDataRate()) {
    case LIS3DH_DATARATE_1_HZ: Serial.println("1 Hz"); break;
    case LIS3DH_DATARATE_10_HZ: Serial.println("10 Hz"); break;
    case LIS3DH_DATARATE_25_HZ: Serial.println("25 Hz"); break;
    case LIS3DH_DATARATE_50_HZ: Serial.println("50 Hz"); break;
    case LIS3DH_DATARATE_100_HZ: Serial.println("100 Hz"); break;
    case LIS3DH_DATARATE_200_HZ: Serial.println("200 Hz"); break;
    case LIS3DH_DATARATE_400_HZ: Serial.println("400 Hz"); break;

    case LIS3DH_DATARATE_POWERDOWN: Serial.println("Powered Down"); break;
    case LIS3DH_DATARATE_LOWPOWER_5KHZ: Serial.println("5 Khz Low Power"); break;
    case LIS3DH_DATARATE_LOWPOWER_1K6HZ: Serial.println("16 Khz Low Power"); break;
  }

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  display.clearDisplay();
  delay(200);
  int16_t x1, y1;
	uint16_t w, h;
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE, BLACK);
  display.getTextBounds(models[prevModelIndex], 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2);
  display.print(models[prevModelIndex]);
  display.display();
  Mouse.begin();
}

void loop() {
  
  lis.read();

  sensors_event_t event;
  lis.getEvent(&event);
  Mouse.move(event.acceleration.x, -event.acceleration.y, 0);

  // choose model
  int modelIndex = map(analogRead(A0), 0, 1023, 0, MODEL_NUM-1);

  if (digitalRead(CLICK) == HIGH && millis() - clickStamp >= 200) {

    if (!Mouse.isPressed()) {
      Mouse.press();
    } else {
      Mouse.release();
    }
    clickStamp = millis();

    
  }
  
  if (digitalRead(CLEAR) == HIGH && millis() - clearStamp >= 200) {
    Serial.println("clear");
    clearStamp = millis();
  }

  //display selected model
  int16_t x1, y1;
	uint16_t w, h;
  
  if (modelIndex != prevModelIndex) {
    // change in model
    display.clearDisplay();
    display.getTextBounds(models[modelIndex], 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2);
    display.print(models[modelIndex]);

    display.display();

    // send model data to serial
    Serial.println(models[modelIndex]);
    prevModelIndex = modelIndex;
  }
  
  // buggy code

}

