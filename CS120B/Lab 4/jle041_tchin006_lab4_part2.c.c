/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab #4 Exercise #2
 *	Exercise Description: Buttons are connected to PA0 and PA1. Output for PORTC
 *  is initially 7. Pressing PA0 increments PORTC once (stopping at 9). Pressing
 *  PA1 decrements PORTC once (stopping at 0). If both buttons are depressed (even 
 *  if not initially simultaneously), PORTC resets to 0.
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */


#include <avr/io.h>
enum States {start, increment, decrement, reset} state;
unsigned char tmpC;

void Tick() {
	switch(state) {
		case start:
			if (((PINA & 0x01) == 0x01) && ((PINA & 0x02) == 0x00)) {
				state = increment;
			}
			else if (((PINA & 0x01) == 0x00) && ((PINA & 0x02) == 0x02)) {
				state = decrement;
			}
			else if (((PINA & 0x01) == 0x01) && ((PINA & 0x02) == 0x02)) {
				state = reset;
			}
			break;
		case increment:
			if (((PINA & 0x01) == 0x01) && ((PINA & 0x02) == 0x00)) {
				state = increment;
			}
			else if (((PINA & 0x01) == 0x00) && ((PINA & 0x02) == 0x02)) {
				state = decrement;
			}
			else if (((PINA & 0x01) == 0x01) && ((PINA & 0x02) == 0x02)) {
				state = reset;
			}
			break;
		case decrement:
			if (((PINA & 0x01) == 0x01) && ((PINA & 0x02) == 0x00)) {
				state = increment;
			}
			else if (((PINA & 0x01) == 0x00) && ((PINA & 0x02) == 0x02)) {
				state = decrement;
			}
			else if (((PINA & 0x01) == 0x01) && ((PINA & 0x02) == 0x02)) {
				state = reset;
			}
			break;
		case reset:
			if (((PINA & 0x01) == 0x01) && ((PINA & 0x03) == 0x00)) {
				state = increment;
			}
			else if (((PINA & 0x01) == 0x00) && ((PINA & 0x03) == 0x02)) {
				state = decrement;
			}
			else if (((PINA & 0x01) == 0x01) && ((PINA & 0x03) == 0x02)) {
				state = reset;
			}
			break;
		default:
			break;
	}
	switch(state) {
		case start:
			tmpC = 0x07;
			PORTC = tmpC;
			break;
		case increment:
			if (PORTC < 9) {
				tmpC++;
			}
			PORTC = tmpC;
			break;
		case decrement:
			if (PORTC > 0) {
				tmpC--;
			}
			PORTC = tmpC;
			break;
		case reset:
			tmpC = 0;
			PORTC = tmpC;
			break;
		default:
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // inputs
	DDRC = 0xFF; PORTC = 0x00; // outputs
	PORTC = 0x00;
	state = start;
    while (1) 
    {
		Tick();
    }
}

