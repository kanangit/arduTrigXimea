const int pinIN1 = 8;

void setup() {
  pinMode(pinIN1, OUTPUT);

}

void loop() {
  // digitalWrite(pinIN1, HIGH);
  // delay(1000);
  // digitalWrite(pinIN1, LOW);

  
  
  digitalWrite(pinIN1, LOW);
  delay(100);
  digitalWrite(pinIN1, HIGH);

}
