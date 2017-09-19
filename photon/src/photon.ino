/*
 * Project photon
 * Description:
 * Author:
 * Date:
 */

 // Pins
 #define pump D1

 // Output device parameters
 #define dispenseTimeForOneShot 800

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin function here.
  pinMode(pump, OUTPUT);
  delay(1000);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
}

void dispenseLiquid() {
  //  Given that the mosfet range is [0, 5.0], D1 is rated only up to
  //    3.3v. Therefore, we are able to achieve 50% duty cycle by
  //    writing 194.
  //  Calculations: 194 / 255 = 76%
  //                .76 * 3.3 = 2.5
  //  50% duty cycle is achieved with a 2.5v mosfet signal output.
  analogWrite(pump, 194);
  delay(800);
  analogWrite(pump, 0);
}
