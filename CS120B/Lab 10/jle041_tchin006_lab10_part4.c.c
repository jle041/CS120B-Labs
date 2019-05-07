/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab #10 Exercise #4
 *	Exercise Description: Extend the previous exercise to allow a user to adjust
 *  the sound frequency up or down using buttons connected to PA0 (up) and PA1
 *  (down). Using our 1 ms timer abstraction, the fastest you'll be able to pulse
 *  is 1 ms on and 1 ms off, meaning 500 Hz. You'll probably want to introduce
 *  another synchSM that polls the buttons and sets a global variable storing the
 *  current frequency that in turn is read by the frequency generator task. 
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

// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR0B &= 0x08; } //stops timer/counter
		else { TCCR0B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR0A = 0xFFFF; }
		
		// prevents OCR0A from underflowing, using prescaler 64
		// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR0A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR0A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT0 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR0A = (1 << COM0A0) | (1 << WGM00);
	// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR0B = (1 << WGM02) | (1 << CS01) | (1 << CS00);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR0A = 0x00;
	TCCR0B = 0x00;
}

enum BL_States {BL_start, blinkOff, blinkOn} BL_State;
static unsigned char blinkingLED = 0x00;
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
			blinkingLED = 0x10;
			break;
		default:
			break;
	}
}

enum TL_States {TL_start, led0, led1, led2} TL_State;
static unsigned char threeLEDs = 0x00;
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

enum scaleSpeaker {SS_start, wait, scaleUp, scaleDown} SS_State;
double speakerFreq;
unsigned char btnA0; // scale up
unsigned char btnA1; // scale down
unsigned char notesIndex;
double notes[8] = {261.63, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.25};
void TickScale() {
	btnA0 = ~PINA & 0x01;
	btnA1 = ~PINA & 0x02;
	
	switch(SS_State) {
		case SS_start:
			SS_State = wait;
			break;
		case wait:
			if (btnA0 && ~btnA1) {
				SS_State = scaleUp;
			}
			else if (~btnA0 && btnA1) {
				SS_State = scaleDown;
			}
			else {
				SS_State = wait;
			}
			break;
		case scaleUp:
			SS_State = wait;
			break;
		case scaleDown:
			SS_State = wait;
			break;
		default:
			SS_State = SS_start;
			break;
	}
	switch(SS_State) {
		case SS_start:
			notesIndex = 0;
			break;
		case wait:
			speakerFreq = notes[notesIndex];
			break;
		case scaleUp:
			if (notesIndex < 7) {
				notesIndex++;
			}
			break;
		case scaleDown:
			if (notesIndex > 0) {
				notesIndex--;
			}
			break;
		default:
			break;	
	}
}

enum speaker_States {S_start, sOn} S_State;
void TickSpeaker() {
	switch(S_State) {
		case S_start:
			S_State = sOn;
			break;
		case sOn:
			S_State = sOn;
			break;
		default:
			S_State = S_start;
			break;
	}
	switch(S_State) {
		case S_start:
			set_PWM(0);
			break;
		case sOn:
			if (~PINA & 0x04) {
				set_PWM(speakerFreq);
			}
			else {
				set_PWM(0);
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
	unsigned long BL_elapsedTime = 0;
	unsigned long TL_elapsedTime = 0;
	unsigned long S_elapsedTime = 0;
	unsigned long SS_elapsedTime = 0;
    const unsigned long timerPeriod = 1;
	
	PORTB = 0;
	TimerSet(1);
	TimerOn();
	BL_State = BL_start;
	TL_State = TL_start;
	S_State = S_start;
	SS_State = SS_start;
	PWM_on();
	
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
		if (S_elapsedTime >= 1) {
			TickSpeaker();
			S_elapsedTime = 0;
		}
		if (SS_elapsedTime >= 200) {
			TickScale();
			SS_elapsedTime = 0;
		}
		
		while (!TimerFlag) {}
		TimerFlag = 0;
		
		BL_elapsedTime += timerPeriod;
		TL_elapsedTime += timerPeriod;
		S_elapsedTime += timerPeriod;
		SS_elapsedTime += timerPeriod;
		
		PORTB = threeLEDs + blinkingLED;
    }
}

