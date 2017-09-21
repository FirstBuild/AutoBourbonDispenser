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
#define weightSamplingRate 20
#define dispenseDebounceTimeout 15
#define weightHighThreshold 7.0
#define weightLowThreshold 2.0
#define autoDispenseDelay 2000

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

  setupWeightSensing();
}

void loop() {
  handleDoubleThrowToggle();
  handleBullsEyeWindowLights();

  switch (dispenseMode) {
    case 0:
      break;

    case 1:
      handleDispenseButton();
      break;

    case 2:
      handleWeightSensing();
      break;

    default:
      break;
  }
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
    tareScale();
    dispenseMode = 2;
    Serial.println("Automatic Dispense Mode: Enabled");
  } else if (autoDispenseSwitch.onReleased()) {
    dispenseMode = 0;
    Serial.println("Automatic Dispense Mode: Disabled");
  }
}

void handleDispenseButton() {
    if (dispensePushButton.onPressed()) {
      Serial.println("Dispensing liquid.");
      dispenseLiquid();
    }
}

void setupWeightSensing() {
  scale = new HX711ADC(HX711_DOUT_PIN, HX711_CLK_PIN);
  scale->set_scale(calibration_factor);
  tareScale();
}

void tareScale() {
  scale->tare();	//Reset the scale to 0
}

void handleWeightSensing() {
  static float weightInLbs = 0;
  static bool hasDispensedLiquid = false;

  static unsigned long rateTracker = millis();
  if ((millis() - rateTracker) > weightSamplingRate) {
    weightInLbs = scale->get_units();
    Serial.println(scale->get_units(), 1);
    rateTracker = millis();
  }

  static unsigned long dispenseTimeTracker = millis();
  if ((millis() - dispenseTimeTracker) > dispenseDebounceTimeout) {
    if (!hasDispensedLiquid && weightInLbs > weightHighThreshold) {
      hasDispensedLiquid = true;
      Serial.println("Weight placed.");
      //delay(autoDispenseDelay);
      dispenseLiquid();
    } else if (hasDispensedLiquid && weightInLbs <= weightLowThreshold) {
      hasDispensedLiquid = false;
      Serial.println("Weight removed.");
    }
    dispenseTimeTracker = millis();
  }
}

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
