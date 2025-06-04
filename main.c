

#ifndef F_CPU 
#define F_CPU 8000000UL	// 8 MHz clock speed
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/power.h>
#include <avr/sfr_defs.h>
#include "LCD_Library-main/defines.h"
#include "LCD_Library-main/hd44780.c"
#include "LCD_Library-main/lcd.c"

FILE lcd_str = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);

uint8_t lcdMode = 0;
uint8_t lcdModeTemp;
uint16_t tachNum = 6800;
uint8_t COMPARE = 50;
int TOP = 199;
uint8_t stall = 0;

// start 16bt timer
void start16BitTimer(void) {
	TCCR1B = (1 << CS11);
}

// stop 16bt timer 
void stop16BitTimer(void) {
	TCCR1B &= (1 << CS12);
	TCCR1B &= (1 << CS11);
	TCCR1B &= (1 << CS10);
}

// Initializes Timer0 for PWM
void pwmInit() {
	
	TCNT0 = 0x00;	// Initialize counter
	
	/*	Set Timer/Counter Operation to Mode 5
		Count up to top value then count down 	*/
	TCCR0A = TCCR0A | ((1 << COM0B1) | (0 << COM0B0) | (0 << COM0A1) | (0 << COM0A0) | (0 << WGM01) | (1 << WGM00));
	
	//	no prescaling 
	TCCR0B = TCCR0B | ((1 << WGM02) | (0 << CS02) | (0 << CS01) | (1 << CS00));
	
	//	enable timer0 overflow interrupt 
	TIMSK0 = TIMSK0 | ((1 << TOIE0));
	
	OCR0A = TOP;				// set top value that timer0 counts to 
	OCR0B = COMPARE;			// compare value that controls fan speed 

	DDRD = DDRD | (1 << 5);		// set pd5 for pwm output 
	DDRD = DDRD | (0 << 2);		// set pd2 for button input 
	DDRD &=  ~(1 << PIND2);		// pd3 tach input 
	DDRB |= (1 << PORTB2);		//pb2 for buzzer output 
}

// Calculate RPM
float getRPM() {
	float rpm;
	float t;					// Period
	
	t = tachNum * (1e-6);	// Convert microseconds to seconds 
	t = t * 2.0;				// Multiply by two to get a full revolution
	rpm = 60.0 / t;				// Convert into minutes
	
	return rpm;
}


// Calculate and display duty cycle
void showDutyCycle() {
	char buffer[14];
	float duty;
	int dutyInt, dutyDecimal;
	
	duty = (100.0) * (float)((uint8_t)COMPARE) / 200.0;
	dutyInt = (int)duty;
	dutyDecimal = (int)(duty * 100) % 100;
	
	sprintf(buffer, "Duty = %2d.%d (%%%%)  ", dutyInt, dutyDecimal);

	row2();
	printf(buffer);
}
	
// Checks for changes with RPG
void checkRPG() {
	if(bit_is_clear(PINB, 0) && bit_is_clear(PINB, 1)) {
		while(bit_is_clear(PINB, 0) && bit_is_clear(PINB, 1)) {}
			
			// Clockwise
			if(bit_is_clear(PINB, 1)) {
				if(COMPARE != 0) {
					COMPARE--;					
				}
			}
			// Counter clockwise 
			else if(bit_is_clear(PINB, 0)) {
				COMPARE++;
			}
			if(COMPARE > TOP) {COMPARE = TOP;}
			if (COMPARE < 0) {COMPARE = 0;}
				
			OCR0B = COMPARE;
	}
	
}


//make noise, set PB2 high 
void beep(){
	PORTB |= (1 << PORTB2);
}

//wait for button release 
void handleButton(void){
	while((PIND & (1 << PIND2)) == 0){	
	}
}



// Interrupt handler for RPM 
ISR(INT1_vect) { 
	stall = 0;				// Set STALL back to false to no longer show message 
	lcdMode = lcdModeTemp;  //restore mode that was on lcd prior to stall 
	PORTB &= ~(1 << PORTB2);//turn off the buzzer 
	uint8_t sreg;			
	stop16BitTimer();		// Stop 16 bit timer
	sreg = SREG;			// Store status register 
	cli();					// Disable global interrupts
	tachNum = TCNT1;		// save num of ticks from timer1
	TCNT1 = 0x0000;			// reset timer1
	SREG = sreg;			// restore status register 
	start16BitTimer();		// start 16 bit timer 
}


// Display stall message when timer1 overflows
ISR(TIMER1_OVF_vect) {
	stall = 1; 			
}

// Interrupt handler for COMPARE
ISR(TIMER0_OVF_vect) {
	OCR0B = COMPARE;
}

//main program loop 
int main(void) {
	clock_prescale_set(clock_div_2);	// For timer clock
	
	stdout = &lcd_str;		
	lcd_init();					// Initialize LCD
	
	pwmInit();							// Initialize Timer0 for PWM signals

	// Configure and Enable Timer/Counter1 Overflow Interrupt
	EIMSK |= (1 << INT1);				// External pin interrupt enabled
	EICRA |= (1 << 3) | (1 << 2);		// ISC11 = 1, ISC10 = 1; The rising edge of INT1 generates an interrupt request
	TIMSK1 = TIMSK1 | (1 << 0);			// Timer/Counter1 overflow interrupt is enabled
	
	
	sei();								// Enable global interrupts
	start16BitTimer();					// Start 16 bit timer

	DDRB = DDRB | (0 << 0);				// Set PB0 as input for RPG
	DDRB = DDRB | (0 << 1);				// Set PB1 as input for RPG
	
	
	
	 lcdMode = 0;		//set lcd display mode (rmp + duty)
	 lcdModeTemp = lcdMode;	//save current lcd mode incase of a stall
	 home();
	 printf("RPM = %d", (int)getRPM());	
	 row2();
	 showDutyCycle();
	
	// Main display loop 
	while (1) {	
		
		
		//handle switching modes
		if((PIND & (1 << PIND2)) == 0){
			handleButton();
			clear();
			if(lcdMode == 0){ //set to rpm + fan status mode 
				lcdMode = 1;
				lcdModeTemp = lcdMode;
			
			}
			else{ //set to rpm + duty cycle mode 
				lcdMode = 0;
				lcdModeTemp = lcdMode;
				
			}
		}
		
		//display stall screen when fan is stalled 
		if(stall == 1){
			home();
			printf("FAN STALLING    ");
			row2();
			printf("                ");
			beep();
			lcdMode = 3; //change lcd mode so only fan stall message appears 
		}
		
		//display rpm and duty cycle 
		if(lcdMode == 0){
			
			home();
			printf("RPM = %d", (int)getRPM());
			printf("    ");
			row2();
			showDutyCycle();
		}
		//display rpm and fan status 
		if(lcdMode == 1){
			
			int curRpm = (int)getRPM();
			home();
			printf("RPM = %d", curRpm);
			printf("   ");
			row2();
			if(curRpm >= 2400){
				printf("Fan  OK");
			}else{printf("Fan Low");
			
			}

		}
		
		checkRPG();
	}
}
