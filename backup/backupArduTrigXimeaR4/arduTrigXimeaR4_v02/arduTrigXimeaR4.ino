// 500‐pulse burst generator on R4 Minima. 
//May or may not work on older Arduinos (r3 and older)

#include <Arduino.h>
#include <digitalWriteFast.h>

// pins (pick any interrupt‐capable pin for input)
static const uint8_t inputPin = 2;
static const uint8_t out1Pin  = 3;
static const uint8_t out2Pin  = 4;

// state vars
volatile bool triggered = false;  // set in ISR
uint16_t genState = 0;            // 0..500
unsigned long t0, lastTime;       // timestamps in µs

void setup() {
  pinMode(inputPin, INPUT_PULLUP);
  pinMode(out1Pin,  OUTPUT);
  pinMode(out2Pin,  OUTPUT);
  digitalWrite(out1Pin, LOW);
  digitalWrite(out2Pin, LOW);

  // catch rising edges on inputPin
  attachInterrupt(digitalPinToInterrupt(inputPin), onPulse, RISING);
}

void loop() {
  if (!triggered) return;

  unsigned long now = micros();
  switch (genState) {
    case 0:
      // pulse #1 on out2 immediately
      pulse(out2Pin);
      t0 = now;
      lastTime = now;
      genState++;
      break;

    case 1:
      // wait 500 ms before the simultaneous second pulse
      if (now - t0 >= 500000UL) {
        pulse(out2Pin);
        pulse(out1Pin);
        lastTime = micros();   // reset for 500 Hz loop
        genState++;
      }
      break;

    default:
      // genState == 2…500 => emit the rest on out2 at 500 Hz
      if (genState < 500) {
        if (now - lastTime >= 2000UL) {     // 2 ms period
          pulse(out2Pin);
          lastTime += 2000UL;
          genState++;
        }
      } else {
        // done
        triggered = false;
        genState   = 0;
      }
      break;
  }
}

// simple ISR: arm the sequence, but do NOT block here
void onPulse() {
  if (!triggered) {
    triggered = true;
    genState  = 0;
  }
}

// very short single‐pulse on pin (∼10 µs width)
inline void pulse(uint8_t pin) {
  digitalWriteFast(pin, HIGH);
  delayMicroseconds(100);
  digitalWriteFast(pin, LOW);
}