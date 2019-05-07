/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab #11  Exercise #3
 *	Exercise Description: Combine the functionality of the keypad and LCD so
 *  when keypad is pressed and released, the character of the button pressed
 *  is displayed on the LCD, and stays displayed until a different button press
 *  occurs (May be accomplished with two tasks: LCD interface & modified test
 *  harness).
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include "keypad.h"
#include "io.c"

int main(void)
{
	unsigned char x;
	DDRA = 0xFF; PORTA = 0x00; // LCD control lines
	DDRD = 0xFF; PORTD = 0x00; // LCD data lines
	// DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0s
	DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	
	LCD_init();
	LCD_ClearScreen();
	
	while(1) {
		x = GetKeypadKey();
		switch (x) {
			// case '\0': LCD_DisplayString(1, " "); break; // All 5 LEDs on
			case '1': LCD_DisplayString(1, "1"); break; // hex equivalent
			case '2': LCD_DisplayString(1, "2"); break;
			case '3': LCD_DisplayString(1, "3"); break;
			case '4': LCD_DisplayString(1, "4"); break;
			case '5': LCD_DisplayString(1, "5"); break;
			case '6': LCD_DisplayString(1, "6"); break;
			case '7': LCD_DisplayString(1, "7"); break;
			case '8': LCD_DisplayString(1, "8"); break;
			case '9': LCD_DisplayString(1, "9"); break;
			case 'A': LCD_DisplayString(1, "A"); break;
			case 'B': LCD_DisplayString(1, "B"); break;
			case 'C': LCD_DisplayString(1, "C"); break;
			case 'D': LCD_DisplayString(1, "D"); break;
			case '*': LCD_DisplayString(1, "*"); break;
			case '0': LCD_DisplayString(1, "0"); break;
			case '#': LCD_DisplayString(1, "#"); break;
			// default: LCD_DisplayString(1, " "); break; // Should never occur. Middle LED off.
		}
	}
}

