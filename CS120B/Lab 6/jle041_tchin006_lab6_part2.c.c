/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab #6 Exercise #2
 *	Exercise Description: (Challenge) Create a simple light game that requires
 *  pressing a button on PA0 while the middle of three LEDs on PB0, PB1, and PB2
 *  is lit. The LEDs light for 300 ms each in sequence. When the button is pressed,
 *  the currently lit LED stays lit. Pressing the button again restarts the game.  
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

enum States {start, led0, led1, led2, btnRelease, pause} state;

unsigned char btn = 0x00;
unsigned char currState = 0x00;
unsigned char checkPause = 0x00;
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
		state = led0;
		break;
		case led0:
			while (!TimerFlag);
			TimerFlag = 0;
			if (btn) {
				state = btnRelease;
			}
			else if (~btn) {
				state = led1;
			}
			currState = 0x00;
			break;
		case led1:
			while (!TimerFlag);
			TimerFlag = 0;
			if (btn) {
				state = btnRelease;
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
			while (!TimerFlag);
			TimerFlag = 0;
			if (btn) {
				state = btnRelease;
			}
			else if (~btn) {
				state = led1;
			}
			currState = 0x02;
			break;
		case btnRelease:
			if (btn) {
				state = btnRelease;
			}
			else if (~btn && checkPause == 0x00) {
				state = pause;
				checkPause = 0x01;
			}
			else if (~btn && checkPause == 0x01) {
				while (!TimerFlag);
				TimerFlag = 0;
				state = start;
				checkPause = 0x00;
			}
			break;
		case pause:
			if (btn) {
				while (!TimerFlag);
				TimerFlag = 0;
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
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(70);
	TimerOn();
	while(1) {
		// User code (i.e. synchSM calls)
		Tick();

	}
}
