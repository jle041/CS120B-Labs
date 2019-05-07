/*	Partner(s) Name & E-mail: Thomas Chin, tchin006@ucr.edu
 *	Lab Section: 024
 *	Assignment: Lab #3  Exercise #4
 *	Exercise Description:A car's passenger-seat weight sensor outputs a 9-bit 
 *  value (ranging from 0 to 511) and connects to input PD7..PD0PB0 on the 
 *  microcontroller. If the weight is equal to or above 70 pounds, the airbag 
 *  should be enabled by setting PB1 to 1. If the weight is above 5 but below 70,
 *  the airbag should be disabled and an "Airbag disabled" icon should light by 
 *  setting PB2 to 1. (Neither B1 nor B2 should be set if the weight is 5 or less, 
 *  as there is no passenger). 
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
	DDRB = 0x06; PORTB = 0x00; // Configure PB1 and PB2 as outputs, and PB0 as an input
	DDRD = 0x00; PORTD = 0xFF; // Configure port D's 8 pins as inputs
	unsigned char totalWeight = 0;
	
	while (1)
	{
		//Combine ports to create 9-bit total weight
		totalWeight = 0x000;
		totalWeight = PORTD;
		totalWeight = totalWeight << 1;
		totalWeight = totalWeight + GetBit(PORTB, 0);
		
		//Compare totalWeight
		if (totalWeight >= 70) {
			PORTB = SetBit(PORTB, 1, 1);
		}
		else {
			PORTB = SetBit(PORTB, 1, 0);
		}
		
		if (totalWeight < 70 && totalWeight > 5) {
			PORTB = SetBit(PORTB, 2, 1);
		}
		else {
			PORTB = SetBit(PORTB, 2, 0);
		}
	}
}


