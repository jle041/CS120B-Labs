/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab #8 Exercise #1
 *	Exercise Description: Design a system that reads the 10-bit ADC result
 *  from the ADC register, and displays the result on a bank of 10 LEDs.
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
	ADC_init();
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

	unsigned short ADCshort;
	unsigned short max = 0x007B; // from part 2
	
    while (1) 
    {
		ADCshort = ADC; // PA0
		if (ADCshort >= max / 2) {
			PORTB = 0x01;
		}
		else {
			PORTB = 0;
		}
    }
}