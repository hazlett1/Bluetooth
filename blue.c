#ifndef F_CPU
#define F_CPU 16000000 //Sets the CPU top 16 Megahertz
#endif

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>

#define DEBOUNCE_TIME 25 //Used to make sure the button was supposed to be pressed
#define BAUD_RATE 51  //19.2k for a clock of 16 MHz
#define DEBUG_ON PORTC |= 0x02
#define DEBUG_OFF PORTC &= ~(0x02);

void uart_init();
void adc_init();
int adc_read(unsigned char admuxVal);
void adc_readData();
void uart_sendChar(char data);
void uart_sendStr(char *data);
void buttons_init();

unsigned char READING_UART_FLAG = 0, READ_UART = 0, READ_ADC_FLAG = 0, READING_ADC = 0, SEND_TITLE = 0;

int main(void){
	
	uart_init();
	buttons_init();
	adc_init();
	
	while(1){	
		
		int adc = adc_read(0x00);
		_delay_ms(100);
		
		if(adc > 700){
			DEBUG_ON;
		} else {
			DEBUG_OFF;
		}
				
		if(READ_UART){
			if (READ_ADC_FLAG) {
				READ_ADC_FLAG = 0;
				READING_ADC = 1;
				adc_readData();
				READING_ADC = 0;
			}
			else if(SEND_TITLE){
				uart_sendStr("adc0\tadc1\tadc2\tadc3\tadc4");
				uart_sendStr("____\t____\t____\t____\t____");
				SEND_TITLE = 0;
			}
			else {
				if(PINC & (1 << PC0))
					uart_sendStr("Turning light on!");
				else 
					uart_sendStr("Turning light off!");	
			}
			READ_UART = 0;
		}
	}
	return 0;
}

ISR(USART_RXC_vect){
	READING_UART_FLAG = 1;
	char data = UDR;
	if(data == 'h'){
		PORTC |= 0x01;
	}
	else if(data == 'l'){
		PORTC &= ~(0x01);
	} 
	else if(data == 'r'){
		READ_ADC_FLAG = 1;
	}
	else if(data == 't'){
		SEND_TITLE = 1;
	}
	else if(data == '\n'){
		READING_UART_FLAG = 0;
		READ_UART = 1;
	}
}


//USART device initialization for bluetooth to device
void uart_init(){
	
	unsigned int ubrr = BAUD_RATE;
	
	//Sets the 12-bit baud rate
	UBRRH = (ubrr>>8);
	UBRRL = (ubrr&0xFF);
	
	//Enables TXEN & RXEN & RX interrupts
	UCSRB = 0x98;
	
	//Sets URSEL to write to UCSRC and sets 8-bit transfer with 1 stop bit
	UCSRC = 0x86;	
	
	sei();
}

void uart_sendChar(char data){
	//Waits until it can transmit again then sends data
	while(!(UCSRA & 0xA0));
	UDR = data;
}

//Sends a string to device
void uart_sendStr(char *data){
	while(*data != '\0'){
		uart_sendChar(*data);
		data++;
	}
	uart_sendChar('\r');
	uart_sendChar('\n');
	_delay_ms(100);
}
void buttons_init(){
	
	//Disable JTAG twice for regular PORTC i/o
	MCUCSR |= (1 << JTD);
	MCUCSR |= (1 << JTD);
	
	//Sets PORTC to input & output (0 INPUT, 1 OUTPUT)
	DDRC = 0xFF;
	
	//Sets PORTC intial value to LOW (0 LOW, 1 HIGH)
	PORTC = 0x00;
}
void adc_init(){
	
	//Sets ADC0-4 to input
	DDRA = 0x00;
		
	//Sets PORTA0-4 to High
	PORTA = 0xFF;
	
	//Sets ADC reading to ADC0 first
	ADMUX = 0x00;
	
	//Enables ADC & sets division factor to 128
	ADCSRA = 0x87;
	
}

void adc_readData(){
	int adcVals[5];
	char str[50];
	for(unsigned char admuxVal = 0; admuxVal < 5; admuxVal++){
		adcVals[admuxVal] = adc_read(admuxVal);
		_delay_ms(50);
	}
	sprintf(str, "  %d  \t%d  \t%d  \t%d  \t%d", adcVals[0], adcVals[1], adcVals[2], adcVals[3], adcVals[4]);
	uart_sendStr(str);
}

int adc_read(unsigned char admuxVal){

	uint8_t low; //Will collect the left hand bits 0 - 7
	int adc; //Will combine the right hand and left hand bits 9 & 8 and 7 - 0

	ADMUX = admuxVal;

	ADCSRA |= (1 << ADSC); //ADSC - Analog Digital Start Conversion
	while(ADCSRA & (1 << ADSC)); //Loop around until conversion is finished

	low = ADCL; //Set the left hand bits to low
	adc = (ADCH << 8) | low; //Combine all the bits together

	return adc; //Returns the number 0 - 1024
}

