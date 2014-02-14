/*
 * arduino2.h
 *
 *  Created on: 11. 2. 2014
 *      Author: Jan Dolinay
 *
 *   Alternate version of digital input/output for Arduino.
 *   This is common file included to user application.
 *   It includes the pins2_arduino.h with definitions specific to given
 *   Arduino variant (e.g. standard or mega).
 */

#ifndef ARDUINO2_H_
#define ARDUINO2_H_




// Standard headers for AVR
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "arduino.h"


#ifdef __cplusplus
extern "C"{
#endif
// Definitions specific for selected board
#include "pins2_arduino.h"




// ===========================================================================
// Prototypes of digital I/O functions
// ===========================================================================
// These functions are defined in pin2_arduino.h
// The definitions could probably be common for more Arduino variants, but
// it seems easier to have separate definitions for now.
// Note: In standard Arduino these functions are implemented in
// hardware\arduino\cores\arduino\wiring_digital.c


// Arduino compatible functions (slower, but will take integer as pin)
// The pin is simple integer ( 0 thru 19).
// Note: these functions are never compiled into single instruction, even with cont pin.
static inline void pinMode2(uint8_t, uint8_t);
static inline void digitalWrite2(uint8_t, uint8_t);
static inline uint8_t digitalRead2(uint8_t);



// New version of functions which take pin code as input
static inline void pinMode2f(GPIO_pin_t pin, uint8_t mode );
static inline uint8_t digitalRead2f(GPIO_pin_t pin);
static inline void digitalWrite2f(GPIO_pin_t pin, uint8_t value);

// ===========================================================================
//

// Note: the ARDUINO2_IOREGS_ABOVEFF is defined in pins2_arduino.h
#if ARDUINO2_PREFER_SPEED
	#define		ARDUINO2_USE_INLINE_FUNCTIONS	1
#else
	#define		ARDUINO2_USE_INLINE_FUNCTIONS	0
#endif

// Note: the ARDUINO2_IOREGS_ABOVEFF is defined in pins2_arduino.h
#if ARDUINO2_IOREGS_ABOVEFF
	#define	ARDUINO2_ATOMIC		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
#else
	#define	ARDUINO2_ATOMIC
#endif


// Prototypes
// Internal use only! Include Arduino2.h in your program and use functions found there.
void internal_pinMode2(GPIO_pin_t pin, uint8_t mode);		// implemented in digital2.c
uint8_t internal_digitalRead2(GPIO_pin_t pin);				// implemented in digital2.c
void internal_digitalWrite2(GPIO_pin_t pin, uint8_t value);	// implemented in digital2.c
static inline GPIO_pin_t Arduino_to_GPIO_pin(uint8_t pin);

//
// pinMode2
//
static inline void pinMode2(uint8_t, uint8_t) __attribute__((always_inline, unused));
static inline
void pinMode2(uint8_t pin, uint8_t mode )
{
	pinMode2f(Arduino_to_GPIO_pin(pin), mode);
}

//
// digitalRead2
// Size: 58 B with pin as a variable; 34 B with pin as a constant.
//
//static inline uint8_t digitalRead2(uint8_t) __attribute__((always_inline, unused));
static inline uint8_t digitalRead2(uint8_t) __attribute__((unused));
static inline
uint8_t digitalRead2(uint8_t pin)
{
	return digitalRead2f(Arduino_to_GPIO_pin(pin));
}

//
// digitalWrite2
//
static inline void digitalWrite2(uint8_t, uint8_t) __attribute__((always_inline, unused));
static inline
void digitalWrite2(uint8_t pin, uint8_t value)
{
	digitalWrite2f(Arduino_to_GPIO_pin(pin), value);
}


// =======================================
// New, fast functions
// The pin is one of the values defined in GPIO_pin_t, e.g. DP1 for digital pin 1, etc.
// Note: these functions can be compiled into single instruction if called with const pin
// and value/mode.
// ========================================

//
// pinMode2
//
// Set the pin to input, input with pull-up resistor or output mode.
// Note: There may be applications which require fast switching between input and
// output mode, so it is good to offer fast version for pins known at compile-time
// (where the compiler will perform "constant-folding" and we can get single instruction)
//
// Size: 2 B (single instruction) with const pin
//		24 B with pin as a variable and inline code
//		94 B with pin as a variable and non-inline code
//  Note: the size for inline variant is probably much smaller than the called variant
//	because the compiler can evaluate the "mode" in compile-time and generate code
//  just for the mode used (e.g. output), while for called variant
//	it has to generate the code for all possible values of "mode".
//
static inline void pinMode2f(GPIO_pin_t, uint8_t ) __attribute__((always_inline, unused));
static inline
void pinMode2f(GPIO_pin_t pin, uint8_t mode )
{

	 if (__builtin_constant_p(pin) && __builtin_constant_p(mode))
	 {
		 // ARDUINO2_ATOMIC expands to nothing when not needed (atmega328)
		 // and to ATOMIC_BLOCK when needed (atmega2560)
		 ARDUINO2_ATOMIC
		 {
			 // fast version which results in single instruction
			 if ( mode == OUTPUT )
			 {
					GPIO_DDR_REG(pin) |= GPIO_PIN_MASK(pin);
			 }
			 else
			 {
				if ( mode == INPUT_PULLUP )
				{
					GPIO_DDR_REG(pin) &= ~GPIO_PIN_MASK(pin);
					GPIO_PORT_REG(pin) |= GPIO_PIN_MASK(pin);
				}
				else
				{
					// input mode without pull-up
					GPIO_DDR_REG(pin) &= ~GPIO_PIN_MASK(pin);
					GPIO_PORT_REG(pin) &= ~GPIO_PIN_MASK(pin);
				}
			 }
		 }
	 }
	 else
	 {

#if	ARDUINO2_USE_INLINE_FUNCTIONS

		if ( mode == OUTPUT )
		{
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				GPIO_DDR_REG(pin) |= GPIO_PIN_MASK(pin);
			}
		}
		else
		{
			if ( mode == INPUT_PULLUP )
			{
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
				{
					GPIO_DDR_REG(pin) &= ~GPIO_PIN_MASK(pin);
					GPIO_PORT_REG(pin) |= GPIO_PIN_MASK(pin);
				}
			}
			else
			{
				// input mode without pull-up
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
				{
					GPIO_DDR_REG(pin) &= ~GPIO_PIN_MASK(pin);
					GPIO_PORT_REG(pin) &= ~GPIO_PIN_MASK(pin);
				}
			}
		}

#else
		 // Call non-inline version
		 internal_pinMode2(pin, mode);
#endif

	 }
}

