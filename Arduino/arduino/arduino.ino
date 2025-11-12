#include <Servo.h>

Servo servos[5];
float degreesServo[5];  // Estado lógico del servo: 0 = abierto, 180 = cerrado

String buffer = "";   // Almacena los datos recibidos
String lastData = ""; // Para almacenar el último mensaje válido entre saltos de línea
void setup() {
  Serial.begin(9600);
  initServos();
 
}

void initServos() {

  for (int i = 0; i < 5; i++) {
    servos[i].attach(i + 2);  
    servos[i].write(0);      
    degreesServo[i] = 0;    
  }
}

void loop() {
  // testServos();

  readSerialData(); // Lee y actualiza el buffer con datos válidos
  actions();        // Procesa el último mensaje válido
}

void readSerialData() {
  // Leer todos los datos disponibles
  while (Serial.available()) {
    char incoming = Serial.read();
    buffer += incoming;
  }

  // Buscar el último y penúltimo salto de línea
  int lastNewline = buffer.lastIndexOf('\n');
  int secondLastNewline = buffer.lastIndexOf('\n', lastNewline - 1);

  if (lastNewline != -1 && secondLastNewline != -1) {
    // Extraer los datos entre el último y penúltimo salto de línea
    lastData = buffer.substring(secondLastNewline + 1, lastNewline);

    // Eliminar los datos procesados del buffer
    buffer = buffer.substring(lastNewline);
  }
}

void actions() {
  if (lastData.length() >= 5) { // Asegura que hay datos para los 5 servos
    for (int i = 0; i < 5; i++) {
      if (lastData[i] == '1' && degreesServo[i]!=180) {
        servos[i].write(180);  // Cierra el dedo
        degreesServo[i] = 180;
      } 
      else if (lastData[i] == '0' && degreesServo[i]!=0) {
        servos[i].write(0);    // Abre el dedo
        degreesServo[i] = 0;
      }
    }
    lastData = ""; // Limpia el string después de procesarlo    
  }

}
