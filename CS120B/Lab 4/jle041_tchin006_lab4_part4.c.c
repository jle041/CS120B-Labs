/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab # 4 Exercise # 4 
 *	Exercise Description: (Challenge) Extend the above door so that it can also
 *  be locked by entering the earlier code. 
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
enum States {start, pound1, pound2, y3, unlock, locked} state;
unsigned char lockCode = 0x00;

void Tick() {
	switch (state) {
		case start:
			state = pound1;
			PORTB = 0x00;
			break;
		case pound1:
			if ((PINA & 0x04) == 0x04) {
				state = pound2;
				break;
			}
			else if ((PINA & 0x04) != 0x04 && lockCode == 0x01) {
				state = unlock;
				break;
			}
			else {
				state = pound1;
				break;
			}
		case pound2:
			if((PINA & 0x04) == 0x00 && lockCode == 0x01) {
				state = y3;
				break;
			}
			else if ((PINA & 0x04) != 0x00 && lockCode == 0x01) {
				state = unlock;
				break;
			}		
			else if ((PINA & 0x04) == 0x00 && lockCode == 0x00) {
				state = y3;
				break;
			}
			else {
				state = pound1;
				break;
			}
		case y3:
			if ((PINA & 0x02) == 0x02 && lockCode == 0x01) {
				state = locked;
				break;
			}
			else if ((PINA & 0x02) != 0x02 && lockCode == 0x01) {
				state = unlock;
				break;
			}		
			else if ((PINA & 0x02) == 0x02 && lockCode == 0x00) {
				state = unlock;
				PORTB = 0x01;
				break;
			}
			else {
				state = pound1;
				break;
			}		
		case unlock:
			if ((PINA & 0x80) == 0x80) {
				state = locked;
				break;
			}
			else if ((PINA & 0x80) == 0x80 && lockCode == 0x01) {
				state = locked;
				break;
			}
			else if ((PINA & 0x04) == 0x04) {
				lockCode = 0x01;
				state = pound2;
				break;
			}
			else {
				break;
			}		
		case locked:
			state = pound1;
			PORTB = 0x00;
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
		case locked:
			PORTC = 0x05;
			break;
		default:
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // inputs
	DDRB = 0xFF; PORTB = 0x01; // outputs
	DDRC = 0xFF; PORTC = 0xFF; // outputs
	PORTB = 0x00;
	state =	start;
	while (1)
	{
		Tick();
	}
}