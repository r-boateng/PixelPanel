#include <Arduino.h>
#include <FastLED.h>
#include <Wire.h>
#include <Adafruit_APDS9960.h>

// FastLED setup WS2812
#define DATA_PIN D6
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 64
CRGB leds[NUM_LEDS];

#define BRIGHTNESS 200
#define FRAMES_PER_SECOND 120

// 9960 setup
Adafruit_APDS9960 apds;
// SparkFun_APDS9960 apds = SparkFun_APDS9960();
// uint16_t ambient_light = 0;
// uint16_t red_light = 0;
// uint16_t green_light = 0;
// uint16_t blue_light = 0;


// Main Hue for LEDs
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
CRGB crgb;

void setup()
{
  delay(3000); // 3 second delay for recovery

  Serial.begin(115200);

  // ADPS setup
  // if (apds.init()) { Serial.println(F("APDS-9960 initialization complete")); }
  // else { Serial.println(F("Something went wrong during APDS-9960 init!")); }

  // // Start running the APDS-9960 light sensor (no interrupts)
  // if (apds.enableLightSensor(false)) { Serial.println(F("Light sensor is now running")); }
  // else { Serial.println(F("Something went wrong during light sensor init!")); }
  if(!apds.begin()){ Serial.println("failed to initialize device! Please check your wiring."); }
  else Serial.println("Device initialized!");

  //enable color sensign mode
  apds.enableColor(true);
  

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void ScanColour()
{
  //create some variables to store the color data in
  uint16_t r, g, b, ambient;
  
  //wait for color data to be ready
  while(!apds.colorDataReady()){ delay(5); }

  //get the data and print the different channels
  apds.getColorData(&r, &g, &b, &ambient);
  Serial.print("red: ");
  Serial.print(r);
  
  Serial.print(" green: ");
  Serial.print(g);
  
  Serial.print(" blue: ");
  Serial.print(b);
  
  Serial.print(" clear: ");
  Serial.println(ambient);
  Serial.println();
  
  gHue = r<<16|g<<8|b;
  crgb = CRGB(r,g,b);
  //crgb.addToRGB((255 - ambient));

  delay(10);
}

// void ScanColour()
// {
//   CRGB col;

//   // Read the light levels (ambient, red, green, blue)
//   if (!apds.readAmbientLight(ambient_light) || !apds.readRedLight(red_light) ||
//       !apds.readGreenLight(green_light) || !apds.readBlueLight(blue_light))
//   {
//     Serial.println("Error reading light values");
//   }
//   else
//   {
//     red_light = constrain(map(red_light, 50, 1500, 0, 255),0,255);
//     green_light = constrain(map(green_light, 50, 1500, 0, 255),0,255);
//     blue_light = constrain(map(blue_light, 50, 1500, 0, 255),0,255);
//     ambient_light = constrain(map(ambient_light, 50, 1500, 0, 255),0,100);

//     col.r = red_light;
//     col.g = green_light;
//     col.b = blue_light;
//     //gHue = col;
//     gHue = red_light<<16|green_light<<8|blue_light;

//     csvHue = CHSV(gHue + random(8), 200, (255 - ambient_light));

//     Serial.print("gHue: ");
//     Serial.print(gHue);
//     Serial.print(" Ambient: ");
//     Serial.print(ambient_light);
//     Serial.print(" Red: ");
//     Serial.print(red_light);
//     Serial.print(" Green: ");
//     Serial.print(green_light);
//     Serial.print(" Blue: ");
//     Serial.println(blue_light);
    
//   }

//   // Wait before next reading
//   delay(10);
// }

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += crgb;
  //leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void loop()
{
  ScanColour();
  //EVERY_N_BSECONDS(20)
  confetti();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 
}