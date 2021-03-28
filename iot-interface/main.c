#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "ds18b20.h"

// definition of registers see atmega docs page 218
// UCSRnA - 7 6 5 4 3 2 1 0 - used for reading flags
//          
// UCSRnB - 7 6 5 4 3 2 1 0 - set Rxen and Txen 
//          0 0 0 1 1 0 0 0
//
// UCSRnC - 7 6 5 4 3 2 1 0 - asynchronous, set 8 bit, 1 stop bit, no parity
//          0 0 0 0 0 1 1 0 

void uartInit(void)
{
  // DDRE   = 0b00000000;
  // double baud rate
  UCSR0A = 0b00000010;
  UCSR0B = 0b00011000;
  UCSR0C = 0b00000110;
  //set to 12 - baud rate will be 9600
  UBRR0L = 0b00001100; 
  UBRR0H = 0b00000000;
}

unsigned char uartRxChar(void) {
  while ((UCSR0A & (1 << RXC0)) == 0);
  return(UDR0);			
}

void uartTxChar(char ch) {
  while (! (UCSR0A & (1<<UDRE0)));	
  UDR0 = ch ;
}

void uartTxString(char *str) {
  unsigned char j=0;

  while (str[j]!=0)	{
    uartTxChar(str[j]);	
    j++;
  }
}

void adcInit(void) {
  DIDR0 = 0x00;
  // set PA0-PA7 to input
  // DDRA = 0x00;
  // enable ADC with freq/128  
  ADCSRA = 0x86;
  // free running  
  ADCSRB = 0x00;  
  // set Vref at Vcc and ADC channel 0
  ADMUX  = 0b01000000;  
}

int adcRead(char channel) {
  int hi,lo;

  ADMUX = 0b01000000 | (0b00000111 & channel);
  _delay_us(500);
  // start conversion
  ADCSRA |= (1<<ADSC); 
  // wait for conversion to complete
  while (!(ADCSRA & (1<<ADSC))){}
  _delay_us(500);
  lo = ADCL;
  hi = ADCH;
  return (hi << 8) + lo;
}

int main(void) {
  int t,x,y,z = 0;
  char buf[50];

  uartInit();
  adcInit();

  _delay_ms(100);
  sei();

  while(1) {
    // wait to start measurement
    uartRxChar();
    _delay_ms(10);
    // take accelerometer reading for x,y,z co ordinates
    x = adcRead(0);
    y = adcRead(1);
    z = adcRead(2);
    // take dsb1820 temp
    t = ds18b20Gettemp();
    sprintf(buf,"%d %d %d %d\n\r",t,x,y,z);
    // send to serial usb
    uartTxString(buf);	
  }
  return 0;  
}
