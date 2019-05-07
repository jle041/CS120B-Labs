/*
 * custom_project.c
 *
 * Created: 2/28/2019 4:31:04 PM
 * Author : Nick Grant
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "io.c"
#include "timer.h"
#include "scheduler.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define SET_BIT(p,i) ((p) |= (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) & (1 << (i)))
#define SHIFT_REG PORTA

void transmit_data(unsigned char data){
	unsigned char tmp = data;
	
	SET_BIT(SHIFT_REG, 3);				// set SRCLR high
	CLR_BIT(SHIFT_REG, 1);				// set RCLK low
	
	for(int i = 0; i < 8; i++){
		CLR_BIT(SHIFT_REG, 2);			// set SRCLK low
		tmp = GET_BIT(data, i);
		if(tmp) {
			SET_BIT(SHIFT_REG, 0);	// set SER to data to be transmitted
		}
		else {	
			CLR_BIT(SHIFT_REG, 0);
		}
		SET_BIT(SHIFT_REG, 2);			// set SRCLK high
	}
	
	SET_BIT(SHIFT_REG, 3);				// set RCLK high
// 	unsigned char r;
// 	for (r = 0; r < 8 ; ++r) {
// 		// Sets SRCLR to 1 allowing data to be set
// 		// Also clears SRCLK in preparation of sending data
// 		PORTC = 0x01;
// 		// set SER = next bit of data to be sent.
// 		PORTC |= ((data >> r) & 0x08);
// 		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
// 		PORTC |= 0x02;
// 	}
// 	// set RCLK = 1. Rising edge copies data from the “Shift” register to the “Storage” register
// 	PORTC |= 0x04;
// 	// clears all lines in preparation of a new transmission
// 	PORTC = 0x00;
	
	// 0 = SER
	// 1 = RCLK
	// 2 = SRCLK
	// 3 = SRCLR
}

void CreateCustomCharacter(unsigned char *Pattern, const char Location) {
	int j = 0;
	LCD_WriteCommand(0x40+(Location*8));     //Send the Address of CGRAM
	for (j = 0; j < 8; j++) {
		LCD_WriteData(Pattern[j]);         //Pass the bytes of pattern on LCD
	}
}

task tasks[4];
const unsigned short tasksNum = 4;

uint8_t EEMEM highScore;

unsigned char j, k, l;
char title[57] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'P', 'R', 'E', 'S', 'S', ' ', 'A', 'N', 'Y', ' ', 'B', 'U', 'T', 'T', 'O', 'N', ' ', 'T', 'O', ' ', 'S', 'T', 'A', 'R', 'T', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
unsigned char btnA, btnB, btnC, btnD, btnE;
unsigned char rulesLine, checkStart= 0;
unsigned char timeSec = 0;
unsigned char score = 0;
uint8_t tmpHighScore = 0;
const char *rules[6] = {"Click the button", "corresponding to", "the line. Tap as", "many lines as u ", "can before time ", "runs out!       "};
enum LCD_States {LCD_Start, LCD_Lobby, LCD_Options, LCD_Rules, LCD_Game};
int TickFct_LCD (int state){
	btnA = ~PINB & 0x08;
	btnB = ~PINB & 0x10;
	btnC = ~PINB & 0x20;
	btnD = ~PINB & 0x40;
	btnE = ~PINB & 0x80;
	
	// TRANSITIONS
	switch(state) {
		case LCD_Start:
			state = LCD_Lobby;
		break;
		case LCD_Lobby:
			if (btnA || btnB || btnC || btnD || btnE) {
				state = LCD_Options;
			}
			else {
				state = LCD_Lobby;
			}
			l++;
		break;
		case LCD_Options:
			if (!btnA && !btnB && btnC && !btnD && !btnE) {
				k = 0; l = 0;
				state = LCD_Lobby;
			}
			else if (!btnA && btnB && !btnC && !btnD && !btnE) {
				state = LCD_Rules;
				rulesLine = 0;
			}
			else if (btnA && !btnB && !btnC && !btnD && !btnE) {
				state = LCD_Game;
				checkStart = 1;
				LCD_ClearScreen();
			}
			else {
				state = LCD_Options;
			}
		break;
		case LCD_Rules:
			if (!btnA && !btnB && btnC && !btnD && !btnE) {
				state = LCD_Options;
			}
			else {
				state = LCD_Rules;
			}
		break;
		case LCD_Game:
			if (btnA && btnB && btnC && btnD && btnE) {
				state = LCD_Lobby;
				checkStart = 0;
			}
			else {
				state = LCD_Game;
			}
		break;
		default:
			state = LCD_Start;
		break;
	}
	// ACTIONS
	switch(state) {
		case LCD_Start:
			LCD_ClearScreen();
			checkStart = 0;
			score = 0;
		break;
		case LCD_Lobby:
			checkStart = 0;
			score = 0;
			LCD_DisplayString(1, "TAP THE RED TILE");
			if (l == 41) {
				k = 0;
				l = 0;
			}
			else {
				k = l;
			}
			for (j = 17; j < 33; j++) {
				LCD_Cursor(j);
				LCD_WriteData(title[k]);
				k++;
			}
		break;
		case LCD_Options:
			LCD_ClearScreen();
			LCD_DisplayString(1, "1)START  2)RULES");
			LCD_DisplayString(17, "     3)BACK     ");
		break;
		case LCD_Rules:
			LCD_DisplayString(1, "1)   2)   3)BACK");
			LCD_Cursor(3);
			LCD_WriteData(0); //up arrow
			LCD_Cursor(8);
			LCD_WriteData(1); //down arrow
			
			LCD_DisplayString(17, rules[rulesLine]);
			if (btnA && !btnB) {
				if (rulesLine > 0) {
					rulesLine--;
				}
			}
			else if (!btnA && btnB) {
				if (rulesLine < 5) {
					rulesLine++;
				}
			}
		break;
		case LCD_Game:
			LCD_DisplayString(9, "Score:");
		break;
		default:
		break;
	}
	
	return state;
};

enum Game_States {Game_Start, Game_Run};
int TickFct_Game (int state) {
	btnA = ~PINB & 0x08;
	btnB = ~PINB & 0x10;
	btnC = ~PINB & 0x20;
	btnD = ~PINB & 0x40;
	btnE = ~PINB & 0x80;
	
	// TRANSITIONS
	switch(state) {
		case Game_Start:
			if (checkStart) {
				state = Game_Run;
				// tmpHighScore = eeprom_read_byte(&highScore);
			}
			else {
				state = Game_Start;
			}
		break;
		case Game_Run:
			if (checkStart) {
				state = Game_Run;
			}
			else {
				state = Game_Start;
				if (score >= tmpHighScore) {
					tmpHighScore = score;
					eeprom_write_byte(&highScore, tmpHighScore);
				}		
				LCD_DisplayString(17,"G A M E  O V E R");
			}
		break;
		default:
			state = Game_Start;
		break;
	}
	// ACTIONS
	switch(state) {
		case Game_Start:
			score = 0;
		break;
		case Game_Run:
			if (score == 0) {
				LCD_DisplayString(15, "0 ");
			}
			else if (score == 1) {
				LCD_DisplayString(15, "1 ");
			}
			else if (score == 2) {
				LCD_DisplayString(15, "2 ");
			}
			else if (score == 3) {
				LCD_DisplayString(15, "3 ");
			}
			else if (score == 4) {
				LCD_DisplayString(15, "4 ");
			}
			else if (score == 5) {
				LCD_DisplayString(15, "5 ");
			}
			else if (score == 6) {
				LCD_DisplayString(15, "6 ");
			}
			else if (score == 7) {
				LCD_DisplayString(15, "7 ");
			}
			else if (score == 8) {
				LCD_DisplayString(15, "8 ");
			}
			else if (score == 9) {
				LCD_DisplayString(15, "9 ");
			}
			else if (score == 10) {
				LCD_DisplayString(15, "10");
			}
			else if (score == 11) {
				LCD_DisplayString(15, "11");
			}
			else if (score == 12) {
				LCD_DisplayString(15, "12");
			}
			else if (score == 13) {
				LCD_DisplayString(15, "13");
			}
			else if (score == 14) {
				LCD_DisplayString(15, "14");
			}
			else if (score == 15) {
				LCD_DisplayString(15, "15");
			}
			else if (score == 16) {
				LCD_DisplayString(15, "16");
			}
			else if (score == 17) {
				LCD_DisplayString(15, "17");
			}
			else if (score == 18) {
				LCD_DisplayString(15, "18");
			}
			else if (score == 19) {
				LCD_DisplayString(15, "19");
			}
			else if (score == 20) {
				LCD_DisplayString(15, "20");
			}
			else if (score == 21) {
				LCD_DisplayString(15, "21");
			}
			else if (score == 22) {
				LCD_DisplayString(15, "22");
			}
			else if (score == 23) {
				LCD_DisplayString(15, "23");
			}
			else if (score == 24) {
				LCD_DisplayString(15, "24");
			}
			else if (score == 25) {
				LCD_DisplayString(15, "25");
			}
			else if (score == 26) {
				LCD_DisplayString(15, "26");
			}
			else if (score == 27) {
				LCD_DisplayString(15, "27");
			}
			else if (score == 28) {
				LCD_DisplayString(15, "28");
			}
			else if (score == 29) {
				LCD_DisplayString(15, "29");
			}
			else if (score == 30) {
				LCD_DisplayString(15, "30");
			}
			else if (score == 31) {
				LCD_DisplayString(15, "31");
			}
			else if (score == 32) {
				LCD_DisplayString(15, "32");
			}
			else if (score == 33) {
				LCD_DisplayString(15, "33");
			}
			else if (score == 34) {
				LCD_DisplayString(15, "34");
			}
			else if (score == 35) {
				LCD_DisplayString(15, "35");
			}
			else if (score == 36) {
				LCD_DisplayString(15, "36");
			}
			else if (score == 37) {
				LCD_DisplayString(15, "37");
			}
			else if (score == 38) {
				LCD_DisplayString(15, "38");
			}
			else if (score == 39) {
				LCD_DisplayString(15, "39");
			}
			else if (score == 40) {
				LCD_DisplayString(15, "40");
			}
			else if (score == 41) {
				LCD_DisplayString(15, "41");
			}
			else if (score == 42) {
				LCD_DisplayString(15, "42");
			}
			else if (score == 43) {
				LCD_DisplayString(15, "43");
			}
			else if (score == 44) {
				LCD_DisplayString(15, "44");
			}
			else if (score == 45) {
				LCD_DisplayString(15, "45");
			}
			else if (score == 46) {
				LCD_DisplayString(15, "46");
			}
			else if (score == 47) {
				LCD_DisplayString(15, "47");
			}
			else if (score == 48) {
				LCD_DisplayString(15, "48");
			}
			else if (score == 49) {
				LCD_DisplayString(15, "49");
			}
			else if (score == 50) {
				LCD_DisplayString(15, "50");
			}
			
			LCD_DisplayString(17, "Hi-Score:");
			if (tmpHighScore == 1) {
				LCD_DisplayString(26, "1 ");
			}
			else if (tmpHighScore == 2) {
				LCD_DisplayString(26, "2 ");
			}
			else if (tmpHighScore == 3) {
				LCD_DisplayString(26, "3 ");
			}
			else if (tmpHighScore == 4) {
				LCD_DisplayString(26, "4 ");
			}
			else if (tmpHighScore == 5) {
				LCD_DisplayString(26, "5 ");
			}
			else if (tmpHighScore == 6) {
				LCD_DisplayString(26, "6 ");
			}
			else if (tmpHighScore == 7) {
				LCD_DisplayString(26, "7 ");
			}
			else if (tmpHighScore == 8) {
				LCD_DisplayString(26, "8 ");
			}
			else if (tmpHighScore == 9) {
				LCD_DisplayString(26, "9 ");
			}
			else if (tmpHighScore == 10) {
				LCD_DisplayString(26, "10");
			}
			else if (tmpHighScore == 11) {
				LCD_DisplayString(26, "11");
			}
			else if (tmpHighScore == 12) {
				LCD_DisplayString(26, "12");
			}
			else if (tmpHighScore == 13) {
				LCD_DisplayString(26, "13");
			}
			else if (tmpHighScore == 14) {
				LCD_DisplayString(26, "14");
			}
			else if (tmpHighScore == 15) {
				LCD_DisplayString(26, "15");
			}
			else if (tmpHighScore == 16) {
				LCD_DisplayString(26, "16");
			}
			else if (tmpHighScore == 17) {
				LCD_DisplayString(26, "17");
			}
			else if (tmpHighScore == 18) {
				LCD_DisplayString(26, "18");
			}
			else if (tmpHighScore == 19) {
				LCD_DisplayString(26, "19");
			}
			else if (tmpHighScore == 20) {
				LCD_DisplayString(26, "20");
			}
			else if (tmpHighScore == 21) {
				LCD_DisplayString(26, "21");
			}
			else if (tmpHighScore == 22) {
				LCD_DisplayString(26, "22");
			}
			else if (tmpHighScore == 23) {
				LCD_DisplayString(26, "23");
			}
			else if (tmpHighScore == 24) {
				LCD_DisplayString(26, "24");
			}
			else if (tmpHighScore == 25) {
				LCD_DisplayString(26, "25");
			}
			else if (tmpHighScore == 26) {
				LCD_DisplayString(26, "26");
			}
			else if (tmpHighScore == 27) {
				LCD_DisplayString(26, "27");
			}
			else if (tmpHighScore == 28) {
				LCD_DisplayString(26, "28");
			}
			else if (tmpHighScore == 29) {
				LCD_DisplayString(26, "29");
			}
			else if (tmpHighScore == 30) {
				LCD_DisplayString(26, "30");
			}
			else if (tmpHighScore == 31) {
				LCD_DisplayString(26, "31");
			}
			else if (tmpHighScore == 32) {
				LCD_DisplayString(26, "32");
			}
			else if (tmpHighScore == 33) {
				LCD_DisplayString(26, "33");
			}
			else if (tmpHighScore == 34) {
				LCD_DisplayString(26, "34");
			}
			else if (tmpHighScore == 35) {
				LCD_DisplayString(26, "35");
			}
			else if (tmpHighScore == 36) {
				LCD_DisplayString(26, "36");
			}
			else if (tmpHighScore == 37) {
				LCD_DisplayString(26, "37");
			}
			else if (tmpHighScore == 38) {
				LCD_DisplayString(26, "38");
			}
			else if (tmpHighScore == 39) {
				LCD_DisplayString(26, "39");
			}
			else if (tmpHighScore == 40) {
				LCD_DisplayString(26, "40");
			}
			else if (tmpHighScore == 41) {
				LCD_DisplayString(26, "41");
			}
			else if (tmpHighScore == 42) {
				LCD_DisplayString(26, "42");
			}
			else if (tmpHighScore == 43) {
				LCD_DisplayString(26, "43");
			}
			else if (tmpHighScore == 44) {
				LCD_DisplayString(26, "44");
			}
			else if (tmpHighScore == 45) {
				LCD_DisplayString(26, "45");
			}
			else if (tmpHighScore == 46) {
				LCD_DisplayString(26, "46");
			}
			else if (tmpHighScore == 47) {
				LCD_DisplayString(26, "47");
			}
			else if (tmpHighScore == 48) {
				LCD_DisplayString(26, "48");
			}
			else if (tmpHighScore == 49) {
				LCD_DisplayString(26, "49");
			}
			else if (tmpHighScore == 50) {
				LCD_DisplayString(26, "50");
			}
			else {
				LCD_DisplayString(26, "0 ");
			}

		break;
		default:
		break;
	}
	return state;	
};

enum Timer_States {Timer_Start, Timer_Run};
int TickFct_Timer (int state) {
	switch(state) {
		case Timer_Start:
			if (checkStart) {
				state = Timer_Run;
			}
			else {
				state = Timer_Start;
			}
		break;
		case Timer_Run:
			if (checkStart) {
				state = Timer_Run;
			}
			else {
				state = Timer_Start;
			}
		break;
		default:
			state = Timer_Start;
		break;
	}
	// ACTIONS
	switch(state) {
		case Timer_Start:
			timeSec = 30;
		break;
		case Timer_Run:
			LCD_DisplayString(1, "Time:");
		
			if (timeSec == 30) {
				LCD_DisplayString(6, "30");
			}
			else if (timeSec == 29) {
				LCD_DisplayString(6, "29");
			}
			else if (timeSec == 28) {
				LCD_DisplayString(6, "28");
			}
			else if (timeSec == 27) {
				LCD_DisplayString(6, "27");
			}
			else if (timeSec == 26) {
				LCD_DisplayString(6, "26");
			}
			else if (timeSec == 25) {
				LCD_DisplayString(6, "25");
			}
			else if (timeSec == 24) {
				LCD_DisplayString(6, "24");
			}
			else if (timeSec == 23) {
				LCD_DisplayString(6, "23");
			}
			else if (timeSec == 22) {
				LCD_DisplayString(6, "22");
			}
			else if (timeSec == 21) {
				LCD_DisplayString(6, "21");
			}
			else if (timeSec == 20) {
				LCD_DisplayString(6, "20");
			}
			else if (timeSec == 19) {
				LCD_DisplayString(6, "19");
			}
			else if (timeSec == 18) {
				LCD_DisplayString(6, "18");
			}
			else if (timeSec == 17) {
				LCD_DisplayString(6, "17");
			}
			else if (timeSec == 16) {
				LCD_DisplayString(6, "16");
			}
			else if (timeSec == 15) {
				LCD_DisplayString(6, "15");
			}
			else if (timeSec == 14) {
				LCD_DisplayString(6, "14");
			}
			else if (timeSec == 13) {
				LCD_DisplayString(6, "13");
			}
			else if (timeSec == 12) {
				LCD_DisplayString(6, "12");
			}
			else if (timeSec == 11) {
				LCD_DisplayString(6, "11");
			}
			else if (timeSec == 10) {
				LCD_DisplayString(6, "10");
			}
			else if (timeSec == 9) {
				LCD_DisplayString(6, "9 ");
			}
			else if (timeSec == 8) {
				LCD_DisplayString(6, "8 ");
			}
			else if (timeSec == 7) {
				LCD_DisplayString(6, "7 ");
			}
			else if (timeSec == 6) {
				LCD_DisplayString(6, "6 ");
			}
			else if (timeSec == 5) {
				LCD_DisplayString(6, "5 ");
			}
			else if (timeSec == 4) {
				LCD_DisplayString(6, "4 ");
			}
			else if (timeSec == 3) {
				LCD_DisplayString(6, "3 ");
			}
			else if (timeSec == 2) {
				LCD_DisplayString(6, "2 ");
			}
			else if (timeSec == 1) {
				LCD_DisplayString(6, "1 ");
			}
			else if (timeSec <= 0) {
				LCD_DisplayString(6, "0");
			}
			
			if (timeSec != 0) {
				timeSec--;
			}
			else {
				checkStart = 0;
			}
		break;
		default:
		break;
	}
	
	return state;
};

unsigned char tmpRow = 0;
unsigned char btnNeeded = 0;
enum Matrix_States {Matrix_Start, Matrix_Run, Matrix_Press};
int TickFct_Matrix (int state) {
	unsigned char pattern = 0xFF;
	unsigned char row = 0xFF;
	// TRANSITIONS
	switch(state) {
		case Matrix_Start:
			if (checkStart) {
				state = Matrix_Run;
			}
			else {
				state = Matrix_Start;
			}
		break;
		case Matrix_Run:
			if (checkStart) {
				state = Matrix_Press;
			}
			else {
				state = Matrix_Start;
			}
		break;
		case Matrix_Press:
			if (btnNeeded == 1) {
				if (btnA && !btnB && !btnC && !btnD && !btnE) {
					score++;
					state = Matrix_Run;
				}
				else {
					state = Matrix_Press;
				}
			}
			else if (btnNeeded == 2) {
				if (!btnA && btnB && !btnC && !btnD && !btnE) {
					score++;
					state = Matrix_Run;
				}
				else {
					state = Matrix_Press;
				}
			}
			else if (btnNeeded == 3) {
				if (!btnA && !btnB && btnC && !btnD && !btnE) {
					score++;
					state = Matrix_Run;
				}
				else {
					state = Matrix_Press;
				}
			}
			else if (btnNeeded == 4) {
				if (!btnA && !btnB && !btnC && btnD && !btnE) {
					score++;
					state = Matrix_Run;
				}
				else {
					state = Matrix_Press;
				}
			}
			else if (btnNeeded == 5) {
				if (!btnA && !btnB && !btnC && !btnD && btnE) {
					score++;
					state = Matrix_Run;
				}
				else {
					state = Matrix_Press;
				}
			}
		break;
		default:
			state = Matrix_Start;
		break;
	}
	// ACTIONS
	switch(state) {
		case Matrix_Start:
			btnNeeded = 0;
			transmit_data(0xFF);
			PORTC = 0x00;
		break;
		case Matrix_Run:
			tmpRow = rand() % 5;
			if (tmpRow == 0) {
				// LCD_DisplayString(30, "0");
				row = 0xFE;
				btnNeeded = 1;
			}
			else if (tmpRow == 1) {
				// LCD_DisplayString(30, "1");
				row = 0xFD;
				btnNeeded = 2;
			}
			else if (tmpRow == 2) {
				// LCD_DisplayString(30, "2");
				row = 0xFB;
				btnNeeded = 3;
			}
			else if (tmpRow == 3) {
				// LCD_DisplayString(30, "3");
				row = 0xF7;
				btnNeeded = 4;
			}
			else {
				// LCD_DisplayString(30, "4");
				row = 0xEF;
				btnNeeded = 5;
			}
			transmit_data(pattern);
			PORTC = row;
		break;
		case Matrix_Press:
		break;
		default:
		break;
	}
	
	return state;	
};

int main() {
    DDRA = 0xFF; PORTA = 0x00; // outputs, for pattern of LED matrix
	DDRB = 0x03; PORTB = 0xF8; // LCD control lines on B0 and B1, buttons on B4-8
	DDRC = 0xFF; PORTC = 0x00; // outputs, for shift register for rows of LED matrix
	DDRD = 0xFF; PORTD = 0x00; // LCD data lines
	
	unsigned char upArrow[] = { 0b00100, 0b01110, 0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100 };
	unsigned char downArrow[] = { 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b11111, 0b01110, 0b00100 };
	
	time_t t;
	srand((unsigned) time(&t));

	LCD_init();
	LCD_WriteCommand(0x38); // Function Set: 8-bit, 2 Line, 5x7 Dots
	LCD_WriteCommand(0x01); // Clear LCD
	LCD_WriteCommand(0x06); // Entry Mode
	LCD_WriteCommand(0x0c); // Display on Cursor off

	unsigned char i = 0;
	tasks[i].state = LCD_Start;
	tasks[i].period = 500;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_LCD;
	i++;
	tasks[i].state = Game_Start;
	tasks[i].period = 100;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_Game;
	i++;
	tasks[i].state = Timer_Start;
	tasks[i].period = 1000;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_Timer;
	i++;
	tasks[i].state = Matrix_Start;
	tasks[i].period = 300;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_Matrix;
	
	TimerSet(100);
	TimerOn();
	
	CreateCustomCharacter(upArrow, 0);
	CreateCustomCharacter(downArrow, 1);

	while(1) {}
	
	return 0;
}

void TimerISR() {
	unsigned char i;
	for (i=0; i < tasksNum; ++i) {
		if (tasks[i].elapsedTime >= tasks[i].period) {
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += 100;
	}
}