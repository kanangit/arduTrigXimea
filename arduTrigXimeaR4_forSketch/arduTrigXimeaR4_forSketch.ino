// burst generator on R4 Minima
// for triggering usb Ximea cameras
// after receiving the trig signal outputs one pulse,
// waits for 500 ms, and then outputs the (n_pulses - 1) 
// number of pulses

//May or may not work on older Arduinos (r3 and older)

#include <Arduino.h>
#include <digitalWriteFast.h>

// pins (pick any interrupt‐capable pin for input)
static const uint8_t inputPin = 2;
static const uint8_t out1Pin  = 3;
static const uint8_t out2Pin  = 4;

// state vars
unsigned int pulse_width = 100UL; // duration of individual pulse in us
unsigned int n_pulses = 2000;
//double fps = 400;
//unsigned long reg_delay = round(1.0  * 1000000 / fps);
unsigned long reg_delay = 200UL;
volatile bool triggered = false;  // set in ISR
uint16_t genState = 0;            // 0..500
unsigned long t0, lastTime;       // timestamps in µs
int lastInputState = HIGH;  // for edge detection

void setup() {
  pinMode(inputPin, INPUT_PULLUP);
  pinMode(out1Pin,  OUTPUT);
  pinMode(out2Pin,  OUTPUT);
  digitalWrite(out1Pin, LOW);
  digitalWrite(out2Pin, LOW);

  // catch rising edges on inputPin
  //attachInterrupt(digitalPinToInterrupt(inputPin), onPulse, RISING);
}

void loop() {

  int inState = digitalRead(inputPin);
  if (!triggered && lastInputState == LOW && inState == HIGH) {
    // RISING edge: arm sequence
    triggered = true;
    genState  = 0;
  }
  lastInputState = inState;

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
      // changed to 1000 us
      if (now - t0 >= 1000UL) {

        digitalWriteFast(out2Pin, HIGH);
        digitalWriteFast(out1Pin, HIGH);
        delayMicroseconds(pulse_width);
        digitalWriteFast(out2Pin, LOW);
        digitalWriteFast(out1Pin, LOW);

        lastTime = lastTime + 2 * reg_delay;   // reset for 500 Hz loop
        genState++;
      }
      break;

    default:
      // genState == 2…n_pulses => emit the rest on out2 at 500 Hz
      if (genState < n_pulses) {
        if (now - lastTime >= reg_delay) {     // 2 ms period
          pulse(out2Pin);
          lastTime += reg_delay;
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


// very short single‐pulse on pin (∼10 µs width)
inline void pulse(uint8_t pin) {
  digitalWriteFast(pin, HIGH);
  delayMicroseconds(100);
  digitalWriteFast(pin, LOW);
}