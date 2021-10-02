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


	void change_brightness (int level) {
		
		if (level <= 5){
			/* clear the bar */
			for (int i = 0; i < 5; i++){
				lcd_gotoxy((11 + i),4);
				lcd_puts(" ");
			}
			
			/* fill it up to the level desired */
			for (int i=0; i < level; i++) {
				lcd_gotoxy((11 + i),4);
				lcd_putc(255);
			}
		}
	}


int main(void)
{	
	lcd_init(LCD_DISP_ON_CURSOR);
	
	/* set port B as output*/
	 DDRC &= 0xF9;
	
	/* relevant bitshift to activate pullup resistor*/
	PORTC = (1 << PORTC0) | (1 << PORTC1); 
	



	/*clear the LCD screen*/
	lcd_clrscr();

	/*create the bar and relevant messages*/
	lcd_gotoxy(11,4);           
	lcd_putc(255);       
	lcd_gotoxy(12,4);         
	lcd_putc(255); 
	lcd_gotoxy(13,4);
	lcd_putc(255);
	lcd_gotoxy(14,4);
	lcd_putc(255);
	lcd_gotoxy(15,4);
	lcd_putc(255); 

	lcd_gotoxy(0,0);
	lcd_puts("Hi i am LCD");
	
	lcd_gotoxy(0,4);
	lcd_puts("Brightness:");
	
	int current_level = 5;
	
    /* Replace with your application code */
    while (1) 
    {
		/* if the pin is low decrease the brightness */
		if((PINC & (1 << PINC0)) == 0){
			_delay_ms(250);
			if (current_level - 1 == 0){
				current_level = 5;
			} else {
				current_level --;
			}
			change_brightness(current_level);
		}
			
			
		/* if the pin is high increase the brightness */
		if((PINC & (1 << PINC1)) == 0){
			_delay_ms(250);
			if(current_level + 1 > 5){
				current_level = 5;
			} else {
				current_level ++;
			}
			change_brightness(current_level);
		}
			
		
	}
	
 
	
}

