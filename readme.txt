Fast digital I/O functions for Arduino.
Created by Jan Dolinay, Feb. 2014
Works for Arduino Uno and Arduino Mega


Contents of this package:
-------------------------
arduino	- the files to be placed into Arduino folder for the new I/O to be available.
        Place the file into the same subfolders as they are here.
examples - example sketches for Arduino Uno and Mega to test the new digital I/O.


Howto use:
-----------
1. 
    * Copy arduino2.h and digital2.c to [your arduino location]\hardware\arduino\cores\arduino\.
    * Copy pins2_arduino.h to [your arduino location]\hardware\arduino\variants\standard\.
    
    If you use Arduino Mega, copy the pins2_arduino.h file to [your arduino location]\hardware\arduino\variants\mega\.
    Note that the pin2_arduino.h file is different for Arduino standard and Arduino Mega! Chose the proper file for your Arduino variant.

2. Start Arduino IDE and create a new program (sketch) as you normally do. 
You can now use the I/O 2 functions described below instead of the standard
Arduino functions.
 
 
 
Functions which take simple pin number
---------------------------------------

These functions are fully compatible with the original Arduino functions. 
Just add '2' to the names of the original Arduino I/O functions and you are 
using these new functions.

void pinMode2(uint8_t pin, uint8_t mode);
 Set the direction of the pin. Possible options are INPUT, INPUT_PULLUP and OUTPUT.

uint8_t digitalRead2(uint8_t pin);
 Read the value at given pin. The pin should be set as input. Return value is either HIGH or LOW.

void digitalWrite2(uint8_t pin, uint8_t value);
 Set given pin to HIGH or LOW level. The pin should be set as output.


Functions which use pin code
----------------------------
These functions are faster than the previous ones. They differ from the original 
Arduino functions by the type of pin parameter - they use GPIO_pin_t instead of 
uint8_t (int). So in your program, you define the pin as GPIO_pin_t = DP1; 
instead of int pin = 1;.
The other arguments are the same as for standard Arduino functions.

void pinMode2f(GPIO_pin_t pin, uint8_t mode);
uint8_t digitalRead2f(GPIO_pin_t pin);
void digitalWrite2f(GPIO_pin_t pin, uint8_t value);

		

