/*	Partner(s) Name & E-mail: Thomas Chin, tchin006@ucr.edu
 *	Lab Section: 024
 *	Assignment: Lab #3  Exercise #3
 *	Exercise Description: In addition to the above, PA4 is 1 if a key is in the
 *  ignition, PA5 is one if a driver is seated, and PA6 is 1 if the driver's 
 *  seatbelt is fastened. PC7 should light a "Fasten seatbelt" icon if a key is
 *  in the ignition, the driver is seated, but the belt is not fastened.
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
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs,
	unsigned char tmpC = 0x00; // intermediate variable used for port updates

	while(1)
	{
		tmpC = 0x00;
		
		if ((PORTA & 0x0F) >= 0x01) {
			tmpC = SetBit(tmpC, 5, 1);
		}
		if ((PORTA & 0x0F) >= 0x03) {
			tmpC = SetBit(tmpC, 4, 1);
		}
		if ((PORTA & 0x0F) >= 0x05) {
			tmpC = SetBit(tmpC, 3, 1);
		}
		if ((PORTA & 0x0F) >= 0x07) {
			tmpC = SetBit(tmpC, 2, 1);
		}
		if ((PORTA & 0x0F) >= 0x0A) {
			tmpC = SetBit(tmpC, 1, 1);
		}
		if ((PORTA & 0x0F) >= 0x0D) {
			tmpC = SetBit(tmpC, 0, 1);
		}
		
		// Low fuel light is on if fuel level is 4 or less
		if ((PORTA & 0x0F) <= 0x04) {
			tmpC = SetBit(tmpC, 6, 1);
		}
		
		// If ignition key is in, driver is seated, and seatbelt is NOT fastened
		// PA4 == 1, PA5 == 1, PA6 == 0  -->  PC7 = 0
		if ((GetBit(PORTA, 4) == 1) && (GetBit(PORTA, 5) == 1) && (GetBit(PORTA, 6) == 0)) {
			tmpC = SetBit(tmpC, 7, 1);
		}
		
		PORTC = tmpC;
			
	}
}


