#include <Servo.h>

Servo servos[5];

float degreesServo[5];  // Estado lógico del servo: 0 = abierto, 180 = cerrado
int velServo[5];  // Estado lógico del servo: 0 = abierto, 180 = cerrado

float vel=0.005;

String buffer = "";   // Almacena los datos recibidos
String lastData = ""; // Para almacenar el último mensaje válido entre saltos de línea
void setup() {
  Serial.begin(9600);
  initServos();
}

void initServos() {
  for (int i = 0; i < 5; i++) {
    servos[i].attach(i + 2);  // Asocia cada servo al pin correspondiente
    servos[i].write(90);      // Inicializa en posición neutra
    degreesServo[i] = 0;      // Estado inicial: abierto
    velServo[i]=0;
  }
}

void loop() {
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
  // Procesa solo el último dato válido
  if (lastData.length() >= 5) { // Asegura que hay datos para los 5 servos
    for (int i = 0; i < 5; i++) {
      if (lastData[i] == '1') {
        servos[i].write(160);  // Cierra el dedo
        velServo[i]=-1;
      } else if (lastData[i] == '0') {
        servos[i].write(20);    // Abre el dedo
        velServo[i]=1;
      }
    }
    lastData = ""; // Limpia el string después de procesarlo    
  }

  for(int i = 0; i < 5; i++){
    float newTotal= degreesServo[i]+velServo[i]*vel;
    if(newTotal<=0 || newTotal>=100 ){
      servos[i].write(90);
      velServo[i]=0;
    }
    else{
      degreesServo[i]=newTotal;
    }
  }
}
