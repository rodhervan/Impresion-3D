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


float maxX = 10000;
float maxY = 10000;
float minX = -10000;
float minY = -10000;

// Ajustar dependiendo de cuanto se mueva cada motor
double ScaleFactorA = 1;
double ScaleFactorB = 1;

// Maximo de coordenadas almacenadas en un tiempo dado
const int maxQueueSize = 36;

// Queue to store the last 5 positions from stepper1
const int StqueueSize = 100;
int positionQueue1[StqueueSize];
int positionQueue2[StqueueSize];
int queueIndex1 = 0;
int queueIndex2 = 0;

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

void addToQueue1(int position) {
  positionQueue1[queueIndex1] = position;
  queueIndex1 = (queueIndex1 + 1) % StqueueSize;
}
// Function to add a position to the queue for stepper2
void addToQueue2(int position) {
  positionQueue2[queueIndex2] = position;
  queueIndex2 = (queueIndex2 + 1) % StqueueSize;
}
bool areAllEqual1(int value) {
  for (int i = 0; i < StqueueSize; i++) {
    if (positionQueue1[i] != value || value == 0) {
      return false;
    }
  }
  return true;
}
// Function to check if all elements in the queue for stepper2 are the same
bool areAllEqual2(int value) {
  for (int i = 0; i < StqueueSize; i++) {
    if (positionQueue2[i] != value || value == 0) {
      return false;
    }
  }
  return true;
}

void loop() {
  
  // Revisa si hay nuevas coordenadas
  Coordinates newCoords = leerSerial();
  if (newCoords.posx != -1) {
    if (queueSize < maxQueueSize) {
      queue[queueSize] = newCoords;
      queueSize++;
      Serial.println(queueSize);
      updateMotorTargets();
    }
  }
  int stepper1Position = stepper1.distanceToGo();
  int stepper2Position = stepper2.distanceToGo();
  addToQueue1(stepper1Position);
  addToQueue2(stepper2Position);
  Serial.print(stepper1.distanceToGo());
  Serial.print("  ");
  Serial.println(stepper2.distanceToGo());

  if (areAllEqual1(stepper1Position)) {
    // Do something when all elements in the queue are the same (excluding zero)
    // Replace the following line with your desired action.
    Serial.println("All elements in the queue are the same");
    stepper1.moveTo(stepper1.currentPosition());
    stepper2.moveTo(stepper2.currentPosition());
    stepper1.run();
    stepper2.run();
        // Reset all elements in the queue to zero
    for (int i = 0; i < StqueueSize; i++) {
      positionQueue1[i] = 0;
    }

  }
  if (areAllEqual2(stepper2Position)) {
    // Do something when all elements in the queue for stepper2 are the same (excluding zero)
    Serial.println("All elements in the queue for stepper2 are the same");
    stepper1.moveTo(stepper1.currentPosition());
    stepper2.moveTo(stepper2.currentPosition());
    stepper1.run();
    stepper2.run();
    
    // Reset all elements in the queue for stepper2 to zero
    for (int i = 0; i < StqueueSize; i++) {
      positionQueue2[i] = 0;
    }
  }


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

  // // revisa que este dentro de los limites
  // if ((current.posx > maxX)||(current.posy > maxY)||(current.posx < minX)||(current.posy < minY)){
  //   Serial.println("Fuera del rango");
  // }
 ////// tranformar deltas en x,y a movimiento de motores en el coreXY
  Coordinates motor;
  // lo transformo a int porque no sé si la función de los steppers recibe input double
  //motor.posx = int(ScaleFactorA*(delta.posx + delta.posy));
  
  //motor.posy = int(ScaleFactorB*(delta.posx - delta.posy));
  //stepper1 lado izquierdo del corexy
  motor.posx = int(-ScaleFactorA*(current.posx + current.posy));
  //stepper2 lado derecho del corexy
  motor.posy = int(ScaleFactorB*(current.posx - current.posy));



  return motor;
}

void updateMotorTargets() {
  if (queueSize > 0) {
    Coordinates current = queue[0];
    Coordinates motor = transformacion_a_corexy(current, currentCoords);
    stepper1.moveTo(motor.posx);
    stepper2.moveTo(motor.posy);
  }
}