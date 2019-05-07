/*	Partner(s) Name & E-mail: Thomas Chin, tchin006@ucr.edu
 *	Lab Section: 024
 *	Assignment: Lab #5  Exercise #1
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


int main(void)
{
	unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
		return (b ? x | (0x01 << k) : x & ~(0x01 << k));
	}
	unsigned char GetBit(unsigned char x, unsigned char k) {
		return ((x & (0x01 << k)) != 0);
	}

	/* Replace with your application code */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	unsigned char tempA = 0x00;
	unsigned char tempB = 0x00;
	
	while(1) {
		tempA = ~PINA;
		
		if(tempA == 0x01 || tempA == 0x02)
		{
			tempB = 0x60; //Includes port6
		}
		else if (tempA == 0x03 || tempA == 0x04)
		{
			tempB = 0x70; //Includes port6
		}
		else if (tempA == 0x05 || tempA == 0x06)
		{
			tempB = 0x38;
		}
		else if (tempA == 0x07 || tempA == 0x08 || tempA == 0x09)
		{
			tempB = 0x3C;
		}
		else if (tempA == 0x0A || tempA == 0x0B || tempA == 0x0C)
		{
			tempB = 0x3E;
		}
		else if (tempA == 0x0D || tempA == 0x0E || tempA == 0x0F)
		{
			tempB = 0x3F;
		}
		else if (tempA == 0x00) {
			tempB = 0x40;
		}
		PORTB = tempB;
	}
}


