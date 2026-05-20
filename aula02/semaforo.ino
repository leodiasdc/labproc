#include <Adafruit_NeoPixel.h>

#define PIN        8 
#define NUMPIXELS  1 
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin(); 
  pixels.setBrightness(50); 
}

void loop() {
  pixels.clear(); 
  pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  pixels.show();
  delay(4000);

  pixels.setPixelColor(0, pixels.Color(0, 255, 0));
  pixels.show();
  delay(3000);

  pixels.setPixelColor(0, pixels.Color(255, 255, 0));
  pixels.show();
  delay(1000);
}