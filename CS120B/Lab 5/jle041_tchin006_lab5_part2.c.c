/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab #5 Exercise #2
 *	Exercise Description: Buttons are connected to PA0 and PA1. Output for PORTB
 *  is initially 7. Pressing PA0 increments PORTB once (stopping at 9). Pressing
 *  PA1 decrements PORTB once (stopping at 0). If both buttons are depressed (even 
 *  if not initially simultaneously), PORTB resets to 0.
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */


#include <avr/io.h>
enum States {start, wait, pressedAdd, pressedSub, add, sub, reset} state;
unsigned char tmpB = 0x00;
unsigned char addBtn = 0x00;
unsigned char subBtn = 0x00;
	
void Tick() {	
	addBtn = ~PINA & 0x01;
	subBtn = ~PINA & 0x02;
	
	switch(state) {
		case start:
			state = wait;
			break;
		case wait:
			if (addBtn && !subBtn) {
				state = pressedAdd;
			}
			else if (!addBtn && subBtn) {
				state = pressedSub;
			}
			else if (addBtn && subBtn) {
				state = reset;
			}
			else {
				state = wait;
			}
			break;
		case pressedAdd:
			if (!addBtn && !subBtn) {
				state = add;
			}
			else if (addBtn && subBtn) {
				state = reset;
			}
			else {
				state = pressedAdd;
			}
			break;
		case pressedSub:
			if (!addBtn && !subBtn) {
				state = sub;
			}
			else if (addBtn && subBtn) {
				state = reset;
			}
			else {
				state = pressedSub;
			}
			break;
		case add:
			state = wait;
			break;
		case sub:
			state = wait;
			break;
		case reset:
			state = wait;
			break;
		default:
			break;
	}
	switch(state) {
		case start:
			tmpB = 0;
			PORTB = tmpB;
			break;
		case add:
			if (tmpB + 1 <= 9) {
				tmpB++;
			}
			PORTB = tmpB;
			break;
		case sub:
			if (tmpB - 1 >= 0) {
				tmpB--;
			}
			PORTB = tmpB;
			break;
		case reset:
			tmpB = 0;
			PORTB = tmpB;
			break;
		default:
			break;
	}
	
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // inputs
	DDRB = 0xFF; PORTB = 0x00; // outputs
	state = start;
    while (1) 
    {
		Tick();
    }
}

