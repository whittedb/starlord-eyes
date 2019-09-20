#include <Arduino.h>
#include <util/atomic.h>
#include <FastLED.h>
#include "MyTimer.h"


constexpr auto LED_CNT = 12;
constexpr auto DRIVER_PIN = 4;
constexpr auto ON_OFF_PIN = 2;
constexpr auto FADE_ON_DELAY = 10;
constexpr auto FADE_ON_STEP = 1;
constexpr auto FADE_OFF_DELAY = 20;
constexpr auto FADE_OFF_STEP = 3;
constexpr auto MAX_BRIGHTNESS = 200;

enum State
{
  S_IDLE,
  S_OFF,
  S_ON,
  S_TURN_ON,
  S_TURN_OFF,
  S_FADE_ON,
  S_WAIT_FADE_ON,
  S_FADE_OFF,
  S_WAIT_FADE_OFF,
};

volatile State state = S_IDLE;
//Adafruit_NeoPixel eyes(LED_CNT, DRIVER_PIN, NEO_GRBW);
CRGB leds[LED_CNT];
int16_t currentBrightness = 0;
MyTimer timer = MyTimer();

void debounceButton();

void setup()
{
  Serial.begin(9600);
  delay(2000);  // Wait for NeoPixel Ring to initialize

  FastLED.addLeds<NEOPIXEL, DRIVER_PIN>(leds, LED_CNT);
  FastLED.clear(true);
  for (int i = 0; i < LED_CNT; ++i) {
    leds[i].setRGB(255, 0, 0);
  }
  /*
    eyes.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
    eyes.show();             // Turn OFF all pixels ASAP
    eyes.setBrightness(128); // Set BRIGHTNESS (max = 255)
  */
  pinMode(ON_OFF_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ON_OFF_PIN), debounceButton, FALLING);
}

void loop() {
  State s;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { s = state; }
  switch (s)
  {
    case S_IDLE:
      break;

    case S_OFF:
      //Serial.println("OFF");
      break;

    case S_ON:
      //Serial.println("ON");
      break;

    case S_TURN_ON:
      Serial.println("TURN_ON");
      currentBrightness = 0;
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { state = S_FADE_ON; }
      break;

    case S_TURN_OFF:
      Serial.println("TURN_OFF");
      currentBrightness = MAX_BRIGHTNESS;
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { state = S_FADE_OFF; }
      break;

    case S_FADE_ON:
      FastLED.show(currentBrightness);
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { state = S_WAIT_FADE_ON; }
      timer.start(FADE_ON_DELAY);
      break;

    case S_WAIT_FADE_ON:
      if (timer.expired()) {
        if (currentBrightness < MAX_BRIGHTNESS) {
          currentBrightness += FADE_ON_STEP;
          if (currentBrightness > MAX_BRIGHTNESS) {
            currentBrightness = MAX_BRIGHTNESS;
          }
          ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { state = S_FADE_ON; }
        } else {
          ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { state = S_ON; }
        }
      }
      break;

    case S_FADE_OFF:
      FastLED.show(currentBrightness);
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { state = S_WAIT_FADE_OFF; }
      timer.start(FADE_OFF_DELAY);
      break;

    case S_WAIT_FADE_OFF:
      if (timer.expired()) {
        if (currentBrightness > 0) {
          currentBrightness -= FADE_OFF_STEP;
          if (currentBrightness < 0) {
            currentBrightness = 0;
          }
          ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { state = S_FADE_OFF; }
        } else {
          ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { state = S_OFF; }
        }
      }
      break;

    default:
      break;
  }
}

void debounceButton()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) {
    if (state == S_ON || state == S_IDLE) {
      state = S_TURN_OFF;
    } else if (state == S_OFF) {
      state = S_TURN_ON;
    }
    
  }
  last_interrupt_time = interrupt_time;
}
