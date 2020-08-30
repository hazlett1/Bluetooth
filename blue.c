#ifndef F_CPU
#define F_CPU 16000000 //Sets the CPU top 16 Megahertz
#endif

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define DEBOUNCE_TIME 25 //Used to make sure the button was supposed to be pressed
#define BAUD_RATE 103 //9600 for a clock of 16 MHz

void uart_init();
//void adc_init();
//uint16_t adc_read();
void uart_sendChar(char data);
void uart_sendStr(char *data);
//void buttons_init();
//void adc_send(char *data);

int main(void){
	
	//Initialize uart control
	uart_init();
	//buttons_init();
	//adc_init();
	
//	uint16_t str;
	while(1){			
		/*if(!(PINA & 0x01)){
			_delay_ms(DEBOUNCE_TIME);
			if(!(PINA & 0x01)){
				uart_sendChar(str);
				str++;
			}
		}*/		
		
	}
	return 0;
}

ISR(USART_RXC_vect){
	char data = UDR;
	if(data == 'h'){
		PORTA |= 0x80;
	}
	else if(data == 'l'){
		PORTA &= ~(0x80);
	}
	uart_sendChar(data);
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
	while(!(UCSRA & 0x20));
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
	
	//Sets PORTC to input & output (0 INPUT, 1 OUTPUT)
	//DDRA = 0xFE;
	
	//Sets PORTC intial value to LOW (0 LOW, 1 HIGH)
	//PORTA = 0x01;
	
	/*//Disable JTAG twice for regular PORTC i/o
	MCUCSR |= (1 << JTD);
	MCUCSR |= (1 << JTD); */
}
//void adc_init(){
	
	//Sets ADC0 & ADC1 to output
//	DDRA = 0xFC;
		
//	PORTA = 0x02;
	
	//Sets ADC voltage reference to AVCC and D- ADC0, D+ ADC1
//	ADMUX = 0x49;
	
	//Enables ADC and sets division factor to 128
//	ADCSRA = 0x87;
	
//}


//uint16_t adc_read(){

//	uint8_t low; //Will collect the left hand bits 0 - 7
//	uint16_t adc; //Will combine the right hand and left hand bits 9 & 8 and 7 - 0

//	ADCSRA |= (1 << ADSC); //ADSC - Analog Digital Start Conversion
//	while(ADCSRA & (1 << ADSC)); //Loop around until conversion is finished

//	low = ADCL; //Set the left hand bits to low
//	adc = (ADCH << 8) | low; //Combine all the bits together

//	return adc; //Returns the number 0 - 1024
//}

//void adc_send(char *data){
	
//	DDRA |= 0x02;
	
//	while(*data != '\0'){
		
//		ADCH = 0b00;
//		ADCL = data;
		
//		ADCSRA |= (1 << ADSC); //ADSC - Analog Digital Start Conversion
//		while(ADCSRA & (1 << ADSC)); //Loop around until conversion is finished
		
//	}
		
//	DDRA &= ~(0x02);
//}
//Send a byte over USART to device

