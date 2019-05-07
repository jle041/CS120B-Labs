/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab #7 Exercise #2
 *	Exercise Description: (Challenge) Extend the earlier light game to maintain
 *  a score on the LCD display. The initial score is 5. Each time the user presses
 *  the button at the right time, the score increments. Each time the user fails,
 *  the score decrements. When reaching 9, show victory somehow. 
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"
enum States {start, init, led0, led1, led2, btnRelease, pause} state;

unsigned char btn = 0x00;
unsigned char currState = 0x00;
unsigned char checkPause = 0x00;
unsigned char tmpScore = 0x00;
volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A.
	OCR1A = 125;	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void Tick() {
	btn = ~PINA & 0x01;
	
	switch(state) { // Transitions
		case start:
			state = init;
			break;
		case init:
			state = led0;
			break;
		case led0:
			if (btn) {
				state = btnRelease;
				if (tmpScore - 1 >= 0) {
					tmpScore--;
					LCD_Cursor(1);
					LCD_WriteData(tmpScore + '0');
				}
			}
			else if (~btn) {
				state = led1;
			}
			currState = 0x00;
			break;
		case led1:
			if (btn) {
				state = btnRelease;
				tmpScore++;
				LCD_Cursor(1);
				LCD_WriteData(tmpScore + '0');
			}
			else if (~btn) {
				if (currState == 0x00) {
					state = led2;
				}
				else if (currState == 0x02){
					state = led0;
				}
			}
			currState = 0x01;
			break;
		case led2:
			if (btn) {
				state = btnRelease;
				if (tmpScore - 1 >= 0) {
					tmpScore--;
					LCD_Cursor(1);
					LCD_WriteData(tmpScore + '0');
				}
			}
			else if (~btn) {
				state = led1;
			}
			currState = 0x02;
			break;
		case btnRelease:
			if (btn && currState == 0x09) {
				state = start;
			}
			else if (tmpScore == 9) {
				LCD_DisplayString(1, "WINNER!!!");
				currState = 0x09;
				state = btnRelease;
			}
			else if (btn) {
				state = btnRelease;
			}
			else if (~btn && checkPause == 0x00) {
				state = pause;
				checkPause = 0x01;
			}
			else if (~btn && checkPause == 0x01) {
				state = led0;
				checkPause = 0x00;
			}
			break;
		case pause:
			if (btn) {
				state = btnRelease;
			}
			else {
				state = pause;
			}
			break;
		default:
			state = start;
			break;
	}
	switch (state) { // Actions
		case init:
			tmpScore = 5;
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_WriteData('5');
			break;
		case led0:
			PORTB = 0x01;
			break;
		case led1:
			PORTB = 0x02;
			break;
		case led2:
			PORTB = 0x04;
			break;
		case pause:
			if (currState == 0x00) {
				PORTB = 0x01;
			}
			else if (currState == 0x01) {
				PORTB = 0x02;
			}
			else if (currState == 0x02) {
				PORTB = 0x04;
			}
			break;
		default:
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0x00; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	TimerSet(300);
	TimerOn();
	state = start;
	LCD_init();
	while(1) {
		// User code (i.e. synchSM calls)
		Tick();
		while (!TimerFlag);
		TimerFlag = 0;
	}
}
