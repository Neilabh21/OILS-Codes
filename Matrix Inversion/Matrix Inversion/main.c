/*
 * Matrix Inversion.c
 *
 * Created: 31-03-2019 23:07:58
 * Author : Neilabh
 */ 

#include <avr/io.h>

#include <avr/delay.h>

#include <avr/interrupt.h>

#define  F_CPU 32000000

/*! \brief Enable high-level interrupts. */
#define PMIC_EnableHighLevel() (PMIC.CTRL |= PMIC_HILVLEN_bm)

#include<math.h>
volatile uint8_t overflow_count;

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


float determinant(float [][3], float);
void cofactor(float [][3], float);
void transpose(float [][3], float [][3], float);
float determinant(float a[3][3], float k)
{
	float s = 1, det = 0, b[3][3];
	if (k == 1)
	{
		return (a[0][0]);
	}
	else
	{
		det = 0;
		for (int c = 0; c < k; c++)
		{
			int m = 0;
			int n = 0;
			for (int i = 0;i < k; i++)
			{
				for (int j = 0 ;j < k; j++)
				{
					b[i][j] = 0;
					if (i != 0 && j != c)
					{
						b[m][n] = a[i][j];
						if (n < (k - 2))
						n++;
						else
						{
							n = 0;
							m++;
						}
					}
				}
			}
			det = det + s * (a[0][c] * determinant(b, k - 1));
			s = -1 * s;
		}
	}

	return (det);
}

void cofactor(float num[3][3], float f)
{
	float b[3][3], fac[3][3];
	for (int q = 0;q < f; q++)
	{
		for (int p = 0;p < f; p++)
		{
			int m = 0;
			int n = 0;
			for (int i = 0;i < f; i++)
			{
				for (int j = 0;j < f; j++)
				{
					if (i != q && j != p)
					{
						b[m][n] = num[i][j];
						if (n < (f - 2))
						n++;
						else
						{
							n = 0;
							m++;
						}
					}
				}
			}
			fac[q][p] = pow(-1, q + p) * determinant(b, f - 1);
		}
	}
	transpose(num, fac, f);
}

/*Finding transpose of matrix*/
void transpose(float num[3][3], float fac[3][3], float r)
{
	float b[3][3], inverse[3][3], d;

	for (int i = 0;i < r; i++)
	{
		for (int j = 0;j < r; j++)
		{
			b[i][j] = fac[j][i];
		}
	}
	d = determinant(num, r);
	for (int i = 0;i < r; i++)
	{
		for (int j = 0;j < r; j++)
		{
			inverse[i][j] = b[i][j] / d;
		}
	}
}
ISR(TCC0_OVF_vect)
{
	//<add code to increment a variable which keeps a record of number of overflows, total time count will be this variable * 256 + value of a>
	overflow_count+=1;
	//return(overflow_count);
}

int main(void)
{
    /* Replace with your application code */
	//Overflow timer
	
	PMIC_EnableHighLevel();				//Enable interrupts : High level for timer
	TCC0.CTRLA = TC_CLKSEL_DIV1024_gc;		//Set Prescaler 1024(Same as CPU_PRESCALER)
	TCC0.CTRLB= TC_WGMODE_NORMAL_gc;    //Wave generation mode : Normal
	TCC0.INTCTRLA = TC_OVFINTLVL_HI_gc;	//Enable overflow interrupt
	TCC0.PER = 0xFF;		    //Initialize Period
	
	sei();
	
	USARTXInit();
	
	uint8_t a;
	
    while (1) 
    {
		
		float k=3, d;

		float abc[3][3] = {1,1,0,0,1,0,0,0,1};
			
		TCC0.CNT = 0; // Offset by 04 93, time calculated by 1 iteration is 5ms
		
		for(int j = 0; j < 6; j++){

			d = determinant(abc, k);
			
			cofactor(abc, k);
			
			
			a = TCC0.CNT;
			USARTXTrans(0X00);
			USARTXTrans(overflow_count);
			USARTXTrans(a);
			USARTXTrans(0X00);
		}
		
		
		a = TCC0.CNT;
		USARTXTrans(0Xff);
		USARTXTrans(overflow_count);
		USARTXTrans(a);
		USARTXTrans(0Xff);
		overflow_count = 0;
		_delay_ms(1000);

	}
}

