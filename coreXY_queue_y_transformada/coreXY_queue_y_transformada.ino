#include <AccelStepper.h>

// The X Stepper pins
#define STEPPER1_DIR_PIN 3
#define STEPPER1_STEP_PIN 2
// The Y stepper pins
#define STEPPER2_DIR_PIN 7
#define STEPPER2_STEP_PIN 6

struct Coordinates {
  int posx;
  int posy;
};

int varX[10];
int varY[2];

int maxX = 280;
int maxY = 280;
int minX = 0;
int minY = 0;

// o double?
double ScaleFactorX = 1;
double ScaleFactorY = 1;

// Maximo de coordenadas almacenadas en un tiempo dado
const int maxQueueSize = 10;

// Definir los steppers y pines que se van a usar (si no funciona el primer parámetro probar con el número 1)
AccelStepper stepper1(AccelStepper::DRIVER, STEPPER1_STEP_PIN, STEPPER1_DIR_PIN);
AccelStepper stepper2(AccelStepper::DRIVER, STEPPER2_STEP_PIN, STEPPER2_DIR_PIN);


Coordinates currentCoords;
Coordinates queue[maxQueueSize];
int queueSize = 0;
Coordinates previousCoords;


void setup() {
  Serial.begin(9600);
  currentCoords.posx = 0;
  currentCoords.posy = 0;

  stepper1.setMaxSpeed(100.0);
  stepper1.setAcceleration(100.0);
  stepper1.moveTo(100);

  stepper2.setMaxSpeed(100.0);
  stepper2.setAcceleration(100.0);
  stepper2.moveTo(100);
}

void loop() {
  // Revisa si hay nuevas coordenadas
  Coordinates newCoords = leerSerial();

  // falta agregar condiciones de limites
  if (newCoords.posx != -1) {
    if (queueSize < maxQueueSize) {
      queue[queueSize] = newCoords;
      queueSize++;
      Serial.println(queueSize);
    }
  }

  // Cambiar movimiento de los motores
  if (stepper1.distanceToGo() == 0 && stepper2.distanceToGo() == 0) {
    if (queueSize > 0) {
      // Hay coordenadas en queue, ejecutarlas:
      previousCoords = currentCoords;
      currentCoords = queue[0];
      //// Nuevas coordenadas a usar
      //Serial.print("posx = ");
      //Serial.println(currentCoords.posx);
      //Serial.print("posy = ");
      //Serial.println(previousCoords.posx);

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

// se deben ingresar las coordenadas en el formato X# Y# ej: X100 Y200
Coordinates leerSerial() {
  Coordinates result;
  result.posx = -1;  // valor para indicar que no hay coordenadas nuevas
  result.posy = -1;

  if (Serial.available() > 0) {
    String inputString = Serial.readStringUntil('\n');
    if (inputString.startsWith("X")) {
      int y_sub = inputString.indexOf(" ");
      result.posx = inputString.substring(1, y_sub).toInt();
      result.posy = inputString.substring(y_sub + 2).toInt();
      Serial.print("El valor ingresado fue: x = ");
      Serial.print(result.posx);
      Serial.print(", y = ");
      Serial.println(result.posy);
    }
  }
  return result;
}

// falta mirar cuando se pasa de los limites el delta en X,Y
Coordinates transformacion_a_corexy(const Coordinates& current, const Coordinates& previous){
  Coordinates delta;
  delta.posx = current.posx - previous.posx;
  delta.posy = current.posy - previous.posy;
 // tranformar a movimiento de motores en el coreXY
  Coordinates motor;
  motor.posx = int(ScaleFactorX*delta.posx + ScaleFactorY*delta.posy);
  motor.posy = int(ScaleFactorX*delta.posx - ScaleFactorY*delta.posy);

  return motor;
}
