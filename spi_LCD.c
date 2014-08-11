#include <p18F87J11.h>
#include <delays.h>

//*****************************************************************
// Functions can be called by the main
//*****************************************************************
void LCDInit(void);
void LCDLine_1(void);
void LCDLine_2(void);

//******END********************************************************


void InitSPI(void b);
void InitPORTA_SPI(char b);
void InitPORTB_SPI(char b);
void WritePortA(char b);
void WritePortB(char b);
void data_write(char b);
void command_write(char b);

//*****************************************************************
// Write to MCP923S17 Port A
//*****************************************************************
void WritePortA(char b)
{
LATAbits.LATA2 = 0;
SSPBUF = 0x40;
while(!PIR1bits.SSPIF);
PIR1bits.SSPIF = 0;
SSPBUF = 0x12;
while(!PIR1bits.SSPIF);
PIR1bits.SSPIF = 0;
SSPBUF = b;
while(!PIR1bits.SSPIF);
PIR1bits.SSPIF = 0;
LATAbits.LATA2 = 1;
}
//*****************************************************************
// Write to MCP923S17 Port B
//*****************************************************************
void WritePortB(char b)
{
LATAbits.LATA2 = 0;
SSPBUF = 0x40;
while(!PIR1bits.SSPIF);
PIR1bits.SSPIF = 0;
SSPBUF = 0x13;
while(!PIR1bits.SSPIF);
PIR1bits.SSPIF = 0;
SSPBUF = b;
while(!PIR1bits.SSPIF);
PIR1bits.SSPIF = 0;
LATAbits.LATA2 = 1;
}
//*****************************************************************
// Send data to the display
//*****************************************************************
void data_write(char b)
{
WritePortA(0x80);
WritePortB(b);
WritePortA(0xC0);
WritePortA(0x00);
}
//*****************************************************************
// Send a command to the display
//*****************************************************************
void command_write(char b)
{
WritePortA(0x00);
Delay10TCYx(0);
 
WritePortB(b);
Delay10TCYx(0);
 
WritePortA(0x40);
Delay10TCYx(0);
 
WritePortA(0x00);
}
//*****************************************************************
// Write to line 1 of the display
//*****************************************************************
void LCDLine_1(void)
{
command_write(0x80);
}
//*****************************************************************
// Write to line 2 of the display
//*****************************************************************
void LCDLine_2(void)
{
command_write(0xC0);
}
//******************************************************************
// Function to write to the PORT
//******************************************************************
void InitWrite(char b)
{
WritePortA(0);
WritePortB(b);
Nop();
Nop();
Nop();
WritePortA(0x40);
Nop();
Nop();
Nop();
Nop();
Nop();
Nop();
Nop();
Nop();
Nop();
WritePortA(0);
}
//*****************************************************************
// Initialize MCP923S17 Port A
//*****************************************************************
void InitPortA_SPI(char b)
{
LATAbits.LATA2 = 0;
SSPBUF = 0x40;
/*
SSP1IF: MSSP1 Interrupt Flag bit
1 = The transmission/reception is complete (must be cleared in software)
0 = Waiting to transmit/receive
*/
while(!PIR1bits.SSPIF);
PIR1bits.SSPIF = 0;
SSPBUF = 0x00;
while(!PIR1bits.SSPIF);
PIR1bits.SSPIF = 0;
SSPBUF = b;
while(!PIR1bits.SSPIF);
PIR1bits.SSPIF = 0;
LATAbits.LATA2 = 1;
}
//*****************************************************************
// Initialize MCP923S17 Port B
//*****************************************************************
void InitPortB_SPI(char b)
{
LATAbits.LATA2 = 0;
 
SSPBUF = 0x40;
while(!PIR1bits.SSPIF);
PIR1bits.SSPIF = 0;
SSPBUF = 0x01;
while(!PIR1bits.SSPIF);
PIR1bits.SSPIF = 0;
SSPBUF = b;
while(!PIR1bits.SSPIF);
PIR1bits.SSPIF = 0;
LATAbits.LATA2 = 1;
}
//*****************************************************************
// Initialize MCP923S17 SPI
//*****************************************************************
void InitSPI(void)
{
// RC3 is connected to SCK: Serial clock input
TRISCbits.TRISC3 = 0;
// RC5 is connected to SI: Serial data I/O (MCP23017), Serial data input (MCP23S17)
TRISCbits.TRISC5 = 0;
// SSPxCON1: MSSPx CONTROL REGISTER 1 (I2C? MODE)
// SSPEN=1, 1 = Enables the serial port and configures the SDAx and SCLx pins as the serial port pins
// SSPM<3:0>=0010, 0010 = SPI Master mode, clock = FOSC/64
// CKP=0, 0 = Holds clock low (clock stretch), used to ensure data setup time
SSP1CON1 = 0x22; // 00100010
// CKE: SPI Clock Select bit
// 1 = Transmit occurs on transition from active to Idle clock state
// 0 = Transmit occurs on transition from Idle to active clock state
SSP1STATbits.CKE = 1;
// SSP1IF: MSSP1 Interrupt Flag bit
//1 = The transmission/reception is complete (must be cleared in software)
//0 = Waiting to transmit/receive
PIR1bits.SSPIF = 0;
}
 
 
 
//******************************************************************
// LCD Initialization function
//******************************************************************
void LCDInit(void)
{
// RA2 connects CS: Chip Select (MCP23S17)
TRISAbits.TRISA2 = 0;
LATAbits.LATA2 = 1;	// SPI Sequential Write/Read
 
Delay10TCYx(0);
Delay10TCYx(0);
Delay10TCYx(0);
// RF6 is connected to RESET of expander: Hardware reset. Must be externally biased.
TRISFbits.TRISF6 = 0;
LATFbits.LATF6 = 0;
Delay10TCYx(0);
LATFbits.LATF6 = 1; // RESET expander
InitSPI(); // see above
InitPortA_SPI(0);
InitPortB_SPI(0);
WritePortA(0);
Delay10TCYx(0);
InitWrite( 0b00111100 );	//0011NFxx
Delay10TCYx(0);
InitWrite( 0b00001100 );	//Display Off
Delay10TCYx(0);
InitWrite( 0b00000001 );	//Display Clear
Delay10TCYx(0);
InitWrite( 0b00000110 );	//Entry mode
}

 
