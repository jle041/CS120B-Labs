/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab #7 Exercise #1
 *	Exercise Description: Buttons are connected to PA0 and PA1. Output PORTC
 *  and PORTD drive the LCD display, initially displaying 0. Pressing PA0 
 *  increments the display (stopping at 9). Pressing PA1 decrements the display
 *  (stopping at 0). If both buttons are depressed (even if not initially
 *  simultaneously), the display resets to 0. If a button is held, then the
 *  display continues to increment (or decrement) at a rate of once per second.
 *  Use a synchronous state machine captured in C.
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

 
#include <avr/io.h>
#include "io.c"
#include "timer.h"

enum States {start, wait, add, sub, reset} state;
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
				state = add;
			}
			else if (!addBtn && subBtn) {
				state = sub;
			}
			else if (addBtn && subBtn) {
				state = reset;
			}
			else {
				state = wait;
			}
			break;
		case add:
			if (addBtn && !subBtn) {
				state = add;
			}
			else if (addBtn && subBtn) {
				state = reset;
			}
			else {
				state = wait;
			}
			break;
		case sub:
			if (addBtn && subBtn) {
				state = sub;
			}
			else if (addBtn && subBtn) {
				state = reset;
			}
			else {
				state = wait;
			}
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
			LCD_Cursor(1);
			LCD_WriteData(tmpB + '0');
			break;
		case add:
			if (tmpB + 1 <= 9) {
				tmpB++;
			}
			LCD_Cursor(1);
			LCD_WriteData(tmpB + '0');
			break;
		case sub:
			if (tmpB - 1 >= 0) {
				tmpB--;
			}
			LCD_Cursor(1);
			LCD_WriteData(tmpB + '0');
			break;
		case reset:
			tmpB = 0;
			LCD_Cursor(1);
			LCD_WriteData(tmpB + '0');
			break;
		default:
			break;
	}
	
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // inputs
	DDRC = 0xFF; PORTC = 0x00; // outputs
	DDRD = 0xFF; PORTD = 0x00; // outputs
	TimerSet(500);
	TimerOn();
	state = start;
	LCD_init();
	LCD_ClearScreen();
	LCD_Cursor(1);
	LCD_WriteData('0');
    while (1) 
    {
		Tick();
		while (!TimerFlag);	// Wait 1 sec
		TimerFlag = 0;
    }
}

