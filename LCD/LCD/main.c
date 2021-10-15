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
#include <avr/interrupt.h>
#include <stdbool.h>
#include <compat/twi.h>
#include <stdio.h>
#include <string.h>
#include "LCD_LIB/i2cmaster.h"

#define BAUD 9600
#define ADS1115_ADDR 0x48
#define ADS1115_REG_CONVERSION 0x00
#define ADS1115_REG_CONFIG 0x01


/*DECLARATIONS*/
void uart_init(uint8_t baud);
void uart_transmit(unsigned char data);
unsigned char uart_receive(void);
void uart_newline(void);

void ads1115_write(uint8_t addr, uint8_t pointerReg, uint16_t configReg);
uint16_t ads1115_read(uint8_t addr, uint8_t pointerReg);
uint16_t ads1115_read_SE(uint8_t addr, uint16_t configReg);
int16_t ads1115_read_DIFF_A2_A3(uint8_t addr, uint16_t configReg);
uint32_t sum, final;

int current_level = 4;
int current_mode;
int hold  = 0;

/*INTERRUPT HANDLER*/
ISR(PCINT1_vect){
	/*check what button was pressed*/
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
			
			if((PINC && PINC3) == 0){
				_delay_ms(250);
				if(hold == 0){
					hold = 1;
				} else {
					hold = 0;
				}
			}
}


/* BRIGHTNESS STUFF*/
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
	lcd_init(LCD_DISP_ON);
	/* set bits 6 as outputs*/
	DDRD = (1<<PORTD6);
	
	/* set port B as output*/
	 DDRC &= 0xF9;
	
	/* relevant bitshift to activate pullup resistor*/
	PORTC = (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2)| (1 << PORTC3); 

	/*clear the LCD screen*/
	lcd_clrscr();

	/*create the bar and relevant messages*/    
	 // int current_level = 4;
	 int current_mode = 1;
	
	/*Set Up Interrupts*/
	PCICR = (1<<PCIE1);
	
	//Trigger interrupts on changes to the following pins (C0, C1, C2, C3)
	PCMSK1 = (1<<PCINT8)|(1<<PCINT9)|(1<<PCINT10)|(1<<PCINT11);
	
	
	/* set output compare values*/
	OCR0A = 0;
	change_mode(current_mode);
	change_brightness(current_level);
	
	/* set up timer/counter 0 for past PWM, set on compare match */
	TCCR0A = (1<<COM0A0)|(1<<COM0A1)|(1<<WGM01)|(1<<WGM00);
	TCCR0B = (1 << CS00);
/*UART I2C INIT*/
	//uint8_t UBBRValue = FCPU/(16*BAUD)-1;
	
	uint8_t UBBRValue = 49;
	uart_init(UBBRValue);
	i2c_init();
	uint16_t ads1115Config = 0b11 << 0 | 0b111 << 5 | 0b01 << 8 | 0b010 << 9 | 0b011 << 12 | 0b01 << 15;
	
	int16_t dataBinary;
	double dataVoltage, sum, final;
	char buff[16];
	char voltageString[8];
	char volt[3] = "V";
	sei();



    /* Replace with your application code */
    while (1) 
    {
		

		
						
		/*************************************************************/
		/*					AC VOLTAGE								 */
		/*
		sum = 0;
		for (int i = 0; i < 200; i++) {
			dataBinary = ads1115_read_DIFF_A2_A3(ADS1115_ADDR, ads1115Config);
			dataVoltage = ((double)dataBinary-32.0)/1006.4;
			
			sum += (dataVoltage*dataVoltage);
		}
		
		sum = sum/200;
		final = sqrt(sum);
		
		dtostrf(final, 4, 3, voltageString);
		for (int i = 0; i < sizeof(voltageString); i++) {
			uart_transmit(voltageString[i]);
		}
				uart_newline();
				lcd_gotoxy(0,1);
				strcat(voltageString,volt)
				lcd_puts(voltageString);
				*/
	}
}


void uart_newline(void) {
	uart_transmit('\n');
	uart_transmit('\r');
	return;
}
/*******************************************************************/
/*                      UART FUNCTIONS                             */
void uart_init(uint8_t ubrr) {
	/*initialise uart on ATMega328P*/
	UBRR0H = (unsigned char) (ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	
	//UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	//UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	UCSR0B = (1<<TXEN0);
	UCSR0B |= (1 << RXCIE0) | (1<< RXEN0);
	
	UCSR0C = (1<<USBS0) | (3<<UCSZ00);
	
}
void uart_transmit(unsigned char data) {
	/* Wait for empty transmit buffer */
	while (!( UCSR0A & (1<<UDRE0)));
	/* Put data into buffer, sends the data */
	UDR0 = data;
}
unsigned char uart_receive(void) {
	/* Wait for data to be received */
	while (!(UCSR0A & (1<<RXC0)));
	
	/* Get and return received data from buffer */
	return UDR0;
}
ISR(USART_RX_vect) {
	unsigned char dummy;
	dummy = uart_receive();
	uart_transmit(dummy);
}


/*******************************************************************/
/*                    ADS1115 FUNCTIONS                            */
void ads1115_write(uint8_t addr, uint8_t pointerReg, uint16_t configReg) {
	/*
	 * write bytes to the config register for reading after
	 */
	//send start condition and wait
	i2c_start_wait((addr << 1) + I2C_WRITE);
	//write to pointer reg - config register setting
	i2c_write(pointerReg);
	//write data for config reg
	i2c_write(configReg >> 8);
	i2c_write((configReg && 0xFF));
	//send stop condition
	i2c_stop();
	
	return;	
}
uint16_t ads1115_read(uint8_t addr, uint8_t pointerReg) {
	/*
	 * read from conversion register from ads1115
	 */ 
	//send start condition and wait
	i2c_start_wait((addr << 1) + I2C_WRITE);
	//write to the point reg - conversion register setting
	i2c_write(pointerReg);
	//send stop
	i2c_stop();
	//read 16 bit adc conversion
	i2c_rep_start((addr << 1) + I2C_READ);
	uint8_t MSB = i2c_readAck();
	uint8_t LSB = i2c_readNak();
	//send stop
	i2c_stop();
	
	uint16_t data = (MSB << 8 | LSB);
	return data;
}
uint16_t ads1115_read_SE(uint8_t addr, uint16_t configReg) {
	/*
	 * Read from channel 0 on ads1115 for a given config reg
	 */
	ads1115_write(addr, ADS1115_REG_CONFIG, configReg);
	_delay_ms(8);
	return ads1115_read(addr, ADS1115_REG_CONVERSION);
}
int16_t ads1115_read_DIFF_A2_A3(uint8_t addr, uint16_t configReg) {
	/* 
	 * Read from channel 2 and 3 in diff mode
	 */
	ads1115_write(addr, ADS1115_REG_CONFIG, configReg);
	//_delay_ms(8);
	return (int16_t)ads1115_read(addr, ADS1115_REG_CONVERSION);
	
}


