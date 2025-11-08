// burst generator on R4 Minima
// for triggering usb Ximea cameras
// after receiving the trig signal outputs one pulse,
// waits for long_delay microseconds, and then outputs the (n_pulses - 1) 
// number of pulses

//May or may not work on older Arduinos (r3 and older)

#include <Arduino.h>
#include <digitalWriteFast.h>

// state vars
unsigned long pulse_width = 100UL; // duration of an individual pulse in us
unsigned long long_delay = 2000UL; // delay between the first out pulse and and 2nd one
unsigned long reg_delay = 250UL; // delay between the "regular pulses"
unsigned long n_pulses = 2000; // number of pulses
//double fps = 400;
//unsigned long reg_delay = round(1.0  * 1000000 / fps);

// pins
static const uint8_t inputPin = 2;
static const uint8_t out1Pin  = 3;
static const uint8_t out2Pin  = 4;


volatile bool triggered = false;  //
uint16_t genState = 0;            // variabke ti store the pulse number
unsigned long t0, lastTime;       // timestamps of the beginning of the frist pulse and the last time pulsed
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

  unsigned long now = micros(); //timestramp of every cycle of the main loop() function

  switch (genState) {
    case 0:
      // pulse #1 on out2 immediately
      pulse(out2Pin);
      t0 = now;
      lastTime = now;
      genState++;
      break;

    case 1:
      // wait long_delau microseconds before the simultaneous second pulse
    
      if (now - t0 >= long_delay) {

        unsigned long t2_start = micros(); // start time of the second pulse
        digitalWriteFast(out2Pin, HIGH);
        digitalWriteFast(out1Pin, HIGH);
        delayMicroseconds(pulse_width);
        digitalWriteFast(out2Pin, LOW);
        digitalWriteFast(out1Pin, LOW);

        lastTime = t2_start;   // last time pulsed is now the start of the second pulse
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


// very short single‐pulse on pin (pulse_width duration)
inline void pulse(uint8_t pin) {
  digitalWriteFast(pin, HIGH);
  delayMicroseconds(pulse_width);
  digitalWriteFast(pin, LOW);
}