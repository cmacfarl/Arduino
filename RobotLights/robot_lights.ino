#include <Adafruit_NeoPixel.h>
#include <Narcoleptic.h>
#include "robolights.h"

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(48, 6, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel longStrip = Adafruit_NeoPixel(60, 6, NEO_GRB + NEO_KHZ800);

static uint32_t teal = strip.Color(24, 167, 181);
static uint32_t purple = strip.Color(139, 0, 204);
static uint32_t red = strip.Color(255, 0, 0);
static uint32_t green = strip.Color(0, 255, 0);
static uint32_t yellow = strip.Color(255, 255, 0);
static uint32_t orange = strip.Color(255, 127, 0);

volatile static bool done = false;

#define PINMODE INPUT_PULLUP

void setup() 
{    
    pinMode(0, PINMODE);
    pinMode(1, PINMODE);
    pinMode(2, PINMODE);
    pinMode(3, PINMODE);
    pinMode(4, PINMODE);
    
    pinMode(9, OUTPUT);
    digitalWrite(9, LOW);
    
    attachInterrupt(0, stateChange, FALLING);
    
    strip.begin();
    strip.setBrightness(70);
    strip.show();
}

void stateChange()
{
    done = true;
}

uint8_t getInput()
{
    return ((digitalRead(4) << 3) | (digitalRead(3) << 2) | (digitalRead(1) << 1) | (digitalRead(0)));
}

void blinkLed()
{
    digitalWrite(9, HIGH);   
    delay(500);               
    digitalWrite(9, LOW);   
    delay(1000); 
}


void loop() 
{
    uint8_t val;
    
    done = false;

    val = getInput();
    
    /*
     * 0000 - Pulse teal
     * 0001 - Forward
     * 0010 - Backward
     * 0011 - Flash yellow
     * 0100 - Flash red
     * 0101 - Rainbow
     *
     * 0110 - Red Left
     * 0111 - Red Right
     * 1000 - Orange Left
     * 1001 - Orange Right
     * 1010 - Yellow Left
     * 1011 - Yellow Right
     * 1100 - Green LeftRight 
     */
    switch (val) {
        case 0:
            pulse(teal);
            break;
        case 1:
            halfAndHalf(green, red);
            break;
        case 2:
            halfAndHalf(red, green);
            break;
        case 3:
            flash(yellow);
            break;
        case 4:
            flash(red);
            break;
        case 5:
            rainbow(20);
            break;
        case 6:
            half(red, LEFT);
            break;
        case 7:
            half(red, RIGHT);
            break;
        case 8:
            half(orange, LEFT);
            break;
        case 9:
            half(orange, RIGHT);
            break;
        case 10:
            half(yellow, LEFT);
            break;
        case 11:
            half(yellow, RIGHT);
            break;
        case 12:
            colorWipe(green, 20, true);
            break;
        default:
            pulse(teal);
    }
}

void half(uint32_t c, direction_t dir)
{
    uint8_t i;
    uint32_t right;
    uint32_t left;
    
    switch (dir) {
        case RIGHT:
            right = c;
            left = 0;
            break;
        case LEFT:
            right = 0;
            left = c;
            break;
    }
    
    for (i = 0; i < strip.numPixels()/2; i++) {
        strip.setPixelColor(i, left);
    }
    
    for (i = strip.numPixels()/2; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, right);
    }
    
    strip.show();
    
    while (!done) {
        delay(20);
    }
}

void flash(uint32_t c)
{
    while (!done) {
        colorStrip(c);
        delay(200);
        colorStrip(0);
        delay(200);
    }
}
    
void pulse(uint32_t c)
{
    uint8_t i;
    uint8_t j;
    
    colorStrip(c);
    
    while (!done) {
        for (j = 0; j < 50; j++) {
            for (i = 70; i > 0; i--) {
                strip.setBrightness(i);
                colorStrip(c);
                strip.show();
                if (done) {
                    return;
                }
                delay(40);
            }
            for (i = 0; i < 70; i++) {
                strip.setBrightness(i);
                colorStrip(c);
                strip.show();
                delay(40);
                if (done) {
                    return;
                }
            }
        }
    }
}

void colorStrip(uint32_t c) 
{
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
    }
    strip.show();
}

void colorWipe(uint32_t c, uint8_t wait, bool pause) 
{
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
        strip.show();
        delay(wait);
    }
    
    if (pause == true) {
        while (!done) {
            delay(20);
        }
    }
}

void halfAndHalf(uint32_t firstHalf, uint32_t secondHalf)
{
    uint8_t i;
    
    for (i = 0; i < strip.numPixels()/4; i++) {
        strip.setPixelColor(i, firstHalf);
    }
    
    for (i = strip.numPixels()/4; i < strip.numPixels()/2 + strip.numPixels()/4; i++) {
        strip.setPixelColor(i, secondHalf);
    }
    
    for (i = strip.numPixels()/2 + strip.numPixels()/4; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, firstHalf);
    }
    
    strip.show();
    
    while (!done) {
        delay(50);
    }
}

/*
 * The following is taken from the Neopixel example software
 * provided by Adafruit.
 */
void rainbow(uint8_t wait) 
{
    uint16_t i, j;

    for (j=0; j<256; j++) {
        for (i=0; i<strip.numPixels(); i++) {
            strip.setPixelColor(i, Wheel((i+j) & 255));
        }
        strip.show();
    }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) 
{
    uint16_t i, j;

    for (j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
        for (i=0; i< strip.numPixels(); i++) {
            strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
        }
        strip.show();
    }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) 
{
    if (WheelPos < 85) {
        return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    } else if (WheelPos < 170) {
        WheelPos -= 85;
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else {
        WheelPos -= 170;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
}

