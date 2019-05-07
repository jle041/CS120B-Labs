/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab #11  Exercise #4
 *	Exercise Description: An example behavior would be to initially display a
 *  lengthy string, such as “Congratulations!”. The first keypad button pressed
 *  changes the first character ‘C’ to the button pressed. The second keypad
 *  press changes the second character to the second button pressed, etc. No
 *  refresh should be observable during the character update.
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include "timer.h"
#include "keypad.h"
#include "io.c"

enum States {start, run} state;
unsigned char keypadInput;
unsigned char cursor;
void Tick() {
	switch(state) {
		case start:
			state = run;
			break;
		case run:
			state = run;
			break;
		default:
			state = start;
			break;
	}
	switch(state) {
		case start:
			cursor = 1;
			break;
		case run:
			keypadInput = GetKeypadKey();
			if (keypadInput != '\0') {
				LCD_WriteData(keypadInput);
				if (cursor == 17) {
					cursor = 1;
				}
				LCD_Cursor(cursor);
				cursor++;
			}
			break;
		default:
			break;
	}
}

int main(void)
{
    
	DDRA = 0xFF; PORTA = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	DDRC = 0xF0; PORTC = 0x0F;
	
	TimerOn();
	TimerSet(200);
	
	LCD_init();
	LCD_ClearScreen();
	LCD_DisplayString(1, "Congratulations!");

	state = start;
    while (1) {
		Tick();
		while (!TimerFlag) {};
		TimerFlag = 0;
    }
}

