/*
Matt Mammarelli
9/18/17
ECE 09342-2
*/

//MSP430G2553 Sensors and Signal Conditioning
//Type A in serial to print current value of sensor

#include  "msp430g2253.h"

unsigned int ADC_value=0;

void ConfigureAdc(void);

void main(void)
{
    {
        WDTCTL = WDTPW + WDTHOLD;       // Stop WDT

        if (CALBC1_1MHZ==0xFF)                    // If calibration constant erased
               {
                 while(1);                               // do not load, trap CPU!!
               }
               DCOCTL = 0;                               // Select lowest DCOx and MODx settings
               BCSCTL1 = CALBC1_1MHZ;          // Set range   DCOCTL = CALDCO_1MHZ;
               DCOCTL = CALDCO_1MHZ;
               P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
               P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD
               UCA0CTL1 |= UCSSEL_2;                     // SMCLK
               UCA0BR0 = 104;                            // 1MHz 9600
               UCA0BR1 = 0;                              // 1MHz 9600
               UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
               UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
               IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt



        BCSCTL2 &= ~(DIVS_3);           // SMCLK = DCO = 1MHz
        P1SEL |= BIT3;                  // ADC input pin P1.3
        ConfigureAdc();                 // ADC set-up function call
        __enable_interrupt();           // Enable interrupts.






        while(1)
        {
            __delay_cycles(1000);               // Wait for ADC Ref to settle
            ADC10CTL0 |= ENC + ADC10SC;         // Sampling and conversion start
            __bis_SR_register(CPUOFF + GIE);    // Low Power Mode 0 with interrupts enabled
            ADC_value = ADC10MEM;               // Assigns the value held in ADC10MEM to the integer called ADC_value

        }

    }
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    __bic_SR_register_on_exit(CPUOFF);        // Return to active mode }
}

// Function containing ADC set-up
void ConfigureAdc(void)
{

    ADC10CTL1 = INCH_3 + ADC10DIV_3 ;         // Channel 3, ADC10CLK/3
    ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;  // Vcc & Vss as reference, Sample and hold for 64 Clock cycles, ADC on, ADC interrupt enable
    ADC10AE0 |= BIT3;                         // ADC input enable P1.3
}


//UART
//  Echo back RXed character, confirm TX buffer is ready first
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?

  //if input is A print out the ADC
  if(UCA0RXBUF == 65){
      UCA0TXBUF = ADC10MEM;                    // TX -> RXed character
  }
  //else print out 66 
  else{
      UCA0TXBUF = 66;
  }

}
