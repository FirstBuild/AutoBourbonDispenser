/*
 * Project photon
 * Description:
 * Author:
 * Date:
 */

 #define dispenseTimeForOneShot 800

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  pinMode(D1, OUTPUT);
  delay(1000);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
}

void dispenseLiquid() {
  analogWrite(D1, 194);
  delay(800);
  analogWrite(D1, 0);
}
