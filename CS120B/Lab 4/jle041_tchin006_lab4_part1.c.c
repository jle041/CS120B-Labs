/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab #4  Exercise #1 
 *	Exercise Description: PB0 and PB1 each connect to an LED, and PB0's LED is
 *  initially on. Pressing a button connected to PA0 turns off PB0's LED and 
 *  turns on PB1's LED, staying that way after button release. Pressing the button
 *  again turns off PB1's LED and turns on PB0's LED. 
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

enum States {start, s0, s1} state;

void Tick() {
	// Transitions
	switch (state) {
		case start:
			state = s0;
			break;
		case s0:
			if (PINA == 0x01) {
				state = s1;	
			}
			break;
		case s1:
			if (PINA == 0x01) {
				state = s0;
			}
			break;
	}
	// Actions
	switch (state) {
		case start:
			break;
		case s0:
			PORTB = 0x01;
			break;
		case s1:
			PORTB = 0x02;
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // PORTA is input (only A0 is needed)
    DDRB = 0xFF; PORTB = 0x00; // B0 and B1 are outputs
	PORTB = 0x00;
	state = start;
    while (1) 
    {
		Tick();
    }
}