//
// digitalRead2f
//
// Read value from given digital pin.
// The pin must be set to input mode using pinMode2f.
//
// Size: 8 B with const pin.
//		30 B with pin as a variable and non-inline worker
//		32 B with pin as a variable and inline code
//
static inline uint8_t digitalRead2f(GPIO_pin_t) __attribute__((always_inline, unused));
static inline
uint8_t digitalRead2f(GPIO_pin_t pin)
{
	// There is no need to disable interrupts for reading pin value, so
	// we can use the same code both for pins known and unknown at compile-time.
	// Unless we would want to save little program memory and make the
	// version with non-const pin non-inline.
	if (__builtin_constant_p(pin) )
	{
		if ((GPIO_PIN_REG(pin) & GPIO_PIN_MASK(pin)) != 0)
			return HIGH;
		else
			return LOW;
	}
	else
	{

#if	ARDUINO2_USE_INLINE_FUNCTIONS

		if ((GPIO_PIN_REG(pin) & GPIO_PIN_MASK(pin)) != 0)
			return HIGH;
		else
			return LOW;

#else
		return internal_digitalRead2(pin);
#endif

	}
}


//
// digitalWrite2f
// Size: 2 B with const pin (single SBI instruction). (Atmega328)
//		42 B (including rjmp) with pin as a variable and non-inline worker
//		24 B with pin as a variable and inline code
//  Note: the size for inline variant is probably  smaller than the called variant
//	because the compiler can evaluate the "value" in compile-time and generate code
//  just for the value used (e.g. HIGH), while for called variant
//	it has to generate the code for both HIGH and LOW values.
//
//
// NOTE: If we do not use inline, we get function with 32 bytes of code; the execution
// time is about 2,15 micro seconds in this case; 1.35 micro-seconds for inline version.
// BUT we need to define inline to be able to use the __builtin_constant_p
//
static inline void digitalWrite2f(GPIO_pin_t, uint8_t) __attribute__((always_inline, unused));
static inline
void digitalWrite2f(GPIO_pin_t pin, uint8_t value)
{
	 // If we know the pin number and value at compile-time, the compiler
	 // will compute everything during compilation and resulting code will
	 // be single instruction - which does not need disabled interrupts
	 // Note: we could place the "else" code into separate non-inline function to save
	 // same program memory, but function call adds about 12 clock cycles (0.75 us)
	 // while the code is only 32 B.
	 if (__builtin_constant_p(pin) && __builtin_constant_p(value))
	 {
		 // ARDUINO2_ATOMIC expands to nothing when not needed (atmega328)
		 // and to ATOMIC_BLOCK when needed (atmega2560)
		 ARDUINO2_ATOMIC
		 {
			 if ( value == 0 )
				 GPIO_PORT_REG(pin) &= ~GPIO_PIN_MASK(pin);
			 else
				 GPIO_PORT_REG(pin) |= GPIO_PIN_MASK(pin);
		 }
	 }
	 else
	 {
		 // If pin or value must be computed in runtime, resulting code
		 // will always require disabled interrupts.
#if ARDUINO2_USE_INLINE_FUNCTIONS

		 // NOTE: if you make change here, make it also in internal_digitalWrite2()!
		 if ( value == 0 )
		 {
			 ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			 {
			 	 GPIO_PORT_REG(pin) &= ~GPIO_PIN_MASK(pin);
			 }
		 }
		 else
		 {
			 ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			 {
				 GPIO_PORT_REG(pin) |= GPIO_PIN_MASK(pin);
			 }
		 }

#else
		 internal_digitalWrite2(pin, value);
#endif
	 }

	 // NOTE: Version of the code with macros GPIO_PORT_REG etc. placed directly
	 // in the code results in the same code after compilation as if we use local
	 // variables to store the mask and register address.
}

