/*
 * Making a motor controller program. The motor is controlled by push-buttons
 * ~2.4s (full step) 360 degree rotation time
 * ~1.9s (half step) 360 degree rotation time
 * -------Give motor time to warm up before taking it redline---------
 */

#include <msp430.h>

#define ON 1
#define OFF 0
#define CW 1	// clockwise
#define CCW 0	// counterclockwise
#define FULL 1
#define HALF 0

void port1_init(void);
void port2_init(void);
void spin(void);
void timer(int n);

int Power = OFF;
int Direction = CW;
int Delay = 25;
int Rotation = FULL;

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;

	port1_init();
	port2_init();

	_enable_interrupts();

	while (1)
	{
		if (Power == ON)
			spin();
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
	P2OUT = 0x7F;				// port 2 initialization for button input signals
	P2SEL = 0x00;
	P2DIR = 0x00;
	P2REN = 0x7F;

	P2IES = 0x7F;
	P2IFG = 0x00;
	P2IE = 0x7F;
}
void timer(int n)	// n = 10 is 1ms
{
	while (n-- != 0)
		_delay_cycles(100);
}

void spin(void)
{
	const int full_cw[4] = {0x01, 0x02, 0x04, 0x08};
	const int full_ccw[4] = {0x08, 0x04, 0x02, 0x01};
	const int half_cw[8] = {0x09, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08};
	const int half_ccw[8] = {0x08, 0x0C, 0x04, 0x06, 0x02, 0x03, 0x01, 0x09};
	unsigned int step = 0;

	while (Power == ON)
	{
		if (Rotation == FULL)
		{
			for (step = 0; step < 4; step++)
			{
				if (Direction == CW)
					P1OUT = full_cw[step];
				else // Direction == CCW
					P1OUT = full_ccw[step];

				timer(Delay);
			}

			if (Delay <= 11)	// turn on LED if fastest speed
				P1OUT |= BIT4;
			else
				P1OUT &= ~BIT4;
		}
		else // Rotation == HALF
		{
			for (step = 0; step < 8; step++)
			{
				if (Direction == CW)
					P1OUT = half_cw[step];
				else // Direction == CCW
					P1OUT = half_ccw[step];

				timer(Delay);
			}

			if (Delay <= 4)  	// turn on LED if fastest speed
				P1OUT |= BIT4;
			else
				P1OUT &= ~BIT4;
		}
	}
	P1OUT = 0x00;
}

#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{
	if (P2IFG == BIT0)
	{
		Power ^= 1; 		// toggle the power on and off

		P2IFG &= ~BIT0;
		return;
	}

	if (P2IFG == BIT1)
	{
		Rotation ^= 1;		// toggle rotation: full or half

		P2IFG &= ~BIT1;
		return;
	}

	if (P2IFG == BIT2)
	{
		Delay -= 1;

		P2IFG &= ~BIT2;
		return;
	}

	if (P2IFG == BIT3)
	{
		Delay += 1;

		P2IFG &= ~BIT3;
		return;
	}

	if (P2IFG == BIT4)
	{
		Direction = CW;

		P2IFG &= ~BIT4;
		return;
	}

	if (P2IFG == BIT5)
	{
		Direction = CCW;

		P2IFG &= ~BIT5;
		return;
	}

	if (P2IFG == BIT6)
	{
		Direction = CW;		// reset to initial conditions
		Delay = 25;
		Rotation = FULL;

		P2IFG &= ~BIT6;
		return;
	}
}
