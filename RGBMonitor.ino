#include "FastLED.h"
#define NUM_LEDS 50
#define DATA_PIN 6
#define serialRate 115200
static const uint8_t prefix[] = {'A', 'd', 'a'};

// Define the array of leds
CRGB leds[NUM_LEDS];
int brightness = 255;
bool last;
bool up = 0;

void setup() {
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  Serial.begin(serialRate);
  Serial.print("Ada\n");
  pinMode(A0, INPUT_PULLUP);
    for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].r = 255;
    leds[i].g = 255;
    leds[i].b = 255;
  }
  FastLED.show();
}

void loop() {
  for (int i = 0; i < sizeof(prefix); ++i) {
    while (!Serial.available())brightnessControl();
    if (prefix[i] != Serial.read())
      return;
  }
  while (Serial.available() < 3);
  int highByte = Serial.read();
  int lowByte  = Serial.read();
  int checksum = Serial.read();
  if (checksum != (highByte ^ lowByte ^ 0x55)) {
    return;
  }

  uint16_t ledCount = ((highByte & 0x00FF) << 8 | (lowByte & 0x00FF) ) + 1;
  if (ledCount > NUM_LEDS) {
    ledCount = NUM_LEDS;
  }

  for (int i = 0; i < ledCount; i++) {
    while (Serial.available() < 3);
    leds[i].r = Serial.read();
    leds[i].g = Serial.read();
    leds[i].b = Serial.read();
  }
  FastLED.show();
}

void brightnessControl() {
  if (digitalRead(A0) == 0) {
    if (brightness == 0) {
      up = 1;
    } else if (brightness == 255) {
      up = 0;
    }
    while (digitalRead(A0) == 0) {
      if (up) {
        brightness++;
        if (brightness > 255) {
          brightness = 255;
          blinkLED(200);
        }
      } else {
        brightness--;
        if (brightness < 0) {
          brightness = 0;
          blinkLED(50);
        }
      }
      delay(10);
      FastLED.setBrightness(brightness);
      FastLED.show();
    }
  }
  return;
}

void blinkLED(int brightness) {
  for (int x = 0; x < 3; x++) {
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(100);
    FastLED.setBrightness(brightness/3);
    FastLED.show();
    delay(100);
  }
  return;
}
