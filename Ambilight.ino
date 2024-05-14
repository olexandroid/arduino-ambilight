// see https://alexgyver.ru/arduino_ambilight/
// fixed blinks and lags
// deleted goto
// small code improvements

#define NUM_LEDS 112
#define DI_PIN 13
#define OFF_TIME 10
#define CURRENT_LIMIT 1000
#define START_FLASHES 0
#define serialRate 115200
#include <FastLED.h>

int new_bright, new_bright_f;
unsigned long bright_timer, off_timer;
uint8_t prefix[] = {'A', 'd', 'a'}, hi, lo, chk, i;
boolean led_state = true;
CRGB leds[NUM_LEDS];

void setup(){
  FastLED.addLeds<WS2812, DI_PIN, GRB>(leds, NUM_LEDS);
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);

  if (START_FLASHES) {
    LEDS.showColor(CRGB(55, 0, 0));
    delay(500);
    LEDS.showColor(CRGB(0, 55, 0));
    delay(500);
    LEDS.showColor(CRGB(0, 0, 55));
    delay(500);
    LEDS.showColor(CRGB(0, 0, 0));
  }

  Serial.begin(serialRate);
  Serial.print("Ada\n");
}

void check_connection() {
  if (led_state) {
    if (millis() - off_timer > (OFF_TIME * 1000)) {
      led_state = false;
      FastLED.clear();
      FastLED.show();
    }
  }
}

byte read(){
  while (!Serial.available()) 
    check_connection();
  return Serial.read();
}

void loop() {
  if (!led_state) led_state = true;
  off_timer = millis();  
  for (i = 0; i < sizeof prefix; ++i) {
    if (prefix[i] != read()) return;
  }
  hi = read();
  lo = read();
  chk = read();
  if (chk == (hi ^ lo ^ 0x55)){
    memset(leds, 0, NUM_LEDS * sizeof(struct CRGB));
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i].r = read();
      leds[i].g = read();
      leds[i].b = read();
    }
    FastLED.show();
    // fix blinks and lag
    while (Serial.available()) Serial.read();
  }
}