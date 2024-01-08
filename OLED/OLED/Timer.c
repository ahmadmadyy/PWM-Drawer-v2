#include <avr/io.h>
#include "Timer.h"
#include <avr/interrupt.h>

void initPWM()
{
	// Set OC0B (Pin 5, PD5) as output
	DDRD |= (1 << PD5);

	// Set prescaler to 8
	TCCR0B = (1 << CS02) | (1 << CS01) | (0 << CS00);

	// Enable Phase Correct PWM mode, non-inverting output on OC0B
	TCCR0A |= (1 << WGM01) | (1 << WGM00) | (0 << COM0B0) | (1 << COM0B1);

	// Initial duty cycle = 0%
	OCR0B = 0;
}

void setPWMDutyCycle(uint16_t value)
{
	// Calculate the corresponding PWM value for the given duty cycle
	uint16_t pwmValue = (255.0 / 8192.0) * (uint16_t)value;

	// Set the OCR0B register with the calculated PWM value
	OCR0B = (uint8_t)pwmValue;
}


