/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab #8 Exercise #4
 *	Exercise Description: Design a system, using a bank of eight LEDs, where
 *  the number of LEDs illuminated is a representation of how much light is
 *  detected. For example, when more light is detected, more LEDs are illuminated.
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */


#include <avr/io.h>
void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

	unsigned short ADCshort;
	unsigned short max = 0xCF; // (7B) from part 2
	unsigned short min = 0x27; // (30) from part 2
	unsigned short increment = (max - min) / 8;
	ADC_init();
    while (1) 
    {
		ADCshort = ADC; // PA0
		
		if (ADCshort < (min + increment)) {
			PORTB = 0;
		}
		else if ((ADCshort >= (min + increment)) && (ADCshort < (min + (2 * increment)))) {
			PORTB = 0x01;
		}
		else if ((ADCshort >= (min + (2 * increment))) && (ADCshort < (min + (3 * increment)))) {
			PORTB = 0x03;
		}
		else if ((ADCshort >= (min + (3 * increment))) && (ADCshort < (min + (4 * increment)))) {
			PORTB = 0x07;
		}
		else if ((ADCshort < (min + (5 * increment))) && (ADCshort >= (min + (4 * increment)))) {
			PORTB = 0x0F;
		}
		else if ((ADCshort >= (min + (5 * increment))) && (ADCshort < (min + (6 * increment)))) {
			PORTB = 0x1F;
		}
		else if ((ADCshort >= (min + (6 * increment))) && (ADCshort < (min + (7 * increment)))) {
			PORTB = 0x3F;
		}
		else if ((ADCshort >= (min + (7 * increment))) && (ADCshort < (min + (8 * increment)))) {
			PORTB = 0x7F;
		}
		else {
			PORTB = 0xFF;
		}
		
    }
}
