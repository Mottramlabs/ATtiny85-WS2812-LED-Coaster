/*

  ATtiny85 demo
  Memory is almost FULL!!!!! @ 95%

  Fruit bowel version 3 - 29th December 2017 - Updated 1st November 2020 for ATtiny85
  Uses a 24 pixel ring and a push button to cycle between animation modes.
  Mode 6 cycles through the animations once evey 20 seconds

  Press and hold the button for 1 seconds and it turns the display on or off.
  The settings are held in EEPROM and read after reset

  Modes 0 - 5 are animations
  Mode 6 - (single white LED) cycles through the above animations
  Mode 7 - Orange
  Mode 8 - Pink
  MOde 9 - Cyan

  Board details
  Board:          "ATtiny25/45/85"
  Timer 1 Clock:  "CPU"
  Chip:           "ATtiny85"
  Clock:          "16 MHz (PLL)"
  B.O.D.:         "B.O.D. Disabled"
  Port:           "/dev/cu.usbmodemFA131 (Arduino/Genuino Uno)"

  First - Load the bootloader

*/

#include <EEPROM.h>
/** the current address in the EEPROM (i.e. which byte we're going to write to next) **/

// Using version 3.1.3 works well. I did use a later version but it work keep on rebooting the Arduino, so use 3.1.3 only!
#include "FastLED.h"

#define DATA_PIN    1
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    24
CRGB leds[NUM_LEDS];

#define BRIGHTNESS         255
#define FRAMES_PER_SECOND  120

uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int Pattern = 1;
int Cycle_Pat = 0;

// ***** Button items *****
#define Button_Pin   2
long Pressed_Timer;
long Released_Timer;
uint8_t Short_Press = LOW;
uint8_t Long_Press = LOW;
uint8_t Long_Used = LOW;
// Time in milliseconds
#define ShortPress 50
#define LongPress 1000

uint8_t On_Off = HIGH;

// EEPROM data
int eeAddress = 0;                    // EEPROM address to start reading from
#define EEPROM_Test 0x33              // Test byte used to check for new Arduino chip

long Change_Pat_Timer;
#define Change_Pattern 20000          // Change pattern every 20 seconds
int Change_Time = Change_Pattern;

void setup() {

  pinMode(Button_Pin, INPUT_PULLUP);

  // Tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // Set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  // Fast LED off
  fadeToBlackBy( leds, NUM_LEDS, 20);  FastLED.show();  // This works very fast and does not hang the 150 pixel string. (leds, num_leds, fadeBy)

  Pressed_Timer = millis();
  Released_Timer = millis();
  Change_Pat_Timer = millis();

  // ***** EEPROM Info ****
  // Test for new chip
  int i = EEPROM.read(eeAddress + 2);
  delay(100);
  if (i != EEPROM_Test) {
    EEPROM.write(eeAddress, 0);
    EEPROM.write((eeAddress + 1), HIGH);
    EEPROM.write((eeAddress + 2), EEPROM_Test);
    Serial.println("EEprom failed - writting new data");
  } // End of eeprom test

  // Read stored values
  On_Off = EEPROM.read(eeAddress + 1);
  Pattern = EEPROM.read(eeAddress);

  delay(1000);

  // Range check
  if ((Pattern < 0) || (Pattern > 9)) {
    EEPROM.write(eeAddress, 0);
    Pattern = 0;
  }

} // End of setup


