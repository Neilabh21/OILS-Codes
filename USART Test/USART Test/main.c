/*
 * USART Test.c
 *
 * Created: 31-03-2019 23:04:03
 * Author : Neilabh
 */ 

#include <avr/io.h>

void USARTXInit(void)
{
	PORTC_DIRSET=PIN3_bm;                                           //Setting TX pin of USARTC0 as output (PC3)
	PORTC_DIRCLR=PIN2_bm;                                           //Setting RX pin of USARTC0 as input (PC2)
	USARTC0_BAUDCTRLA=0x0C;                                         //BSEL should be 12 in decimal for selection of baud rate 9600 with given freq
	USARTC0_BAUDCTRLB=0;                                            //BSCALE = 0
	USARTC0_CTRLC=(USART_CHSIZE_8BIT_gc)|(USART_PMODE_DISABLED_gc); //8 bit transmission in one data frame, no parity and one stop bit
	USARTC0_CTRLB=(USART_RXEN_bm)|(USART_TXEN_bm);                  //Enabling the transmitter and receiver channels of USARTC0
}

void USARTXTrans(uint8_t dat)
{
	while(!(USARTC0_STATUS & USART_DREIF_bm));                      //While data register empty interrupt flag is not high, wait
	USARTC0_DATA=dat;                                               //Writing the desired value into the transmit buffer register
}

int main(void)
{
    /* Replace with your application code */
	
	USARTXInit();
	
    while (1) 
    {
		USARTXTrans('a');
    }
}

