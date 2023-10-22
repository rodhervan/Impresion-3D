#include <AccelStepper.h>

// The X Stepper pins
#define STEPPER1_DIR_PIN 3
#define STEPPER1_STEP_PIN 2
// The Y stepper pins
#define STEPPER2_DIR_PIN 7
#define STEPPER2_STEP_PIN 6

int posx = 0;
int posy = 0;

// Define some steppers and the pins the will use
AccelStepper stepper1(AccelStepper::DRIVER, STEPPER1_STEP_PIN, STEPPER1_DIR_PIN);
AccelStepper stepper2(AccelStepper::DRIVER, STEPPER2_STEP_PIN, STEPPER2_DIR_PIN);

void setup() {
  Serial.begin(9600);
  stepper1.setMaxSpeed(200.0);
  stepper1.setAcceleration(200.0);
  stepper1.moveTo(100);

  stepper2.setMaxSpeed(100.0);
  stepper2.setAcceleration(100.0);
  stepper2.moveTo(100);
}

void loop() {
  leerSerial();
  // Change direction at the limits
  if (stepper1.distanceToGo() == 0)
    stepper1.moveTo(posx);
  if (stepper2.distanceToGo() == 0)
    stepper2.moveTo(posy);
  stepper1.run();
  stepper2.run();
}

// se deben ingresar las coordenadas en el formato x=#,y=# ej: x=100,y=200
void leerSerial() {
  if (Serial.available() > 0) {
    String inputString = Serial.readStringUntil('\n');
    if (inputString.startsWith("x=")) {
      int y_sub = inputString.indexOf(",");
      int posx = inputString.substring(2, y_sub).toInt();
      int posy = inputString.substring(y_sub+3).toInt();
      Serial.print("El valor ingresado fue: x = ");
      Serial.print(posx);
      Serial.print(", y = ");
      Serial.println(posy);
    }
  }
}

