/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab #11  Exercise #2
 *	Exercise Description: Use the LCD code, along with a button and/or time
 *  delay to display the message "CS120B is Legend... wait for it DARY!" The
 *  string will not fit on the display all at once, so you will need to come
 *  up with some way to paginate or scroll the text.
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include "io.c"
#include <avr/interrupt.h>
#include "timer.h"

enum States {start, run} state;
unsigned char i, j, k;
char message[69] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'C', 'S', '1', '2', '0', 'B', ' ', 'i', 's', ' ', 'L', 'e', 'g', 'e', 'n', 'd', '.', '.', '.', ' ', 'w', 'a', 'i', 't', ' ', 'f', 'o', 'r', ' ', 'i', 't', ' ', 'D', 'A', 'R', 'Y', '!', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
// "CS120B is Legend... wait for it DARY!" 
void Tick() {
	switch(state) {
		case start:
			state = run;
			break;
		case run:
			state = run;
			k++;
			break;
		default:
		state = start;
			break;
	}
	switch(state) {
		case start:
			LCD_ClearScreen();
			break;
		case run:
			if (k == 53) {
				j = 0;
				k = 0;
			}
			else {
				j = k;
			}
			for (i = 0; i < 16; i++) {
				LCD_Cursor(i);
				LCD_WriteData(message[j]);
				j++;
			}
			break;
		default:
			break;
	}
};

int main(void)
{
	DDRD = 0xFF; PORTD = 0x00; // LCD data lines
	DDRA = 0xFF; PORTA = 0x00; // LCD control lines
	
	// Initializes the LCD display
	LCD_init();
	LCD_ClearScreen();
	// Starting at position 1 on the LCD screen, writes Hello World
	// LCD_DisplayString(1, "Hello World");
	
	TimerSet(500);
	TimerOn();
	state = start;
	
	while (1) {
		Tick();
		while (!TimerFlag) {};
		TimerFlag = 0;
	}
}
