// MultiStepper.pde
// -*- mode: C++ -*-
// Use MultiStepper class to manage multiple steppers and make them all move to
// the same position at the same time for linear 2d (or 3d) motion.

#include <AccelStepper.h>
#include <MultiStepper.h>


// The A Stepper pins
#define STEPPER1_DIR_PIN 8
#define STEPPER1_STEP_PIN 9
// The B stepper pins
#define STEPPER2_DIR_PIN 10
#define STEPPER2_STEP_PIN 11
// EG X-Y position bed driven by 2 steppers

// Definir los steppers y pines que se van a usar (si no funciona el primer parámetro probar con el número 1)
AccelStepper stepper1(AccelStepper::DRIVER, STEPPER1_STEP_PIN, STEPPER1_DIR_PIN);
AccelStepper stepper2(AccelStepper::DRIVER, STEPPER2_STEP_PIN, STEPPER2_DIR_PIN);

// Up to 10 steppers can be handled as a group by MultiStepper
MultiStepper steppers;


long positions[2];
long inputx;
long inputy;
double ScaleFactorA = 1.9;
double ScaleFactorB = 1.9;

void setup() {
  Serial.begin(9600);
  Serial.println("iniciando...");

  // Configure each stepper
  stepper1.setMaxSpeed(100);
  stepper2.setMaxSpeed(100);

  // Then give them to MultiStepper to manage
  steppers.addStepper(stepper1);
  steppers.addStepper(stepper2);

  positions[0] = 0;
  positions[1] = 0;
}

void loop() {
  Serial.println("esperando");

  if (Serial.available() > 0) {
    String inputString = Serial.readStringUntil('\n');
    if (inputString.startsWith("G01")) {
      int y_sub = inputString.indexOf("Y");
      int x_sub = inputString.indexOf("X");
      inputx = inputString.substring(x_sub + 1, y_sub - 1).toFloat();
      inputy = inputString.substring(y_sub + 1).toFloat();
      // Serial.println("Recibido");

    } else {
      Serial.println("ingresar coordenadas en gcode");
    }
  }
  // Serial.print("El valor ingresado fue: x = ");
  // Serial.print(inputx);
  // Serial.print(", y = ");
  // Serial.println(inputy);

  positions[0] = int(ScaleFactorA*(inputx + inputy));
  //stepper2 lado derecho del corexy
  positions[1] = int(-ScaleFactorB*(inputx - inputy));

  steppers.moveTo(positions);
  steppers.runSpeedToPosition();  // Blocks until all are in position
  // Serial.println(stepper1.distanceToGo());
  delay(1000);
  
}
