/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab #10 Exercise #2
 *	Exercise Description: Modify the above example so the three LEDs light for
 *  300 ms, while PB3's LED still blinks 1 second on and 1 second off.
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

enum BL_States {BL_start, blinkOff, blinkOn} BL_State;
unsigned char blinkingLED = 0x00;
void TickBL() {
	switch(BL_State) {
		case BL_start:
			BL_State = blinkOff;
			break;
		case blinkOff:
			BL_State = blinkOn;
			break;
		case blinkOn:
			BL_State = blinkOff;
			break;
		default:
			BL_State = BL_start;
			break;
	}
	switch(BL_State) {
		case BL_start:
			blinkingLED = 0;
			break;
		case blinkOff:
			blinkingLED = 0;			
			break;
		case blinkOn:
			blinkingLED = 0x08;
			break;
		default:
			break;
	}
}

enum TL_States {TL_start, led0, led1, led2} TL_State;
unsigned char threeLEDs = 0x00;
void TickTL() {
	switch(TL_State) {
		case TL_start:
			TL_State = led0;
			break;
		case led0:
			TL_State = led1;
			break;
		case led1:
			TL_State = led2;
			break;
		case led2:
			TL_State = led0;
			break;
		default:
			TL_State = TL_start;
			break;
	}
	switch(TL_State) {
		case TL_start:
			threeLEDs = 0;
			break;
		case led0:
			threeLEDs = 0x01;
			break;
		case led1:
			threeLEDs = 0x02;
			break;
		case led2:
			threeLEDs = 0x04;
			break;
		default:
			break;
	}
}

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;
	unsigned long BL_elapsedTime = 0;
	unsigned long TL_elapsedTime = 0;
    const unsigned long timerPeriod = 100;
	
	PORTB = 0;
	TimerSet(100);
	TimerOn();
	BL_State = BL_start;
	TL_State = TL_start;
	
    while (1) 
    {
		if (BL_elapsedTime >= 1000) {
			TickBL();
			BL_elapsedTime = 0;
		}
		if (TL_elapsedTime >= 300) {
			TickTL();
			TL_elapsedTime = 0;
		}
		
		while (!TimerFlag) {}
		TimerFlag = 0;
		
		BL_elapsedTime += timerPeriod;
		TL_elapsedTime += timerPeriod;
		
		PORTB = threeLEDs + blinkingLED;
    }
}

