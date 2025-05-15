/*
    This code has been written for Crystal Wood night light project by Christian Poupounot.
    It handle RBG leds with different modes in order to animate the lamp.

    I choosed a UNO platform to have nice power and USB plugs to show on the back side of the lamp
*/

#include <Arduino.h>

// Mode management
byte mode = 0; // Modes 0=Fade (default) / 1=Red / 2=Green / 3=Blue / 4=Purple / 5=Cyan / 6=Yellow / 7=White
const byte nb_modes = 8;
int btnMode = 0;
int max_brightness = 255;
bool high_brightness = true; // When false only 2 LEDs will be lighted up, when true the 4 LEDs will be lighted up
const int HIGH_BRIGHTNESS_VALUE = 255;
const int LOW_BRIGHTNESS_VALUE = 50;

// Hardware
const bool COMMON_CATHODE   = true; // Depends on LEDs type you have for your make
const int PIN_LED_R1        = 9;
const int PIN_LED_G1        = 10;
const int PIN_LED_B1        = 11;
const int PIN_LED_R2        = 3;
const int PIN_LED_G2        = 5;
const int PIN_LED_B2        = 6;
const int PIN_BUTTON        = 2;

// Color management
enum RGB{
  RED,
  GREEN,
  BLUE,
  NUM_COLORS
};
int _rgbLedValues[] = {255, 0, 0}; // Red, Green, Blue
enum RGB _curFadingUpColor = GREEN;
enum RGB _curFadingDownColor = RED;
int fade_step = 1;  // Speed of color change

// Functionnal variables
int i;

// Functions declaration
void displayColor(byte r, byte g, byte b);
void fadeColor();
void signalBlink(int delay_time,int nb_iteration);

void setup() {
    Serial.begin(9600);
    Serial.println(F(""));
    Serial.println(F("** Crystal Wood **"));
    Serial.println(F("Starting..."));

    pinMode(PIN_BUTTON,INPUT_PULLUP);
    pinMode(PIN_LED_R1, OUTPUT);
    pinMode(PIN_LED_G1, OUTPUT);
    pinMode(PIN_LED_B1, OUTPUT);
    pinMode(PIN_LED_R2, OUTPUT);
    pinMode(PIN_LED_G2, OUTPUT);
    pinMode(PIN_LED_B2, OUTPUT);

    // Test sequence on start to check if there is some burned leds
    high_brightness = true;
    Serial.println(F("- Test Red"));
    for (i=0 ; i <=255 ; i++){
        displayColor(i, 0, 0);
        delay(1);
    }
    for (i=255 ; i >=0 ; i--){
        displayColor(i, 0, 0);
        delay(1);
    }
    Serial.println(F("- Test Green"));
    for (i=0 ; i <=255 ; i++){
        displayColor(0, i, 0);
        delay(1);
    }
    for (i=255 ; i >=0 ; i--){
        displayColor(0, i, 0);
        delay(1);
    }
    Serial.println(F("- Test Blue"));
    for (i=0 ; i <=255 ; i++){
        displayColor(0, 0, i);
        delay(1);
    }
    for (i=255 ; i >=0 ; i--){
        displayColor(0, 0, i);
        delay(1);
    }
    Serial.println(F("End of tests"));
    high_brightness = true;

    // Quick blink to show that tests are passed
    signalBlink(100,2);

    // Set initial fading color
    displayColor(_rgbLedValues[RED], _rgbLedValues[GREEN], _rgbLedValues[BLUE]);
    delay(100);
}

