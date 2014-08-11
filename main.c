#include <p18F87J11.h>
#include <delays.h>
#include "LCD.h"

/*configuration bits*/
#pragma config DEBUG = ON, XINST = OFF, STVREN = ON, WDTEN = OFF
#pragma config CP0 = ON
#pragma config IESO = ON, FCMEN = ON, FOSC = ECPLL
#pragma config WDTPS = 1
#pragma config WAIT = OFF, BW = 16, MODE = MM, EASHFT = ON
#pragma config MSSPMSK = MSK7, PMPMX = DEFAULT, ECCPMX = DEFAULT,CCP2MX = DEFAULT


#define SWITCH_0 PORTBbits.RB0
#define SWITCH_1 PORTAbits.RA5

#pragma idata
unsigned char string[176]={"                "					//num play 0-15
						"                "					//num play 16-29
						"   Start Game?  "					//init msg
						"  PRESS ANYKEY  "					//init msg
						"WRONG,try again?"					//wrong msg
						"RB0->y || RA5->N"					//wrong msg
						"Congratulations!"					//win msg
						"     YOU WIN!!  "  				//win msg
						"*****CORRECT****"					//exit info
						"*****SEQUENCE***"				    //exit info
						"FINAL LENGTH    "};				//final length

unsigned char var[30] = {1};


#pragma code
/*prototypes*/
void string_write(unsigned char);
void release(void);
void lcd_write(char , char);

void main(void)
{
//	void* foo = 0, bar = 0, echo = 0, num = 0;
	unsigned char foo = 0, bar = 0, echo = 0, num = 0, error = 0;		//counter
	/*reset contents of string*/
	foo = 0;
	while(foo<30)
    {
        string[foo] = ' ';
        ++foo;
    }//end while


	//LCD init
	LCDInit();
	/*switch init*/
	/*RB0*/
	INTCON2bits.RBPU = 0; // enable PORTB internal pullups
	TRISBbits.TRISB0 = 1; // PORTB bit 0 (connected to switch) is input (1)

	/*RA5*/
	WDTCONbits.ADSHR = 1;
	ANCON0 = 0x1f;
	TRISAbits.TRISA5 = 1;

	/*prompt for start*/
	lcd_write(2,3);

    //hold the start info until the SWITCH being pressed
	while( SWITCH_0 == 1 && SWITCH_1 == 1)
		;
	release();

	//gen 30 numbers of 1 or 0 then store into var[]
	foo = 0;
	while(foo < 30)
	{
		num = rand2()%2;
		if(num==1)
			var[foo] = '1';
		else
			var[foo] = '0';
		++foo;
	}//end while


	//loop game 3 to 30
	bar = 3;
	/*-----------------------------------------------------------------------------------------------------------------------
	 *while loop switch check, for the current bar*/
	while( bar <= 3)				//total game process
	{



		start:
		
		/*display current string interval*/
		foo = 0;
		while(foo<bar)
		{
		
			string[0] = var[foo]; 

			/*display num for 0.8s*/
			lcd_write(0,1);
			Delay10KTCYx(80);
			
			/*display interval for 0.2s*/
			lcd_write(1,1);
			Delay10KTCYx(20);
				
			
			++foo;
		}//end while
		/*-----------------------------------------------------------------------------------------------------------------------
		 *switch check, for the current char(echo)*/
		
		echo = 0;
		while(echo < bar)
		{

			/*switch input*/
			while(1)
			{
		

				/*switch input CORRECT---------------------------------------------------------*/
				if((SWITCH_0 == 0&& var[echo]=='0')||(SWITCH_1 == 0&&var[echo]=='1'))
				{
					release();
					break;
				}//end if

				/*switch input WRONG-----------------------------------------------------------*/
				else if((SWITCH_0 == 0&& var[echo]=='1')||(SWITCH_1 == 0&&var[echo]=='0')||(SWITCH_0 == 0&&SWITCH_1 == 0))	
				{
					release();
					/*increment error count*/
					error++;

					/*display wrong msg*/
					lcd_write(4,5);
					
					/*end the game when second error occur*/
					if(error >= 2)
						goto error;

					/*pending switch input------------------------------------------------*/
					Delay10KTCYx(50);
					while(1)
					{
						/*play again with same bar length---------------------*/
						if(SWITCH_0 == 0)		//play again with same length
						{
							release();
							goto start;
						}//end if

						/*end of the game, starting new round-----------------*/
						else if(SWITCH_1 == 0)	//end of game, new round
						{
							release();
							error:
							/*length-------------------------------------*/
							/*determine length & renew string*/
							if(bar/10==0)
							{
								string[175] = bar%10+'0';	
							}//end if
							else
							{
								string[174] = bar/10+'0';
								string[175] = bar%10+'0';
							}//end else

							/*display & delay*/
							lcd_write(10,10);
							Delay10KTCYx(200);

							/*display exit answer info------------------*/
							lcd_write(8,9);	
							Delay10KTCYx(200);
							
							/*renew correct sequence*/
							num = 0;
							while(num<30)
							{
								string[num] = var[num];
								++num;
							}//end while

							/*display correct sequence--------------------*/
							lcd_write(0,1);
							Delay10KTCYx(200);

							goto end;
						}//end else if
					}//end while

					break;
				}//end else if
				else if(SWITCH_0 == 1 && SWITCH_1 == 1)			//unpressed
					continue;

			}//end while


			/*check right -> continue move on to the next char*/
			++echo;

		}//end while

		/*increment the length of bar and continue-----------------------------------------------*/
		++bar;
		/*reset error count*/
		error = 0;

	}//end while


	//end display
	lcd_write(6,7);
	Delay10KTCYx(100);

	/*renew correct sequence*/
	num = 0;
	while(num<30)
	{
		string[num] = var[num];
		++num;
	}//end while

	/*pending for switch input and flicking LED, display correct sequence*/
	TRISD = 0;
	while(SWITCH_0 == 1 && SWITCH_1 == 1)
	{
		lcd_write(0,1);
		LATD = 0xff;
		Delay10KTCYx(3);
		LATD = 0;
		Delay10KTCYx(3);
	}//end while
	release();
	LATD = 0;
	end:
	main();

}//end main

void lcd_write(char foo, char bar)
{
	LCDLine_1();
	string_write(foo);
	LCDLine_2();
	string_write(bar);

}//end lcd_write

/*call function release*/
void release(void)
{
	/*	this function halts execution 
	 *until switchs are released
	 */
	
	unsigned char count = 0;

	
	while(1)
	{
		
		Delay100TCYx(100);
		if(SWITCH_0==1&&SWITCH_1==1)
			count++;
		else
			count = 0;
		

		if(count >= 5)
			break;

	}//end while

}//end release


//******************************************************************
// Write a string on LCD
//******************************************************************
void string_write(unsigned char idx)
{
	unsigned int i=0;
	unsigned int z=0;
	char b;

	while (i<16)
	{
		z = idx*16;
		z = z + i;
		b = string[z];
		data_write(b);
		Delay10TCYx(50);
		i++;
	}//end while

}//end string_write











