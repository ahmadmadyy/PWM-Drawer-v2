#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "ADC.h"

// Initialize ADC
void adc_init() 
{
	// Set reference voltage to AVCC, and select ADC3 as the default input channel
	ADMUX |= (1 << REFS0) | (1 << MUX0) | (1 << MUX1);
	
	// Enable ADC, and set the prescaler to 128
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

// Perform ADC conversion and return the result
uint16_t adc_read() 
{
	// Start the conversion
	ADCSRA |= (1 << ADSC);

	// Wait for the conversion to complete
	while (ADCSRA & (1 << ADSC));

	// Return the ADC result
	return ADC;
}
