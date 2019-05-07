/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab # 4 Exercise # 3 
 *	Exercise Description: A household has a digital combination deadbolt lock 
 *  system on the doorway. The system has buttons on a keypad. Button 'X' 
 *  connects to PA0, 'Y' to PA1, and '#' to PA2. Pressing and releasing '#', 
 *  then pressing 'Y', should unlock the door by setting PB0 to 1. Any other
 *  sequence fails to unlock. Pressing a button from inside the house (PA7) locks 
 *  the door (PB0=0). For debugging purposes, give each state a number, and always
 *  write the current state to PORTC (consider using the enum state variable). 
 *  Also, be sure to check that only one button is pressed at a time. 
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
enum States {start, pound1, pound2, y3, unlock} state;

void Tick() {
	switch (state) {
		case start:
			state = pound1;
			PORTB = 0x00;
			break;
		case pound1:
			if ((PINA & 0x04) == 0x04) {
				state = pound2;
			}
			else {
				state = pound1;
			}
			break;
		case pound2:
			if((PINA & 0x04) == 0x00) {
				state = y3;
			}
			else {
				state = pound1;
			}
			break;
		case y3:
			if ((PINA & 0x02) == 0x02) {
				state = unlock;
				PORTB = 0x01;
			}
			else {
				state = pound1;
			}
			break;
		case unlock:
			if ((PINA & 0x80) == 0x80) {
				state = pound1;
				PORTB = 0x00;
			}
			break;
		default:
			state = pound1;
			PORTB = 0x00;
			break;
	}
	switch(state) {
		case start:
			break;
		case pound1:
			PORTC = 0x01;
			break;
		case pound2:
			PORTC = 0x02;
			break;
		case y3:
			PORTC = 0x03;
			break;
		case unlock:
			PORTC = 0x04;
			break;
		default:
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x01;
	DDRC = 0xFF; PORTC = 0xFF;
	PORTB = 0x00;
	state =	start;
	while (1)
	{
		Tick();
	}
}

