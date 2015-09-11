/*
 * Using the ADC10 to convert signals from four different analog inputs: potentiometer,
 * LDR1, LDR2, and LDR3 into digital PWM output that will control the speed and direction
 * of a DC motor.
 *
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * I have modified some of the msp430g2553.h #define's. When you
 * compile the code you'll get some errors stating some things don't exist.
 * You're going to have to do a little bit of thinking to figure them out,
 * but just a little ;) If you need any help let me know.
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

#include <msp430g2553.h>
#include <msp430g2_add.h>

#define EN 		BIT1
#define _1A 	BIT0
#define _2A 	BIT2

#define POT		3
#define LDR1	2
#define LDR2	1
#define LDR3	0

void GPIO_INIT(void);
void BCS_INIT(void);
void TIMERA_INIT(void);
void ADC10_INIT(void);

void sample(void);

#pragma LOCATION(value, 0x0200)
int value[4] = {0};

char analog_input = 0;

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;

	GPIO_INIT();
	BCS_INIT();
	TIMERA_INIT();
	ADC10_INIT();

	_enable_interrupts();

	while (1)
	{
		if (TA1CTL & MC_1)
			sample();
		else
		{
			P2SEL &= ~BIT1;				// ensuring EN is always low when PWM is off
			LPM1;
		}
	}

}

void sample(void)
{
	ADC10CTL0 &= ~ENC;
	while (ADC10CTL1 & ADC10BUSY)
		;
	ADC10SA = 0x0200;
	ADC10CTL0 |= ADC10SC + ENC;
	LPM1;
}

void GPIO_INIT(void)
{
	/*
	 * Port 1
	 * P1.0 : input A0 - potentiometer
	 * P1.1 : input A1 - LDR1
	 * P1.2 : input A2 - LDR2
	 * P1.3 : input A3 - LDR3
	 * P1.4 : input button - potentiometer
	 * P1.5 : input button - LDR1
	 * P1.6 : input button - LDR2
	 * P1.7 : input button - LDR3
	 */
	P1OUT = 0xF0;
	P1SEL = 0x00;
	P1DIR = 0x00;
	P1REN = 0xF0;
	P1IES = 0xF0;
	P1IFG = 0x00;
	P1IE  = 0xF0;

	/*
	 * Port 2
	 * P2.0 : output motor - 1A
	 * P2.1 : PWM output motor - EN			*since weird CCTL0 pwm thingy
	 * P2.2 : output motor - 2A
	 * P2.3 :
	 * P2.4 : output LED - potentiometer
	 * P2.5 : output LED - LDR1
	 * P2.6 : output LED - LDR2
	 * P2.7 : output LED - LDR3
	 */
	P2OUT = 0x04;
	P2SEL = 0x02;
	P2DIR = 0xFF;
	P2REN = 0x00;
	P2IES = 0x00;
	P2IFG = 0x00;
	P2IE  = 0x00;
}

void BCS_INIT(void)
{
	if (CALBC1_8MHZ == 0xFF)					// if no calibration then trap
		while (1)
			;

	DCOCTL = 0x00;								// calibrate DCO to 8 MHz
	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;

	BCSCTL1 |= XT2OFF + DIVA_0;
	BCSCTL2 = SELM_1 + DIVM_0 + SELS_0 + DIVS_0;
	BCSCTL3 = XT2S_0 + LFXT1S_2;
}

void TIMERA_INIT(void)
{
	TA1CTL = TASSEL_2 + ID_0 + MC_0 + TACLR;
	TA1CCR0 = 1024 - 1;
	TA1CCTL1 = CM_0 + COM + OUTMOD_7 + CCIE;
	TA1CCR1 = 512;
}

void ADC10_INIT(void)
{
	ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10SR200 + REFOFF + MSC + ADC10OFF + ADC10IE;
	ADC10CTL1 = INCH_3 + SHS_0 + ADC10DF_B + ADC10DIV_1 + ADC10SSEL_0 + CONSEQ_3;
	ADC10AE0 = BIT3 + BIT2 + BIT1 + BIT0;
	ADC10DTC0 = ADC10OB + ADC10DT;
	ADC10DTC1 = 4;
}


#pragma vector = TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void)
{
	switch (TA1IV)
	{
	case TA1IV_NONE:	break;
	case TA1IV_TACCR1:	TA1CCR1 = value[analog_input];
						break;
	case TA1IV_TACCR2:	break;
	default:			_never_executed();
	}
}

#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
	switch (P1IFG)
	{
		case BIT4:	P2OUT ^= BIT4;
					if (P2OUT & BIT4)
					{
						analog_input = POT;
						ADC10CTL0 |= ADC10ON;
						P2SEL |= BIT1;
						TA1CTL |= MC_1;
					}
					else
						TA1CTL &= ~MC_1;
					P1IFG &= ~BIT4;
					break;
		case BIT5:	P2OUT ^= BIT5;
					if (P2OUT & BIT5)
					{
						analog_input = LDR1;
						ADC10CTL0 |= ADC10ON;
						P2SEL |= BIT1;
						TA1CTL |= MC_1;
					}
					else
						TA1CTL &= ~MC_1;
					P1IFG &= ~BIT5;
					break;
		case BIT6: 	P2OUT ^= BIT6;
					if (P2OUT & BIT6)
					{
						analog_input = LDR2;
						ADC10CTL0 |= ADC10ON;
						P2SEL |= BIT1;
						TA1CTL |= MC_1;
					}
					else
						TA1CTL &= ~MC_1;
					P1IFG &= ~BIT6;
					break;
		case BIT7: 	P2OUT ^= BIT7;
					if (P2OUT & BIT7)
					{
						analog_input = LDR3;
						ADC10CTL0 |= ADC10ON;
						P2SEL |= BIT1;
						TA1CTL |= MC_1;
					}
					else
						TA1CTL &= ~MC_1;
					P1IFG &= ~BIT7;
					break;
		default:	_never_executed();
	}
	LPM1_EXIT;
}


#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
	if ((TA1CTL & MC_1) == 0)
	{
		ADC10CTL0 &= ~ENC;
		ADC10CTL0 &= ~ADC10ON;
	}
	LPM1_EXIT;
}
