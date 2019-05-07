/*	Partner(s) Name & E-mail: Thomas Chin, tchin006@ucr.edu
 *	Lab Section: 024
 *	Assignment: Lab #2  Exercise #3
 *	Exercise Description: Extend the previous program to still write the
 *  available spaces number, but only to PC3..PC0, and to set PC7 to 1 if the
 *  lot is full.
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs
    unsigned char cntavail = 0x00;
	unsigned char tmpA0 = 0x00;
	unsigned char tmpA1 = 0x00;
	unsigned char tmpA2 = 0x00;
	unsigned char tmpA3 = 0x00;
	
    while (1) 
    {
		cntavail = 0x04; // Available spaces is 4 when lot is empty
		tmpA0 = PINA & 0x01;
		tmpA1 = PINA & 0x02;
		tmpA2 = PINA & 0x04;
		tmpA3 = PINA & 0x08;
		
		if (tmpA0) {
			cntavail--;
		}
		if (tmpA1) {
			cntavail--;
		}
		if (tmpA2) {
			cntavail--;
		}
		if (tmpA3) {
			cntavail--;
		}
		
		if (cntavail == 0) {
			PORTC = 0x80;
		}
		else {
			PORTC = cntavail;
		}
    }
}

