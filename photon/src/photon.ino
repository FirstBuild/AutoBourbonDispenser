/*
 * Project photon
 * Description:
 * Author:
 * Date:
 */

#include "HX711ADC/HX711ADC.h"
#include "FastLED/FastLED.h"

FASTLED_USING_NAMESPACE;

#define LED_PIN     A5
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS    113

#define BRIGHTNESS  100
#define FRAMES_PER_SECOND 60

CRGB leds[NUM_LEDS];

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
  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( CRGB::Seashell );
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setTemperature( CRGB::Seashell );
  FastLED.setBrightness( BRIGHTNESS );

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

  allWhite();
  // Add entropy to random number generator; we use a lot of it.
  //random16_add_entropy( random(256));

  //Fire2012(); // run simulation frame

  FastLED.show(); // display this frame
  FastLED.delay(1000 / FRAMES_PER_SECOND);
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
  analogWrite(pump, 194);
  delay(800);
  analogWrite(pump, 0);
}

// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
////
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation,
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking.
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
#define COOLING  70

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120


void Fire2012()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little

    // Left side -- Bull's Eye Hole

    for( int k= (NUM_LEDS / 2) - 1; k < NUM_LEDS; k++) {
        heat[k] = (heat[k + 1] + heat[k + 2] + heat[k + 2] ) / 3;
    }

    // Right side -- Bull's Eye Hole
    for( int k= (NUM_LEDS / 2) - 1; k >= 0; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }

     //Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int l = random8(NUM_LEDS - 9, NUM_LEDS);
      heat[l] = qadd8( heat[l], random8(160,255) );

      int r = random8(9);
      heat[r] = qadd8( heat[r], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
        if (j < NUM_LEDS - 9) {
          leds[j + 9] = HeatColor( heat[j]);
        } else {
          leds[j % (NUM_LEDS - 9)] = HeatColor( heat[j]);
        }
    }
}
