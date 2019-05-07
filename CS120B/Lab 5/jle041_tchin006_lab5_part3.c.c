/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab #5 Exercise #3
 *	Exercise Description: (Challenge) Create your own festive lights display
 *  with 6 LEDs connected to port PB5..PB0, lighting in some attractive sequence. 
 *  Pressing the button on PA0 changes the lights to the next configuration in
 *  the sequence.  Use a state machine (not synchronous) captured in C. 
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */ 

#include <avr/io.h>

enum States {start, release, s1, s2, s3, all} state;

unsigned char tmpB = 0x00;
unsigned char button = 0x00;
void Tick() {
	button = ~PINA & 0x01;
	
	switch(state) {
		case start:
			if (button) {
				state = release;
			}
			break;
		case s1:
			if (button) {
				state = release;
			}
			break;
		case s2:
			if (button) {
				state = release;
			}
			break;
		case s3:
			if (button) {
				state = release;
			}
			break;
		case all:
			if (button) {
				state = release;
			}
			break;
		case release:
			if (!button) {
				if (tmpB == 8) {
					state = s1;
				}
				else if (tmpB == 20) {
					state = s2;
				}
				else if (tmpB == 34) {
					state = s3;
				}
				else if (tmpB == 65) {
					state = all;
				}
				else if (tmpB == 127) {
					state = start;
				}
			}
			else {
				state = release;
			}
			break;
		default:
			break;
	}
	switch(state) {
		case start:
			tmpB = 8;
			PORTB = tmpB;
			break;
		case s1:
			tmpB = 20;
			PORTB = tmpB;
			break;
		case s2:
			tmpB = 34;
			PORTB = tmpB;
			break;
		case s3:
			tmpB = 65;
			PORTB = tmpB;
			break;
		case all:
			tmpB = 127;
			PORTB = tmpB;
			break;
		default:
			break;
	}
}

int main(void)
{
    DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	state = start;
    while (1) 
    {	
		Tick();
    }
}

