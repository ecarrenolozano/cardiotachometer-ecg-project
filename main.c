/*
************************************
* Project Name : Cardiotacometro   *
* Karen Forero                     *
* Andres Arguello                   *
* Edwin Carreño                     *
*                                  *
************************************

LED ROJO = 0x08
LED VERDE = 0x10
LED AZUL  = 0x20

Diagnostico

-Taquicardia > 100 ppm -----> LED ROJO
-Bradicardia <60  -----> LED AZUL
61 < Normal < 99 ppm  ----> LED VERDE


*/
#include <msp430g2553.h>
#include "lcd16.h"


#ifndef TIMER0_A1_VECTOR
#define TIMER0_A1_VECTOR TIMERA1_VECTOR
#define TIMER0_A0_VECTOR TIMERA0_VECTOR
#endif

//+++++++++++++++++++++++++++++  FUNCIONES +++++++++++++++++++++++++++++++++++

void config_puertos(void);
void config_p2_interrupt(void);
void config_timer(void);
void config_clocks(void);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


volatile unsigned int contador,frecuencia;

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    config_clocks();
    config_puertos();
    //config_p2_interrupt();
    lcdinit();
    //P2OUT = ~0x08;
    _BIS_SR(GIE);
    while(1)
        {
    		contador =0;
            gotoXy(0,0);
            prints("Heart Rate");
            gotoXy(0,1);
            prints("Push Start ");
            int a = 1,i;
            while(a = 1)
            {
            	P2OUT &= ~0x02;
				if((P1IN & 0x10) == 0x10)
				{
					config_p2_interrupt();
					gotoXy(0,0);

					for(i=59;i>=0;i--)
					{

						gotoXy(0,1);
						prints("Tiempo : ");
						gotoXy(9,1);
						integerToLcd(i              );
						P2OUT |= 0x02;
						_delay_cycles(1000000);
						gotoXy(0,0);
						prints("Pulsos :");
						integerToLcd(contador);
						P2OUT &= ~0x02;


					}
					P2OUT &= ~0x02;
					frecuencia = contador;
					gotoXy(0,0);
					prints("Heart Rate ");
					gotoXy(11,0);
					integerToLcd(frecuencia);

					if (frecuencia<60)
					{
						gotoXy(0,1);
						prints("Dx: Bradicardia");
						P2OUT = (P2OUT & ~0x20) | 0x18;
					}
					else if (frecuencia>100)
					{
						gotoXy(0,1);
						prints("Dx: Taquicardia");
						P2OUT = (P2OUT & ~0x08) | 0x30;

					}
					else
					{
						gotoXy(0,1);
						prints("Dx:      Normal");
						P2OUT = (P2OUT & ~0x10) | 0x28;
					}
					a=0;
				}

            }
            contador =0;


        }
} // END MAIN FUNCTION

void config_puertos(void)
{
    P1DIR = 0xEF;
    P1OUT = 0x00;
    P1REN = 0x10;
    P2DIR = 0xFE;
    P2OUT = 0xFC;
}

void config_clocks(void)
{
    if(CALBC1_1MHZ == 0xFF || CALDCO_1MHZ == 0xFF)
    {
        while(1);
    }

    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    BCSCTL3 |= LFXT1S_2;
    BCSCTL1 |= DIVA_0;
    IFG1 &= ~OFIFG;
    BCSCTL2 |= SELM_0 + DIVM_0 + DIVS_0;
}
void config_p2_interrupt(void)
{
	P2OUT &= ~0x02;
    P2IE = 0x01;
    P2IES =0;
    P2IFG = 0;
    _delay_cycles(125000);

}


//++++++++++++++++++++++++++++++++++++++++++++  Interrupciones
#pragma vector = PORT2_VECTOR
__interrupt void pulsador(void)
{
    contador++;
    _delay_cycles(100);
    P2IFG = 0;
}