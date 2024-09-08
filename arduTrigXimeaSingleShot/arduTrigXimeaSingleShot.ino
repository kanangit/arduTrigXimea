const int pinIN1 = 8;
int flag = 0;

void setup() {
  pinMode(pinIN1, OUTPUT);
    Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

}

void loop() {
  // digitalWrite(pinIN1, HIGH);
  // delay(1000);
  // digitalWrite(pinIN1, LOW);


  if (flag == 0) 
  {    
    
    Serial.println(flag);
    for (int i = 0; i < 1; i++)
    {
      digitalWrite(pinIN1, LOW);
      delay(40);
      digitalWrite(pinIN1, HIGH);
      Serial.print("frame = ");
      Serial.print(i);
      Serial.println(" ");
      
    }
    flag = 1;
    Serial.println(flag);

  }
  delay(30000);
  flag = 0;
  

}
