/*
 * LCD.c
 *
 * Created: 7/09/2021 2:57:07 PM
 * Author : mayvin
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "LCD_LIB/lcd.h"
#include <stdint.h>
#define F_CPU 16000000UL	// 16MHz


	void change_brightness (int level) {
		
		if (level <= 4){
			/* clear the bar */
			for (int i = 3; i >= 0; i--){
				lcd_gotoxy(19,i);
				lcd_puts(" ");
			}
			
			/* fill it up to the level desired */
			for (int i=4; i >= 4-level; i--) {
				lcd_gotoxy(19,i);
				lcd_putc(255);
			}
		}
	}

	void change_mode(int mode){
			if(mode == 1){
				lcd_clrscr();
				lcd_gotoxy(0,0);
				lcd_puts("Mode: V(DC)");
			}
			else if (mode == 2){
				lcd_clrscr();
				lcd_gotoxy(0,0);
				lcd_puts("MODE:V(AC)");
			}
			else if (mode == 3){
				lcd_clrscr();
				lcd_gotoxy(0,0);
				lcd_puts("MODE:Resistance");
			} 
			else if (mode == 4){
				lcd_clrscr();
				lcd_gotoxy(0,0);
				lcd_puts("MODE:Continuity");
			}
	}

int main(void)
{	
	lcd_init(LCD_DISP_ON_CURSOR);
	/* set bits 6 as outputs*/
	DDRD = (1<<PORTD6);
	
	/* set port B as output*/
	 DDRC &= 0xF9;
	
	/* relevant bitshift to activate pullup resistor*/
	PORTC = (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2); 
	



	/*clear the LCD screen*/
	lcd_clrscr();

	/*create the bar and relevant messages*/    
	 int current_level = 4;
	 int current_mode = 1;
	
	/* set output compare values*/
	OCR0A = 0;
	change_mode(current_mode);
	change_brightness(current_level);
	
	/* set up timer/counter 0 for past PWM, set on compare match */
	TCCR0A = (1<<COM0A0)|(1<<COM0A1)|(1<<WGM01)|(1<<WGM00);
	TCCR0B = (1 << CS00);
	
    /* Replace with your application code */
    while (1) 
    {
		/* if the pin is low decrease the brightness */
		if((PINC & (1 << PINC0)) == 0){
			_delay_ms(250);
			if (current_level - 1 <= 0 && OCR0A + 63 > 251){
				current_level = 0;
				OCR0A = 255;
			} else {
				current_level --;
				OCR0A = OCR0A + 63;
			}
			change_brightness(current_level);
		}
			
			
		/* if the pin is high increase the brightness */
		if((PINC & (1 << PINC1)) == 0){
			_delay_ms(250);
			if(current_level + 1 >= 4 && OCR0A - 63 <= 3){
				current_level = 4;
				OCR0A = 0;
			} else {
				current_level ++;
				OCR0A = OCR0A - 63;
			}
			change_brightness(current_level);
		}
		
		/* if pin changes change mode*/
		if((PINC & (1<<PINC2)) == 0){
			_delay_ms(250);
			if(current_mode + 1 <= 4){
				current_mode++;
			} else {
				current_mode = 1;
			}
			change_mode(current_mode);
			change_brightness(current_level);
		}
	}
}

