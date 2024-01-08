#include <avr/io.h>
#include <util/delay.h>
#include <u8g2.h>
#include <u8x8_avr.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define SSD1306_ADDR 0x78

volatile uint16_t startTime;
volatile uint16_t endTime;
volatile uint16_t period;

volatile float d;

u8g2_t u8g2;

// To draw the square wave
void drawMultiplePWMWaveforms(float dutyCycle, uint8_t numCycles)
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2, u8g2_font_5x8_tf);
	u8g2_SetFontRefHeightText(&u8g2);
	u8g2_SetFontPosTop(&u8g2);

	// Calculate the width of each cycle
	float cycleWidth = 128.0 / numCycles;

	for (uint8_t i = 0; i < numCycles; ++i)
	{
		// Define the start of each rising and falling edge
		float startX = i * cycleWidth;
		float endX = (i + 1) * cycleWidth;

		// Draw the vertical line
		u8g2_DrawLine(&u8g2, startX, 30, startX, 60);

		// Draw the first horizontal line
		u8g2_DrawLine(&u8g2, startX + 1, 30, startX + 1 + (dutyCycle * cycleWidth / 100.0), 30);

		// Draw the second vertical line
		u8g2_DrawLine(&u8g2, startX + 1 + (dutyCycle * cycleWidth / 100.0), 30, startX + 1 + (dutyCycle * cycleWidth / 100.0), 60);

		// Draw the second horizontal line
		u8g2_DrawLine(&u8g2, startX + 1 + (dutyCycle * cycleWidth / 100.0), 60, endX, 60);
	}

	u8g2_SendBuffer(&u8g2);
}


int main()
{
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_avr_hw_i2c, u8x8_avr_delay);
	u8g2_SetI2CAddress(&u8g2, SSD1306_ADDR);
	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);
	
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2, u8g2_font_6x12_mf);
	u8g2_SetFontRefHeightText(&u8g2);
	u8g2_SetFontPosTop(&u8g2);
	
	unsigned int a, b, c, high, period;
	char frequency[14], duty_cy[7], per[10];

	PORTD = 0xFF; 

	while (1)
	{
		TCCR1A = 0;
		TCNT1 = 0;
		TIFR1 = (1 << ICF1); /* Clear ICF (Input Capture flag) flag */

		TCCR1B = (1 << ICES1) | (1 << CS10) | (1 << CS11); /* Rising edge, 64 prescaler */
		while ((TIFR1 & (1 << ICF1)) == 0);
		a = ICR1; /* Take value of capture register */
		TIFR1 = (1 << ICF1); /* Clear ICF flag */

		TCCR1B = (1 << CS10) | (1 << CS11); /* Falling edge, 64 prescaler */
		while ((TIFR1 & (1 << ICF1)) == 0);
		b = ICR1; /* Take value of capture register */
		TIFR1 = (1 << ICF1); /* Clear ICF flag */

		TCCR1B = (1 << ICES1) | (1 << CS10) | (1 << CS11); /* Rising edge, 64 prescaler */
		while ((TIFR1 & (1 << ICF1)) == 0);
		c = ICR1; /* Take value of capture register */
		TIFR1 = (1 << ICF1); /* Clear ICF flag */

		//TCCR1B = 0; /* Stop the timer */

		high = b - a;
		period = c - a;

		long freq = F_CPU / period; /* Calculate frequency */

		/* Calculate duty cycle */
		float duty_cycle = ((float)high / (float)period) * 100.0;

		itoa((int)duty_cycle, duty_cy, 10);
		ltoa(freq, frequency, 10);
		ltoa(period, per, 10);
		
		drawMultiplePWMWaveforms(duty_cycle, 5);
		
		u8g2_DrawStr(&u8g2, 0, 0, "F: ");
		u8g2_DrawStr(&u8g2, 75, 0, "DC: ");
		u8g2_DrawStr(&u8g2, 0, 10, "P: ");
				
		u8g2_DrawStr(&u8g2, 95, 0, duty_cy);
		u8g2_DrawStr(&u8g2, 15, 0, frequency);
		u8g2_DrawStr(&u8g2, 15, 10, per);
		u8g2_SendBuffer(&u8g2);
	}
}


