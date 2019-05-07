/*	Partner(s) Name & E-mail: Thomas Chin, tchin006@ucr.edu
 *	Lab Section: 024
 *	Assignment: Lab #3  Exercise #2
 *	Exercise Description: A car has a fuel-level sensor that sets PA3..PA0 to
 *  a value between 0 (empty) and 15 (full). A series of LEDs connected to 
 *  PC5..PC0 should light to graphically indicate the fuel level. If the fuel 
 *  level is 1 or 2, PC5 lights. If the level is 3 or 4, PC5 and PC4 light. 
 *  Level 5-6 lights PC5..PC3. 7-9 lights PC5..PC2. 10-12 lights PC5..PC1. 
 *  13-15 lights PC5..PC0. Also, PC6 connects to a "Low fuel" icon, which should 
 *  light if the level is 4 or less. 
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
	DDRA = 0x00; PORTA = 0x0F; // Configure port A's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs,
	unsigned char tmpC = 0x00; // intermediate variable used for port updates

	while(1)
	{
		tmpC = 0x00;
		
		if (PORTA >= 0x01) {
			tmpC = SetBit(tmpC, 5, 1);
		}
		if (PORTA >= 0x03) {
			tmpC = SetBit(tmpC, 4, 1);
		}
		if (PORTA >= 0x05) {
			tmpC = SetBit(tmpC, 3, 1);
		}
		if (PORTA >= 0x07) {
			tmpC = SetBit(tmpC, 2, 1);
		}
		if (PORTA >= 0x0A) {
			tmpC = SetBit(tmpC, 1, 1);
		}
		if (PORTA >= 0x0D) {
			tmpC = SetBit(tmpC, 0, 1);
		}
		
		if (PORTA <= 0x04) {
			tmpC = SetBit(tmpC, 6, 1);
		}
		
		PORTC = tmpC;
			
	}
}


