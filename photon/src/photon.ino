/*
 * Project photon
 * Description:
 * Author:
 * Date:
 */

 #include "HX711ADC/HX711ADC.h"

 // Pins
 #define pump D1
 #define dout A1
 #define clk A0

 // Output device parameters
 #define dispenseTimeForOneShot 800

// setup() runs once, when the device is first turned on.
//void setup() {
//  // Put initialization like pinMode and begin function here.
//  pinMode(pump, OUTPUT);
//  delay(1000);
//
//}

HX711ADC *scale = NULL;

int calibration_factor = 14000; //-7050 worked for my 440lb max scale setup

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  scale = new HX711ADC(7, 6);
  scale->set_scale(calibration_factor);

  scale->tare();	//Reset the scale to 0


  long zero_factor = scale->read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}

void loop() {

  //scale->set_scale(calibration_factor); //Adjust to this calibration factor

  Serial.print("Reading: ");
  Serial.print(scale->get_units(), 1);
  Serial.print(" lbs"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();

  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 10;
  }

  delay(200);
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
