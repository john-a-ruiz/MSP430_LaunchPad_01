/*
 * Trying to do something with blinking LEDs
 */

#include <msp430.h> 

void delay(int n);
void button0(void);
void button1(void);
void button2(void);
void button3(void);
void button4(void);
void button5(void);
void button6(void);
void button7(void);
void control(void);


int Flag = 0;
int Speed = 63;					// initial motor delay time

void main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    P1OUT = 0x00;				// Port 1 Initialization (LEDs)
    P1SEL = 0x00;
    P1DIR = 0xFF;
    P1REN = 0x00;

    P1IES = 0x00;
    P1IFG = 0x00;
    P1IE = 0x00;

    P2OUT = 0xFF;				// Port 2 Initialization (button control)
    P2SEL = 0x00;
    P2DIR = 0x00;
    P2REN = 0xFF;

    P2IES = 0xFF;
    P2IFG = 0x00;
    P2IE = 0xFF;

    _enable_interrupt();

    while (1)
    {
    	if (Flag == 1)
    	{
    		control();
    	}
    }


}

#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{
	if (P2IFG == BIT0)
		button0();

	if (P2IFG == BIT1)
		button1();

	if (P2IFG == BIT2)
		button2();

	if (P2IFG == BIT3)
		button3();

	if (P2IFG == BIT4)
		button4();

	if (P2IFG == BIT5)
		button5();

	if (P2IFG == BIT6)
		button6();

	if (P2IFG == BIT7)
		button7();
}

void delay(int n)
{
	int i;

	for (i = 0; i < n; i++)
	{
		__delay_cycles(1000);
	}
}

void button0(void)
{
	int bit[8] = {BIT0, BIT1, BIT2, BIT3, BIT4, BIT5, BIT6, BIT7};
	int i, j;

	P1OUT = 0x00;

	for (j = 0; j < 10; j++)
	{
		P1OUT |= BIT0;
		delay(32);
		for (i = 0; i < 7; i++)
		{
			P1OUT |= bit[i+1];
			P1OUT &= ~bit[i];
			delay(32);
		}
		P1OUT &= ~BIT7;
	}


	P2IFG &= ~BIT0;
}

void button1(void)
{
	int bit[8] = {BIT0, BIT1, BIT2, BIT3, BIT4, BIT5, BIT6, BIT7};
	int i, j;

	P1OUT = 0x00;

	P1OUT |= BIT0;
	delay(63);
	for (j = 0; j < 5; j++)
	{
		for (i = 0; i < 7; i++)
		{
			P1OUT |= bit[i+1];
			P1OUT &= ~bit[i];
			delay(63);
		}

		for (i = 7; i > 0; i--)
		{
			P1OUT |= bit[i-1];
			P1OUT &= ~bit[i];
			delay(63);
		}
	}
	P1OUT &= ~BIT0;

	P2IFG &= ~BIT1;
}

void button2(void)
{
	int i, j;
	int bit[8] = {BIT0, BIT1, BIT2, BIT3, BIT4, BIT5, BIT6, BIT7};

	P1OUT = 0x00;

	for (j = 0; j < 2; j++)
	{
		for (i = 0; i < 8; i++)
		{
			P1OUT |= bit[i];
			delay(63);
		}

		for (i = 7; i >= 0; i--)
		{
			P1OUT &= ~bit[i];
			delay(63);
		}

		for (i = 7; i >= 0; i--)
		{
			P1OUT |= bit[i];
			delay(63);
		}

		for (i = 0; i < 8; i++)
		{
			P1OUT &= ~bit[i];
			delay(63);
		}
	}

	P2IFG &= ~BIT2;
}

void button3(void)
{
	int i, j;
	int duty_on, duty_off;

	for (j = 0; j < 5; j++)
	{
		for (i = 0, duty_on = 19, duty_off = 2; i < 19; i++, duty_on--, duty_off++)
		{
			int k = 2;
			while (k--)
			{
				P1OUT = 0xFF;
				delay(duty_on);
				P1OUT = 0x00;
				delay(duty_off);
			}
		}

	}

	P2IFG &= ~BIT3;
}

void button4(void)
{
	Flag ^= 1;

	P2IFG &= ~BIT4;
}

void button5(void)
{
	if (Speed > 2)
		Speed -= 2;
	else
		Speed = 0;

	P2IFG &= ~BIT5;
}

void button6(void)
{
	if (Speed < 80)
		Speed += 2;
	else
		Speed += 50;

	P2IFG &= ~BIT6;
}

void button7(void)
{
	Speed = 63;

	P2IFG &= ~BIT7;
}

void control(void)
{
	int bit[8] = {BIT0, BIT1, BIT2, BIT3, BIT4, BIT5, BIT6, BIT7};
	int i;

	P1OUT |= BIT0;
	delay(Speed);
	while (Flag == 1)
	{
		for (i = 0; i < 7; i++)
		{
			P1OUT |= bit[i+1];
			P1OUT &= ~bit[i];
			delay(Speed);
		}

		for (i = 7; i > 0; i--)
		{
			P1OUT |= bit[i-1];
			P1OUT &= ~bit[i];
			delay(Speed);
		}
	}
	P1OUT &= ~BIT0;
}
