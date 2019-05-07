/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab #10 Exercise #5
 *	Exercise Description: Buttons are connected to PA0 and PA1. Output PORTB
 *  drives a bank of 4 LEDs. Pressing PA0 increments a binary number displayed
 *  on the bank of LEDs (stopping at 9). Pressing PA1 decrements the binary number
 *  (stopping at 0). If both buttons are depressed (even if not initially
 *  simultaneously), the display resets to 0. If a button is held, then the display
 *  continues to increment (or decrement) at a rate of once per second. However, if
 *  the button is held for 3 seconds, the incrementing/decrementing occurs once per
 *  400 ms. Use synchronous state machines captured in C.
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

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

unsigned char tempB = 0x00; 
unsigned long current = 0x00;
unsigned long period = 0x00;

enum counterStates {start, wait, increment, decrement, reset} cState;
unsigned char btn0; // increment (~PINA & 0x01)
unsigned char btn1; // decrement (~PINA & 0x02)
void counter() {
	btn0 = ~PINA & 0x01;
	btn1 = ~PINA & 0x02;
	
	switch(cState)	{
		case start:
			cState = wait;
			break;
		case wait:
			if (btn0 && !btn1) {
				cState = increment;
			}
			else if (!btn0 && btn1) {
				cState = decrement;
			}
			else if (btn0 && btn1) {
				cState = reset;
			}
			else {
				cState = wait;
			}
			break;
		case increment:
			if (btn0 && !btn1) {
				cState = increment;
				Button_Hold();
			}
			else if (btn0 && btn1) {
				cState = reset;
			}
			else {
				cState = wait;
				if (tempB < 9) {
					++tempB;
				}
			}
			break;
		case decrement:
			if (!btn0 && btn1) {
				cState = decrement;
				Button_Hold();
			}
			else if (btn0 && btn1) {
				cState = reset;
			}
			else {
				cState = wait;
				if(tempB > 0) {
					--tempB;
				}
			}
			break;
		case reset:
			if (!btn0 && !btn1) {
				cState = wait;
			}
			else {
				cState = reset;
			}
			break;
		default:
			break;
	}
	
	switch(cState) {
		case start:
			break;
		case wait:
			current = 0;
			period = 1;
			break;
		case increment:
			break;
		case decrement:
			break;
		case reset:
			tempB = 0;
			break;
		default:
			break;
	}
}

enum setB_States {setB_Start, set} BState;
void setB() {
	switch(BState){
		case setB_Start:
			BState = set;
			break;
		case set:
			BState = set;
			break;
		default:
			BState = setB_Start;
	}
	switch(BState){
		case setB_Start:
			PORTB = 0x00;
			break;
		case set:
			PORTB = tempB;
			break;
		default:
			break;
	}
}

void Button_Hold() {
	if (period == 1000 && current >= 3000) {
		period = 400;
	}
	else if (period == 1 && current >= 1000) {
		period = 1000;
	}
	
	if (period == 400 || period == 1000) {
		if(cState == increment && tempB < 9) {
			++tempB;
		}
		else if (cState == decrement && tempB > 0) {
			--tempB;
		}
	}
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	unsigned long C_elapsedTime = 1;
	const unsigned long timerPeriod = 1;
	
	TimerSet(timerPeriod);
	TimerOn();
	cState = start;
	BState = setB_Start;
	period = timerPeriod;

	while(1) {
		if (C_elapsedTime >= period) {
			counter();
			C_elapsedTime = 0;
		}
		setB();
		
		while (!TimerFlag){}
		TimerFlag = 0;
		
		C_elapsedTime += timerPeriod;
		current += timerPeriod;
	}
}



