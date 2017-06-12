/*-------------------------------------------------------------------------
  Spark Core, Particle Photon, P1, Electron and RedBear Duo library to control
  WS2811/WS2812 based RGB LED devices such as Adafruit NeoPixel strips.

  Supports:
  - 800 KHz and 400kHz bitstream WS2812, WS2812B and WS2811
  - 800 KHz bitstream SK6812RGBW (NeoPixel RGBW pixel strips)
    (use 'SK6812RGBW' as PIXEL_TYPE)

  Also supports:
  - Radio Shack Tri-Color Strip with TM1803 controller 400kHz bitstream.
  - TM1829 pixels

  PLEASE NOTE that the NeoPixels require 5V level inputs
  and the Spark Core, Particle Photon, P1, Electron and RedBear Duo only
  have 3.3V level outputs. Level shifting is necessary, but will require
  a fast device such as one of the following:

  [SN74HCT125N]
  http://www.digikey.com/product-detail/en/SN74HCT125N/296-8386-5-ND/376860

  [SN74HCT245N]
  http://www.digikey.com/product-detail/en/SN74HCT245N/296-1612-5-ND/277258

  Written by Phil Burgess / Paint Your Dragon for Adafruit Industries.
  Modified to work with Particle devices by Technobly.
  Contributions by PJRC and other members of the open source community.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!
  --------------------------------------------------------------------*/

/* ======================= includes ================================= */

#include "Particle.h"
#include "neopixel.h"


/* ======================= prototypes =============================== */

void colorAll(uint32_t c, uint8_t wait);
void colorWipe(uint32_t c, uint8_t wait);
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
uint32_t Wheel(byte WheelPos);

/* ======================= extra-examples.cpp ======================== */

SYSTEM_MODE(SEMI_AUTOMATIC);

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_COUNT 12
#define PIXEL_PIN D7
#define PIXEL_TYPE WS2812B
#define POWERPIN1 D3
#define POWERPIN2 D4
#define POWERPIN3 D5
#define POWERPIN4 D6

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

/* ======================= variables ======================== */
FuelGauge fuel;
String voltage = "0"; // Variable to keep track of LiPo voltage
double soc = 0; // Variable to keep track of LiPo state-of-charge (SOC)
int state=0;
int brightness=1;

void setup() {
  Serial.begin(9600); // Start serial, to output debug data

  //Setup for Power Controlls
  Particle.function("power",pwr);
  Particle.function("changeBright",chngbrght);
  Particle.variable("brightness",brightness);
  Particle.variable("state",state);
  pinMode(POWERPIN1,OUTPUT);
  pinMode(POWERPIN2,OUTPUT);
  pinMode(POWERPIN3,OUTPUT);
  pinMode(POWERPIN4,OUTPUT);

  //Setup for NeoPixel
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  //Setup for Sparkfun Battery Sheild


	// Set up Spark variables (voltage, soc, and alert):
	Particle.variable("voltage", voltage);
	// To read the values from a browser, go to:
	// http://api.particle.io/v1/devices/{DEVICE_ID}/{VARIABLE}?access_token={ACCESS_TOKEN}

  Particle.connect();

}

void loop() {

  delay(10000);
  if (state==0) {
   Particle.process();
   batteryReport(5000);
   Particle.process();
   System.sleep(SLEEP_MODE_DEEP, 300);
  }
  else if (state==1){
   Particle.process();
   batteryReport(5000);
   Particle.process();

  }
  delay(1000);
  colorAll(strip.Color(1, 1, 1), 50);
  //delay(3000);

}

// Set all pixels in the strip to a solid color, then wait (ms)
void colorAll(uint32_t c, uint8_t wait) {
  uint16_t i;

  if (brightness == 0){
    for (i=0; i<12; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  }
  else {
  for(i=0; i<brightness; i++) {
    strip.setPixelColor(i, c);
  }
  for (i=11; i>=brightness; i--) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  }
  strip.show();
  delay(wait);
}


// Cloud variable to turn the relay switch (camera and neopixel) on and off
int pwr(String command) {
  if (command == "ON") {
  digitalWrite(POWERPIN1,HIGH);
  digitalWrite(POWERPIN2,HIGH);
  digitalWrite(POWERPIN3,HIGH);
  digitalWrite(POWERPIN4,HIGH);
  Serial.println("ON");
  state=1;
  return 1;
  }
  else if (command == "OFF") {
  digitalWrite(POWERPIN1,LOW);
  digitalWrite(POWERPIN2,LOW);
  digitalWrite(POWERPIN3,LOW);
  digitalWrite(POWERPIN4,LOW);
  Serial.println("OFF");
  state=0;
  return 0;
  }
  else {
      return -1;
  }
  delay(1000);


}

void batteryReport(int wait)
{

	// lipo.getVoltage() returns a voltage value (e.g. 3.93)
    voltage = String(fuel.getVCell());
	// lipo.getSOC() returns the estimated state of charge (e.g. 79%)

	// Those variables will update to the Spark Cloud, but we'll also print them
	// locally over serial for debugging:
	Serial.print("Voltage: ");
	Serial.print(voltage);  // Print the battery voltage
	Serial.println(" V");

    Particle.publish("bird_house", voltage, PRIVATE);

     delay(wait);
    //System.sleep(SLEEP_MODE_DEEP, 5);

}

void pwron() {
  digitalWrite(POWERPIN1,HIGH);
  digitalWrite(POWERPIN2,HIGH);
  digitalWrite(POWERPIN3,HIGH);
  digitalWrite(POWERPIN4,HIGH);
}

void pwroff() {
  digitalWrite(POWERPIN1,LOW);
  digitalWrite(POWERPIN2,LOW);
  digitalWrite(POWERPIN3,LOW);
  digitalWrite(POWERPIN4,LOW);
}

// cloud function to change brightness of the neopixel

int chngbrght (String brightvalue) {
    brightness = brightvalue.toInt() - 1;
    return brightness;
}
