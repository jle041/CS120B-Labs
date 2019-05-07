/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab #8 Exercise #2
 *	Exercise Description: Revise Part 1 by replacing the potentiometer with a
 *  photoresistor and 330 ohm resistor. Take note of the highest ADC value
 *  displayed when the photoresistor is exposed to light, and the lowest ADC
 *  value displayed when the photoresistor is deprived of all light. These
 *  values will be used for the remaining lab exercises.
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
	DDRD = 0xFF; PORTD = 0x00;

	unsigned short ADCshort;
	unsigned char tmpB;
	unsigned char tmpD;
	ADC_init();
    while (1) 
    {
		ADCshort = ADC; // PA0
		tmpB = (char)ADCshort;
		tmpD = (char)(ADCshort >> 8);
// 		tmpB = ADCshort & 0x00FF;
// 		tmpD = (ADCshort & 0x0300) >> 8;
		PORTB = tmpB;
		PORTD = tmpD;
    }
}



