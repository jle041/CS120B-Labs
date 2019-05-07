/*	Partner(s) Name & E-mail: Thomas Chin, tchin006@ucr.edu
 *	Lab Section: 024
 *	Assignment: Lab #3  Exercise #1 
 *	Exercise Description: Count the number of 1s on ports A and B and output that number on port C. 
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
	DDRB = 0x00; PORTB = 0xFF; // Configure port B's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs,
	// initialize to 0s
	unsigned char counter = 0x00; // intermediate variable used for port updates

	while(1)
	{
		counter = 0x00;
		
		for (int i = 0; i < 8; i++) {
			if (GetBit(PINA, i) == 1) {
				counter++;
			}
			if (GetBit(PINB, i) == 1) {
				counter++;
			}
		}
		
		PORTC = counter;
	}
}