void loop() {
    // Mode button reading
    btnMode = digitalRead(PIN_BUTTON);
    if (btnMode == LOW){
        Serial.print(F("MODE button pressed ! Changing it to "));
        displayColor(0, 0, 0);
        mode++;
        // If we complete a mode cycle we go back to first mode and change brightness setting
        if (mode >= nb_modes){
            mode = 0;
            Serial.println(mode);
            if (!high_brightness){
                high_brightness = true;
                max_brightness = HIGH_BRIGHTNESS_VALUE;
                
                fade_step++;
                if (fade_step == 11){fade_step = 1;}
                Serial.print(F("Changing to high brightness mode and increasing the speed to "));
                Serial.println(fade_step);
            } else {
                high_brightness = false;
                max_brightness = LOW_BRIGHTNESS_VALUE;
                Serial.println(F("Changing to low brightness mode"));
            }
            signalBlink(50,3);
        } else Serial.println(mode);
        
        // Resetting the fading parameters
        if (mode == 0){
            _rgbLedValues[0] = max_brightness;
            _rgbLedValues[1] = 0;
            _rgbLedValues[2] = 0;
            _curFadingUpColor = GREEN;
            _curFadingDownColor = RED;
        }
    }

    // Handling the current mode
    switch(mode){
        case 0: 
            fadeColor();
        break;
        case 1:
            displayColor(max_brightness,0,0); // RED
        break;
        case 2:
            displayColor(0,max_brightness,0); // GREEN
        break;
        case 3:
            displayColor(0,0,max_brightness); // BLUE
        break;
        case 4:
            displayColor(max_brightness,0,max_brightness); // PURPLE
        break;
        case 5:
            displayColor(0,max_brightness,max_brightness); // CYAN
        break;
        case 6:
            displayColor(max_brightness,max_brightness,0); // YELLOW
        break;
        case 7:
            displayColor(max_brightness,max_brightness,max_brightness); // WHITE
        break;
        default:
            displayColor(0,0,0);
            Serial.print(F("Unknow mode detected :/ check program !"));
        break;
    }

    // UI Debouncing and fading delay
    delay(100);  
}

// Signal blink
void signalBlink(int delay_time,int nb_iteration){
    for (int i=1 ; i<=nb_iteration ; i++){
        for(int j=0 ; j<=max_brightness ; j++){
            displayColor(j, j, j);
            // delay(1);
        }
        delay(delay_time);
        for(int j=max_brightness ; j>=0 ; j--){
            displayColor(j, j, j);
            // delay(1);
        }
        delay(delay_time);
    }
    displayColor(0, 0, 0);
}

// Actual display of selected combination of colors
void displayColor(byte redValue, byte greenValue, byte blueValue) {

    if (COMMON_CATHODE){
        // Common cathode
        analogWrite(PIN_LED_R1, redValue);
        analogWrite(PIN_LED_G1, greenValue);
        analogWrite(PIN_LED_B1, blueValue);
        if (high_brightness){
            analogWrite(PIN_LED_R2, redValue);
            analogWrite(PIN_LED_G2, greenValue);
            analogWrite(PIN_LED_B2, blueValue);
        }
    } else {
        // Common anode
        analogWrite(PIN_LED_R1, ~redValue);
        analogWrite(PIN_LED_G1, ~greenValue);
        analogWrite(PIN_LED_B1, ~blueValue);
        if (high_brightness){
            analogWrite(PIN_LED_R2, ~redValue);
            analogWrite(PIN_LED_G2, ~greenValue);
            analogWrite(PIN_LED_B2, ~blueValue);
        }
    }

    if (!high_brightness){
        analogWrite(PIN_LED_R2, 0);
        analogWrite(PIN_LED_G2, 0);
        analogWrite(PIN_LED_B2, 0);
    }
}

// Fading procedure from https://makeabilitylab.github.io/physcomp/arduino/rgb-led-fade.html
void fadeColor() {
    // Increment and decrement the RGB LED values for the current
    // fade up color and the current fade down color
    _rgbLedValues[_curFadingUpColor] += fade_step;
    _rgbLedValues[_curFadingDownColor] -= fade_step;

    // Check to see if we've reached our maximum color value for fading up
    // If so, go to the next fade up color (we go from RED to GREEN to BLUE
    // as specified by the RGB enum)
    // This fade code partially based on: https://gist.github.com/jamesotron/766994
    if(_rgbLedValues[_curFadingUpColor] > max_brightness){
        _rgbLedValues[_curFadingUpColor] = max_brightness;
        _curFadingUpColor = (RGB)((int)_curFadingUpColor + 1);

        if(_curFadingUpColor > (int)BLUE){
            _curFadingUpColor = RED;
        }
    }

    // Check to see if the current LED we are fading down has gotten to zero
    // If so, select the next LED to start fading down (again, we go from RED to 
    // GREEN to BLUE as specified by the RGB enum)
    if(_rgbLedValues[_curFadingDownColor] < 0){
        _rgbLedValues[_curFadingDownColor] = 0;
        _curFadingDownColor = (RGB)((int)_curFadingDownColor + 1);

        if(_curFadingDownColor > (int)BLUE){
            _curFadingDownColor = RED;
        }
    }

    // Set the color and then delay
    displayColor(_rgbLedValues[RED], _rgbLedValues[GREEN], _rgbLedValues[BLUE]);
}