void loop() {

  // *********  Test push button ***********
  if (digitalRead(Button_Pin) == HIGH) {

    // Reset the timer below
    Pressed_Timer = millis();

    if ((millis() - Released_Timer) > ShortPress) {
      Long_Used = LOW;

      // Test for short press flag and update pattern
      if (Short_Press == HIGH) {
        Short_Press = LOW;

        // Only change pattern if turned on
        if (On_Off == HIGH) {
          Pattern++;
        } // End

        if (Pattern > 9) {
          Pattern = 0;
        } // End of range check

        // Store new pattern
        EEPROM.write(eeAddress, Pattern);

        Change_Time = 2000; // Start changed animation is 2 seconds

      } // End of short press

    } // End of Timer

  } // End of Button release test


  // Test if button pressed, with debounce delay
  if ((digitalRead(Button_Pin) == LOW) && (Long_Used == LOW)) {

    // Reset the timer below
    Released_Timer = millis();

    if ((millis() - Pressed_Timer) > ShortPress) {

      Short_Press = HIGH;

    } // End of Timer

    if ((millis() - Pressed_Timer) > LongPress) {

      Long_Press = HIGH;
      Short_Press = LOW;

    } // End of timer

  } // End of Button press test


  // Test Long press power on/off flag
  if (Long_Press == HIGH) {
    Pressed_Timer = millis();

    Long_Press = LOW;
    Long_Used = HIGH;

    if (On_Off == LOW) {
      On_Off = HIGH;
    }
    else {
      On_Off = LOW;
    }

    EEPROM.write(eeAddress + 1, On_Off);

  } // End of long pressed function


  // Test if on or off

  if (On_Off == HIGH) {
    // On function

    // Which pattern to display
    if (Pattern == 6) {

      PatternShow(Cycle_Pat);

      if ((millis() - Change_Pat_Timer) > Change_Time) {

        Change_Time = Change_Pattern;

        Cycle_Pat++;
        if (Cycle_Pat > 5) {
          Cycle_Pat = 0;
        }

        Change_Pat_Timer = millis();
      }

    } // End of pattern cycle test

    else {

      Cycle_Pat = 6;
      PatternShow(Pattern);
      Change_Pat_Timer = millis();

    } // End of else


    // send the 'leds' array out to the actual LED strip
    FastLED.show();
    // insert a delay to keep the framerate modest
    FastLED.delay(1000 / FRAMES_PER_SECOND);

    // do some periodic updates
    EVERY_N_MILLISECONDS( 20 ) {
      gHue++;  // slowly cycle the "base color" through the rainbow
    }

  } // End of on test

  if (On_Off == LOW) {

    // Off function

    // Fast LED off then signal red/green for status
    fadeToBlackBy( leds, NUM_LEDS, 20);  FastLED.show();  // This works very fast and does not hang the 150 pixel string. (leds, num_leds, fadeBy)

  } // End of off

} // End of Loop


// Which pattern to show
void PatternShow(int i) {
  switch (i) {
    case 0: Rainbow();
      break;
    case 1: RainbowWithGlitter();
      break;
    case 2: Confetti();
      break;
    case 3: Cylon();
      break;
    case 4: Juggle();
      break;
    case 5: Beats();
      break;
    case 6: Marker();
      break;
    case 7: Solid(0xFFA500);  // Orange
      break;
    case 8: Solid(0xF30BC2);  // Purple
      break;
    case 9: Solid(0x00FFFF);  // Cyan
      break;
  } // End switch

} // End of function

void Rainbow()  {
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
} //End of function

void RainbowWithGlitter() {
  // built-in FastLED rainbow, plus some random sparkly glitter
  Rainbow();
  addGlitter(80);
} //End of function

void addGlitter( fract8 chanceOfGlitter)  {
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
} //End of function

void Confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
} //End of function

void Cylon()  {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
} //End of function

void Juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for ( int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
} //End of function

void Beats()  {

  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }

} //End of function

// Requires a 24 bit colour value, i.e. 0x00FF00 or 0xFF0000
void Solid(long m) {
  // See https://github.com/FastLED/FastLED/wiki/Pixel-reference

  for ( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = m;  // write 24 bit colour to LED array
  } // End of loop

} // End of function

// Marker used to signal the auto pattern change mode
void Marker() {
  fadeToBlackBy( leds, NUM_LEDS, 20);
  leds[0] = 0xFFFFFF;
} // End of function
