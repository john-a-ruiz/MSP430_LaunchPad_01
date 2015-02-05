/*
 * Making a motor controller program. The motor is controller by push-buttons
 */

#include <msp430.h>

#define ON 1
#define OFF 0
#define CW 1	// clockwise
#define CCW 0	// counterclockwise

void port1_init(void);
void port2_init(void);
void spin(int Direction);

int Power = OFF;
int Direction = CW;

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;

	port1_init();
	port2_init();

	_enable_interrupts();

	while (1)
	{
		if (Power == ON)
			spin(Direction);
	}
}

void port1_init(void)
{
	P1OUT = 0x00;				// port 1 initializaion for motor control signals
	P1SEL = 0x00;
	P1DIR = 0xFF;
	P1REN = 0x00;

	P1IES = 0x00;
	P1IFG = 0x00;
	P1IE = 0x00;
}
void port2_init(void)
{
	P2OUT = 0x3F;				// port 2 initialization for button input signals
	P2SEL = 0x00;
	P2DIR = 0x00;
	P2REN = 0x3F;

	P2IES = 0x3F;
	P2IFG = 0x00;
	P2IE = 0x3F;
}

void spin(int Direction)
{

}

#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{
	if (P2IFG == BIT0)
		Power ^= 1; 		// toggle the power on and off
}
