/*	Partner(s) Name & E-mail: Thomas Chin, tchin006@ucr.edu
 *	Lab Section: 024
 *	Assignment: Lab #3  Exercise #4
 *	Exercise Description: Read an 8-bit value on PA7..PA0 and write that value 
 *  on PB3..PB0PC7..PC4. That is to say,  take the upper nibble of PINA and map
 *  it to the lower nibble of PORTB, likewise take the lower nibble of PINA and
 *  map it to the upper nibble of PORTC (PA7 -> PB3, PA6 -> PB2, … PA1 -> PC5, PA0 -> PC4).
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

// Bit-access function
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0x00; PORTB = 0x00; // Configure port B's 8 pins as outputs
	DDRC = 0x00; PORTC = 0x00; // Configure port C's 8 pins as outputs
	
	/* Replace with your application code */
	while (1)
	{
		PORTB = PINA & 0xF0;
		PORTB = PORTB >> 4;
		
		PORTC = PINA & 0x0F;
		PORTC = PORTC << 4;
	}
}


