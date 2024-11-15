#include <Servo.h>
Servo servos[5];
float degreesServo[5];
String data = ""; // Para almacenar los datos recibidos
void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 5; i++) {
    servos[i].attach(i+2);  // Asocia cada servo al pin correspondiente
    servos[i].write(0);
  }
}

void loop() {
  if (Serial.available()) {
    data = Serial.readStringUntil('\n'); // Lee hasta que recibe nueva línea
    Serial.println("Data received: " + data); // Imprime la cadena recibida
    for (int i =0 ; i<5;i++){
      int degrees= data[i]=='1'? 170: 10;
      if(degrees != degreesServo[i]){
        degreesServo[i]=degrees;
        servos[i].write(degrees);
      }
    }
  }
  delay(10);
}
