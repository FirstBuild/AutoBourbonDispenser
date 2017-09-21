/*
* Project photon
* Description:
* Author:
* Date:
*/

#include "HX711ADC/HX711ADC.h"
#include "FastLED/FastLED.h"
#include "RBD_Timer/RBD_Timer.h"
#include "RBD_Button/RBD_Button.h"

FASTLED_USING_NAMESPACE;

#define LED_PIN     A5
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS    113

#define BRIGHTNESS  100
#define FRAMES_PER_SECOND 60

CRGB leds[NUM_LEDS];

// Pins
#define pumpControlPin D1
#define HX711_DOUT_PIN D7
#define HX711_CLK_PIN D6
#define dispenseBtnPin A2
#define manualDispenseSwitchPin D3
#define autoDispenseSwitchPin D2

// Output device parameters
#define dispenseTimeForOneShot 800

HX711ADC *scale = NULL;
int calibration_factor = 14000;

RBD::Button dispensePushButton(dispenseBtnPin);
RBD::Button manualDispenseSwitch(manualDispenseSwitchPin);
RBD::Button autoDispenseSwitch(autoDispenseSwitchPin);

static int dispenseMode = 0;

void setup() {
  // Sanity delay.
  delay(2000);

  // Put initialization like pinMode and begin function here.
  Serial.begin(9600);
  pinMode(pumpControlPin, OUTPUT);

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( CRGB::Seashell );
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setTemperature( CRGB::Seashell );
  FastLED.setBrightness( BRIGHTNESS );
}

void loop() {
  handleDispenseButton();
  handleDoubleThrowToggle();
  handleBullsEyeWindowLights();
}

void handleBullsEyeWindowLights() {
  allWhite();
  FastLED.show(); // display this frame
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}

void handleDoubleThrowToggle() {
  if (manualDispenseSwitch.onPressed()) {
    dispenseMode = 1;
    Serial.println("Manual Dispense Mode: Enabled");
  } else if (manualDispenseSwitch.onReleased()) {
    dispenseMode = 0;
    Serial.println("Manual Dispense Mode: Disabled");
  }

  if (autoDispenseSwitch.onPressed()) {
    dispenseMode = 2;
    Serial.println("Automatic Dispense Mode: Enabled");
  } else if (autoDispenseSwitch.onReleased()) {
    dispenseMode = 0;
    Serial.println("Automatic Dispense Mode: Disabled");
  }
}

void handleDispenseButton() {
    if (dispenseMode == 1 && dispensePushButton.onPressed()) {
      Serial.println("Dispensing liquid.");
      dispenseLiquid();
    }
}

//void setup() {
//  Serial.begin(9600);
//  Serial.println("HX711 calibration sketch");
//  Serial.println("Remove all weight from scale");
//  Serial.println("After readings begin, place known weight on scale");
//  Serial.println("Press + or a to increase calibration factor");
//  Serial.println("Press - or z to decrease calibration factor");
//
//  scale = new HX711ADC(HX711_DOUT_PIN, HX711_CLK_PIN);
//  scale->set_scale(calibration_factor);
//
//  scale->tare();	//Reset the scale to 0
//
//  long zero_factor = scale->read_average(); //Get a baseline reading
//  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
//  Serial.println(zero_factor);
//}

//void loop() {
//
//  //scale->set_scale(calibration_factor); //Adjust to this calibration factor
//
//  Serial.print("Reading: ");
//  Serial.print(scale->get_units(), 1);
//  Serial.print(" lbs"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
//  Serial.print(" calibration_factor: ");
//  Serial.print(calibration_factor);
//  Serial.println();
//
//  if(Serial.available())
//  {
//    char temp = Serial.read();
//    if(temp == '+' || temp == 'a')
//      calibration_factor += 10;
//    else if(temp == '-' || temp == 'z')
//      calibration_factor -= 10;
//  }
//
//  delay(200);
//
//}

void allWhite() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Seashell;
  }
}

void dispenseLiquid() {
  //  Given that the mosfet range is [0, 5.0], D1 is rated only up to
  //    3.3v. Therefore, we are able to achieve 50% duty cycle by
  //    writing 194.
  //  Calculations: 194 / 255 = 76%
  //                .76 * 3.3 = 2.5
  //  50% duty cycle is achieved with a 2.5v mosfet signal output.
  analogWrite(pumpControlPin, 194);
  delay(800);
  analogWrite(pumpControlPin, 0);
}
