/*
 * Using the Timer0 A in up mode to control a PWM signal output in order to pulse an LED and
 * display pertinent information on an external LCD.
 *
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * Proper Operation is only defined for frequencies 32Hz - 933Hz
 * Operation outside this range is not defined
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 */

#include <msp430g2553.h>

#define RS 	BIT4
#define EN 	BIT5
#define CLEAR (BIT0 + BIT1 + BIT2 + BIT3 + BIT4)
#define STATIC 0
#define DYNAMIC 1

#define INSTR 	0
#define DATA 	16

#define NULL 	('\0')

void GPIO_init(void);
void BCS_init(void);
void TIMER1_Ax_init(void);
void LCD_init(void);

void send(char type, char byte);
void enable(void);
void print(char *character);
void refresh(void);

int Period = 128;
int Half_Period = 64;
int Step = 8;
int Small_Step = 1;
char Update_Display = 0;
char State = STATIC;
char X = 2;
char D = 8;

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;

	GPIO_init();
	BCS_init();
	TIMER1_Ax_init();
	LCD_init();

	__bis_SR_register(GIE);
	refresh();

	while (1)
	{
		if (Update_Display == 1)
		{
			refresh();
		}

		LPM3;
	}
}

void GPIO_init(void)
{
	/*
	 * Port 1 Configuration
	 * P1.0 - P1.3 = D4 - D7
	 * P1.4 = RS
	 * P1.5 = E
	 */
	P1OUT = 0x00;
	P1SEL = 0x00;
	P1DIR = 0xFF;
	P1REN = 0x00;
	P1IES = 0x00;
	P1IFG = 0x00;
	P1IE  = 0x00;

	/*
	 * Port 2 Configuration
	 * P2.0 = pulse toggle
	 * P2.1 = TA1CCR1++
	 * P2.2 = TA1CCR1--
	 * P2.3 = Period++
	 * P2.4 = Period--
	 * P2.5 = PWMed LED
	 * P2.6 = XTAL in
	 * P2.7 = XTAL out
	 */
	P2OUT = 0x1F;
	P2SEL = 0xE0;
	P2DIR = 0xA0;
	P2REN = 0x1F;
	P2IES = 0x1F;
	P2IFG = 0x00;
	P2IE  = 0x1F;
}


void BCS_init(void)
{
	do
	{	// crystal warms up
		IFG1 &= ~OFIFG;
		__delay_cycles(50);
	} while (IFG1 & OFIFG);
	IE1 |= OFIE;

	BCSCTL1 |= XT2OFF + DIVA_0;
	BCSCTL2 = SELM_1 + DIVM_3 + SELS_1 + DIVS_0;
	BCSCTL3 = XT2S_0 + LFXT1S_0 + XCAP_3;
}

void TIMER1_Ax_init(void)
{
	//	ACLK/1 = 32768 (1s), up mode
	TA1CTL = TASSEL_1 + ID_0 + MC_1 + TACLR;
	TA1CCR0 = Period;

	// reset @ TA1CCR1, set @ TA1CCR0
	TA1CCTL2 = CM_0 + COM + OUTMOD_7;
	TA1CCR2 = Half_Period;
}

void LCD_init(void)
{
	__delay_cycles(200000);			// let display warm up

	P1OUT |= 0x02;					// 4-bit mode >> initial Function Set
	enable();
	P1OUT &= ~CLEAR;

	send(INSTR, 0x28);				// Function Set
	send(INSTR, 0x0C);				// Display on/off control
	send(INSTR, 0x06);				// Entry Mode Set
	send(INSTR, 0x01);				// Clear Display
	send(INSTR, 0x02);				// Return Home (needs ~1.5ms)
	__delay_cycles(2000);
}

void send(char type, char byte)
{
	P1OUT |= (RS & type);

	P1OUT |= (byte >> 4);
	enable();

	P1OUT = (P1OUT & ~0x0F) | (byte & 0x0F);
	enable();

	P1OUT &= ~CLEAR;
}

void enable(void)
{
	P1OUT |= EN;
	P1OUT &= ~EN;
}

void print(char *character)
{
	while (*character != NULL)
		send(DATA, *(character++));
}

void refresh(void)
{
	static char *frequency[] = {"933", "504", "254", "127", " 64", " 32"};
	static char *period[] = {".09", ".25", "  1", "  2", "  4", "  8"};
	static char *duty[] = {" 0", " 6", "12", "18", "25", "31", "37", "43", "49",
								"55", "62", "68", "74", "80", "87", "93", "99"};

	if (State == STATIC)
	{
		send(INSTR, 0x80);					// set DDRAM address to line 1
		print("F=");
		print(frequency[X]);
		print("Hz        ");

		send(INSTR, 0xC0);					// set DDRAM address to line 2
		print("D=");
		print(duty[D]);
		print("%     Static");
	}
	else // State == DYNAMIC
	{
		send(INSTR, 0x80);					// set DDRAM address to line 1
		print("F=");
		print(frequency[X]);
		print("Hz  P=");
		print(period[X]);
		print("s");

		send(INSTR, 0xC0);					// set DDRAM address to line 2
		print("D=Flux   Dynamic");
	}
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void)
{
	static char direction = 0; 							// 0 = down, 1 = up

	switch (TA1IV)
	{
		case 0x00: 	break;
		case 0x02:	break;
		case 0x04: 	if (direction)						// direction = up
					{
						TA1CCR2 += Small_Step;
						if (TA1CCR2 >= Period)
							direction = 0;				// go down
//							P1OUT ^= BIT6;				// get overall frequency
//						}
					}
					else // direction = down
					{
						TA1CCR2 -= Small_Step;
						if (TA1CCR2 == 0)
							direction = 1;				// go up
//							P1OUT ^= BIT6;				// get overall frequency
//						}
					}
		case 0x0A:	break;
		default:	__never_executed();
	}
}

#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{
	switch (P2IFG)
	{
		case BIT0:	TA1CCR2 = Half_Period;				// Toggle LED pulse
					TA1CCTL2 ^= CCIE;
					State ^= 1;
					P2IFG &= ~BIT0;
					break;

		case BIT1:	if (TA1CCR2 == Period)				// TA1CCR2++
					{
						TA1CCR2 = 0;
						D = 0;
					}
					else
					{
						TA1CCR2 += Step;
						D++;
					}
					P2IFG &= ~BIT1;
					break;

		case BIT2:	if (TA1CCR2 == 0)					// TA1CCR2--
					{
						TA1CCR2 = Period;
						D = 16;
					}
					else
					{
						TA1CCR2 -= Step;
						D--;
					}
					P2IFG &= ~BIT2;
					break;

		case BIT3: 	Period += Period;					// Period++
					Half_Period += Half_Period;
					Step += Step;
					if (Step > 8)
						Small_Step += Small_Step;

					TA1CCR0 = Period;
					TA1CCR2 = Half_Period;
					D = 8;
					X++;
					P2IFG &= ~BIT3;
					break;

		case BIT4: 	if (Step > 2)						// Period--
					{
						Period /= 2;
						Half_Period  /= 2;
						Step /= 2;
						if (Small_Step != 1)
							Small_Step /= 2;

						TA1CCR0 = Period;
						TA1CCR2 = Half_Period;
						D = 8;
					}
					X--;
					P2IFG &= ~BIT4;
					break;

		default:	__never_executed();
	}

	Update_Display = 1;
	LPM3_EXIT;
}
