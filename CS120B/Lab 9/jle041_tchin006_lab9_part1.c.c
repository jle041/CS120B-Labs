/*	Partner(s) Name & E-mail: Thomas Chin, tchin006
 *	Lab Section: 024
 *	Assignment: Lab #9 Exercise #1
 *	Exercise Description: Using the ATmega1284’s PWM functionality, design a
 *  system that uses three buttons to select one of three tones to be generated
 *  on the speaker. When a button is pressed, the tone mapped to it is generated
 *  on the speaker.
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
 

#include <avr/io.h>

enum States{start, init, soundOn, soundOff} state;

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

unsigned char btnA; // note C4, freq = 261.63
unsigned char btnB; // note D4, freq = 293.66
unsigned char btnC; // note E4, freq = 329.63

void Tick() {
	btnA = ~PINA & 0x01;
	btnB = ~PINA & 0x02;
	btnC = ~PINA & 0x04;
	
	switch(state) { // TRANSITIONS
		case start:
			state = init;
			break;
		case init:
			if (btnA && ~btnB && ~btnC) {
				state = soundOn;
			}
			else if (~btnA && btnB && ~btnC) {
				state = soundOn;
			}
			else if (~btnA && ~btnB && btnC) {
				state = soundOn;
			}
			else {
				state = init;
			}
			break;
		case soundOn:
			if (btnA && ~btnB && ~btnC) {
				state = soundOn;
			}
			else if (~btnA && btnB && ~btnC) {
				state = soundOn;
			}
			else if (~btnA && ~btnB && btnC) {
				state = soundOn;
			}
			else {
				state = soundOff;
			}
			break;
		case soundOff:
			state = init;
			break;
		default:
			state = start;
			break;
	}
	switch(state) { // ACTIONS
		case start:	
			break;
		case init:
			set_PWM(0);
			break;
		case soundOn:
			if (btnA && ~btnB && ~btnC) {
				set_PWM(261.63);
			}
			else if (~btnA && btnB && ~btnC) {
				set_PWM(293.66);
			}
			else if (~btnA && ~btnB && btnC) {
				set_PWM(329.63);
			}
			break;
		case soundOff:
			set_PWM(0);
			break;
		default:
			break;
	}
}

int main(void)
{
    DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0x08; PORTB = 0x00; // only PB3 is output (speaker must be on PB3)
    state = start;
	PWM_on();
	while (1) {
		Tick();
    }
}