// Macro which obtains pin code from Arduino-style pin number
// This is used internally by inline function which adds range check.
#define		GPIO_GET_PINDEF(pin)  (GPIO_pin_t)pgm_read_word(gpio_pins_progmem + (pin))


// Arduino_to_GPIO_pin
// Convert Arduino-style pin number (0 thru N) to pin code used by the fast functions
//
// Note: If we use big switch, the compiler can evaluate this in compile-time
// for const pin numbers, which results in single instruction digitalRead/Write.
// It cannot do so if we use an array.
// BUT the compiler will only do the single instruction version if the function is
// declared always_inline. Otherwise it will generate normal call even for const pin.
// BUT always_inline function will be inlined for all calls, making the program
// very big.
// The solution used e.g. in digitalWrite2f, with __builtin_constant_p(pin), seems not nice
// we would need 2 copies of the same code, one directly in this function for inlining
// and one in helper, non-inline function.
// Ideal solution would be normal, non-inline function which the compiler would
// automatically inline (make into single instruction) for const input. Since this seem impossible
// to achieve for me now, I use the array version.
static inline GPIO_pin_t Arduino_to_GPIO_pin(uint8_t) __attribute__((always_inline, unused));
static inline
GPIO_pin_t Arduino_to_GPIO_pin(uint8_t pin)
{

	if ( pin < GPIO_PINS_NUMBER )
		return GPIO_GET_PINDEF(pin);
	else
		return DP_INVALID;

}






#ifdef __cplusplus
} // extern "C"
#endif

#endif /* ARDUINO2_H_ */
