/*
 * MultimeterUartI2cTotal.c
 *
 * Created: 16/09/2021 12:20:40 PM
 * Author : tomsa
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <compat/twi.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "i2cmaster.h"

#define BAUD 9600
#define ADS1115_ADDR 0x48
#define ADS1115_REG_CONVERSION 0x00
#define ADS1115_REG_CONFIG 0x01





void uart_init(uint8_t baud);
void uart_transmit(unsigned char data);
unsigned char uart_receive(void);
void uart_newline(void);

void ads1115_write(uint8_t addr, uint8_t pointerReg, uint16_t configReg);
uint16_t ads1115_read(uint8_t addr, uint8_t pointerReg);
uint16_t ads1115_read_SE(uint8_t addr, uint16_t configReg);
int16_t ads1115_read_DIFF_A2_A3(uint8_t addr, uint16_t configReg);
uint32_t sum, final;

int main(void) {
	//uint8_t UBBRValue = FCPU/(16*BAUD)-1;
	uint8_t UBBRValue = 49;
	uart_init(UBBRValue);
	i2c_init();
	uint16_t ads1115Config = 0b11 << 0 | 0b111 << 5 | 0b01 << 8 | 0b010 << 9 | 0b011 << 12 | 0b01 << 15;
		
	int16_t dataBinary;
	float dataVoltage;
	char buff[16];
	char voltageString[8];
	sei();

	while (1) {
		
		/**************************************************************/
		/*					DC VOLTAGE								  */
		
		dataBinary = ads1115_read_DIFF_A2_A3(ADS1115_ADDR, ads1115Config);
		sprintf(buff, "%d", dataBinary);
		
		dataVoltage = (dataBinary-32)/1006.4;
		dtostrf(dataVoltage, 4, 3, voltageString);
		for (int i = 0; i < sizeof(voltageString); i++) {
			uart_transmit(voltageString[i]);
		}
		
		
		/*************************************************************/
		/*					AC VOLTAGE								 */
		/*
		sum = 0;
		for (int i = 0; i < 1600; i++) {
			dataBinary = ads1115_read_DIFF_A2_A3(ADS1115_ADDR, ads1115Config);
			dataVoltage = ((float)dataBinary-32.0)/1006.4;
			sum += (dataVoltage*dataVoltage);			
		}
		sum = sum/1600;
		final = sqrt(sum);
		dtostrf(dataVoltage, 4, 3, voltageString);
		for (int i = 0; i < sizeof(voltageString); i++) {
			uart_transmit(voltageString[i]);
		}
		*/
		
		
		uart_newline();
	//	_delay_ms(1000);
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