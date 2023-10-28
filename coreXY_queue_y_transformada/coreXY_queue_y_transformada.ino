#include <AccelStepper.h>

// The A Stepper pins
#define STEPPER1_DIR_PIN 3
#define STEPPER1_STEP_PIN 2
// The B stepper pins
#define STEPPER2_DIR_PIN 7
#define STEPPER2_STEP_PIN 6

struct Coordinates {
  float posx;
  float posy;
};


float maxX = 220;
float maxY = 200;
float minX = 0;
float minY = 0;

// Ajustar dependiendo de cuanto se mueva cada motor
double ScaleFactorA = 1;
double ScaleFactorB = 1;

// Maximo de coordenadas almacenadas en un tiempo dado
const int maxQueueSize = 36;

// Definir los steppers y pines que se van a usar (si no funciona el primer parámetro probar con el número 1)
AccelStepper stepper1(AccelStepper::DRIVER, STEPPER1_STEP_PIN, STEPPER1_DIR_PIN);
AccelStepper stepper2(AccelStepper::DRIVER, STEPPER2_STEP_PIN, STEPPER2_DIR_PIN);


Coordinates currentCoords;
Coordinates queue[maxQueueSize];
int queueSize = 0;
Coordinates previousCoords;


void setup() {
  Serial.begin(9600);
  Serial.println("iniciando recorrido...");
  currentCoords.posx = 0;
  currentCoords.posy = 0;

  stepper1.setMaxSpeed(100.0);
  stepper1.setAcceleration(100.0);
  stepper1.moveTo(0);

  stepper2.setMaxSpeed(100.0);
  stepper2.setAcceleration(100.0);
  stepper2.moveTo(0);
}

void loop() {
  // Revisa si hay nuevas coordenadas
  Coordinates newCoords = leerSerial();
  if (newCoords.posx != -1) {
    if (queueSize < maxQueueSize) {
      queue[queueSize] = newCoords;
      queueSize++;
      Serial.println(queueSize);
    }
  }
 //Serial.print(stepper1.distanceToGo());
 //Serial.print("  ");
 //Serial.println(stepper2.distanceToGo());


  // Cambiar movimiento de los motores
  if (stepper1.distanceToGo() == 0 && stepper2.distanceToGo() == 0) {
    if (queueSize > 0) {
      // Hay coordenadas en queue, ejecutarlas:
      previousCoords = currentCoords;
      currentCoords = queue[0];

      Coordinates motor = transformacion_a_corexy(currentCoords, previousCoords);
      Serial.print("motor A: ");
      Serial.print(motor.posx);
      Serial.print(", motor B ");
      Serial.println(motor.posy);

      // movimiento del motor con esas coordenadas
      stepper1.moveTo(motor.posx);
      stepper2.moveTo(motor.posy);

      // Quitar coordenadas ya ejecutadas del queue
      for (int i = 0; i < queueSize - 1; i++) {
        queue[i] = queue[i + 1];
      }
      queueSize--;
    }
  }

  stepper1.run();
  stepper2.run();
}

// se deben ingresar las coordenadas en gcode G01 X89.00 Y14.71
Coordinates leerSerial() {
  Coordinates result;
  result.posx = -1;  // valor para indicar que no hay coordenadas nuevas
  result.posy = -1;

  if (Serial.available() > 0) {
    String inputString = Serial.readStringUntil('\n');
    if (inputString.startsWith("G01")) {
      int y_sub = inputString.indexOf("Y");
      int x_sub = inputString.indexOf("X");
      result.posx = inputString.substring(x_sub+1, y_sub-1).toFloat();
      result.posy = inputString.substring(y_sub + 1).toFloat();
      Serial.print("El valor ingresado fue: x = ");
      Serial.print(result.posx);
      Serial.print(", y = ");
      Serial.println(result.posy);
    }
    else{
      Serial.println("ingresar coordenadas en gcode");
    }
  }
  return result;
}

Coordinates transformacion_a_corexy(const Coordinates& current, const Coordinates& previous){
  Coordinates delta;
  //// calculo de los Deltas
  // revisa que este dentro de los limites
  if ((current.posx > maxX)||(current.posy > maxY)||(current.posx < minX)||(current.posy < minY)){
    delta.posx = 0;
    delta.posy = 0;
    Serial.println("Fuera del rango");
  }
  else{
  delta.posx = current.posx - previous.posx;
  delta.posy = current.posy - previous.posy;
  }
 ////// tranformar deltas en x,y a movimiento de motores en el coreXY
  Coordinates motor;
  // lo transformo a int porque no sé si la función de los steppers recibe input double
  motor.posx = int(ScaleFactorA*delta.posx + ScaleFactorB*delta.posy);
  motor.posy = int(ScaleFactorA*delta.posx - ScaleFactorB*delta.posy);

  return motor;
}