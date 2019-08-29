#include <Arduino.h>
#include <util/atomic.h>
#include <Adafruit_NeoPixel.h>


constexpr auto LED_CNT = 24;
constexpr auto DRIVER_PIN = 4;
constexpr auto ON_OFF_PIN = 2;

enum State {
  S_OFF,
  S_ON,
  S_TURN_ON,
  S_TURN_OFF
};

volatile State state = S_OFF;
Adafruit_NeoPixel eyes(LED_CNT, DRIVER_PIN, NEO_GRBW);

void debounceButton();

void setup()
{
  eyes.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
  eyes.show();             // Turn OFF all pixels ASAP
  eyes.setBrightness(128); // Set BRIGHTNESS (max = 255)

  pinMode(ON_OFF_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ON_OFF_PIN), debounceButton, FALLING);
}

void loop() {
  switch (state)
  {
    case S_OFF:
    case S_ON:
      break;

    case S_TURN_ON:
      eyes.fill(eyes.Color(255, 0, 0, 0));
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { state = S_ON; }
      break;

    case S_TURN_OFF:
      eyes.fill(eyes.Color(0, 0, 0, 0));
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { state = S_OFF; }
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
    if (state == S_ON) {
      state = S_TURN_OFF;
    } else if (state == S_OFF) {
      state = S_TURN_ON;
    }
    
  }
  last_interrupt_time = interrupt_time;
}
