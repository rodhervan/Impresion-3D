#include <Arduino.h>
#include <AccelStepper.h>
#include <WiFi.h>

// const char* ssid     = "Place Wifi name here";
// const char* password = "Place your Wifi password here";

// The X Stepper pins
#define STEPPER1_DIR_PIN 16
#define STEPPER1_STEP_PIN 17
// The Y stepper pins
#define STEPPER2_DIR_PIN 18
#define STEPPER2_STEP_PIN 19

struct Coordinates {
  int posx;
  int posy;
};

AccelStepper stepper1(AccelStepper::DRIVER, STEPPER1_STEP_PIN, STEPPER1_DIR_PIN);
AccelStepper stepper2(AccelStepper::DRIVER, STEPPER2_STEP_PIN, STEPPER2_DIR_PIN);

Coordinates currentCoords;

void setup() {

  Serial.begin(115200);
  currentCoords.posx = 0;
  currentCoords.posy = 0;

  stepper1.setMaxSpeed(200.0);
  stepper1.setAcceleration(200.0);
  stepper1.moveTo(100);

  stepper2.setMaxSpeed(100.0);
  stepper2.setAcceleration(100.0);
  stepper2.moveTo(100);
}

// se deben ingresar las coordenadas en el formato x=#,y=# ej: x=100,y=200
Coordinates leerSerial() {
  Coordinates result;
  result.posx = -1; // valor para indicar que no hay coordenadas nuevas
  result.posy = -1;

  if (Serial.available() > 0) {
    String inputString = Serial.readStringUntil('\n');
    if (inputString.startsWith("x=")) {
      int y_sub = inputString.indexOf(",");
      result.posx = inputString.substring(2, y_sub).toInt();
      result.posy = inputString.substring(y_sub + 3).toInt();
      Serial.print("El valor ingresado fue: x = ");
      Serial.print(result.posx);
      Serial.print(", y = ");
      Serial.println(result.posy);
    }
  }
  return result;
}

void loop() {

 // Revisa si hay nuevas coordenadas
  Coordinates newCoords = leerSerial();

  // actualiza si hay disponibles
  if (newCoords.posx != -1) {
    currentCoords = newCoords;
  }

  // Nuevas coordenadas a usar
  Serial.print("posx = ");
  Serial.println(currentCoords.posx);
  Serial.print("posy = ");
  Serial.println(currentCoords.posy);

  // Cambiar movimiento de los motores
  if (stepper1.distanceToGo() == 0)
    stepper1.moveTo(currentCoords.posx);
    
  if (stepper2.distanceToGo() == 0)
    stepper2.moveTo(currentCoords.posy);
  stepper1.run();
  stepper2.run();
}
