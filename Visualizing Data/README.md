# Visualizing Data
Information from three sensors was captured using the ADC then displayed using the RGB LED, the LCD on the MSP430FR6989, plots in MATLAB, and a simultaneous RGB and MATLAB test.
## RGB LED MSP430F5529, MSP430FR2311, MSP430FR5994
The built in ADC in the MSP430 was utilized to convert an analog value from a photoresistor, photodiode, and phototransistor into a digital value.
Then PWM was utilized to control the brightness of the red RGB led based on the received sensor value.


## LCD Display MSP430FR6989
This board would use a built in ADC to convert an analog sensor value to a digital value.
Then this value would be converted into a series of characters and displayed on the on board LCD Screen utilizing the LCDDriver.c and LCDDriver.h files.


## MATLAB MSP430F5529, MSP430G2553
Utilizing the built in ADC for each board, analog values from sensors were converted to digital values.
Then the digital values were transmitted over UART whenever the character 'A' was received by the MSP430.
MATLAB would take three of these values over a period of three seconds and create a plot of them.

### MSP430F5529 Code

```c
/*
Matt Mammarelli
9/18/17
ECE 09342-2
*/

//MSP430F5529 Visualizing Data Matlab
//Will transmit the current sensor value when character A is sent to processor over UART

#include <msp430.h>

unsigned int ADC_value=0;

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  ADC12CTL0 = ADC12SHT02 + ADC12ON;         // Sampling time, ADC12 on
  ADC12CTL1 = ADC12SHP;                     // Use sampling timer
  ADC12IE = 0x01;                           // Enable interrupt
  ADC12CTL0 |= ADC12ENC;
  P6SEL |= 0x01;                            // P6.0 ADC option select
  P1DIR |= 0x01;                            // P1.0 output


  //uart **************************************************************************************************
  // P3.3, P3.4 transmit/receive
  P3SEL = BIT3+BIT4;
  // Put state machine in reset
  UCA0CTL1 |= UCSWRST;
  // SMCLK
  UCA0CTL1 |= UCSSEL_2;
  // 1MHz 9600 baud
  UCA0BR0 = 6;
  // 1MHz 9600
  UCA0BR1 = 0;
  //sets m control register
  UCA0MCTL = UCBRS_0 + UCBRF_13 + UCOS16;
  //sets control register
  UCA0CTL1 &= ~UCSWRST;
  //enable interrupt
  UCA0IE |= UCRXIE;
  //*******************************************************************************************************




  while (1)
  {
    ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
    __bis_SR_register(LPM0_bits + GIE);     // LPM0, ADC12_ISR will force exit
    __no_operation();                       // For debugger
  }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(ADC12IV,34))
  {
  case  0: break;                           // Vector  0:  No interrupt
  case  2: break;                           // Vector  2:  ADC overflow
  case  4: break;                           // Vector  4:  ADC timing overflow
  case  6:                                  // Vector  6:  ADC12IFG0

        ADC_value = ADC12MEM0;

    __bic_SR_register_on_exit(LPM0_bits);   // Exit active CPU
  case  8: break;                           // Vector  8:  ADC12IFG1
  case 10: break;                           // Vector 10:  ADC12IFG2
  case 12: break;                           // Vector 12:  ADC12IFG3
  case 14: break;                           // Vector 14:  ADC12IFG4
  case 16: break;                           // Vector 16:  ADC12IFG5
  case 18: break;                           // Vector 18:  ADC12IFG6
  case 20: break;                           // Vector 20:  ADC12IFG7
  case 22: break;                           // Vector 22:  ADC12IFG8
  case 24: break;                           // Vector 24:  ADC12IFG9
  case 26: break;                           // Vector 26:  ADC12IFG10
  case 28: break;                           // Vector 28:  ADC12IFG11
  case 30: break;                           // Vector 30:  ADC12IFG12
  case 32: break;                           // Vector 32:  ADC12IFG13
  case 34: break;                           // Vector 34:  ADC12IFG14
  default: break;
  }
}




//uart interrupt vector
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
  switch(__even_in_range(UCA0IV,4))
  {
  case 0:break;   // Vector 0 - no interrupt
  case 2:{
      while (!(UCA0IFG&UCTXIFG));  // USCI_A0 TX buffer check

      //if input is A print out the ADC
        if(UCA0RXBUF == 65){
            UCA0TXBUF = ADC_value; //skips a clockcycle of setting adc_value
        }
        //else print out 66
        else{
            UCA0TXBUF = 66;
        }

          break;

  }

  case 4:break;    // Vector 4 - TXIFG
  default: break;
  }
}

```

### MSP430G2553 Code

```c
/*
Matt Mammarelli
9/18/17
ECE 09342-2
*/

//MSP430G2553 Sensors and Signal Conditioning
//Type A in serial to print current value of sensor

#include  "msp430g2253.h"

unsigned int ADC_value=0;

void initAdc(void);

void main(void)
{
    {
        // Stop WDT
        WDTCTL = WDTPW + WDTHOLD;

        // If calibration constant erased
        if (CALBC1_1MHZ==0xFF)
        {
            // do not load
                 while(1);
         }
        // Select lowest DCOx and MODx settings
        DCOCTL = 0;
        // Set range   DCOCTL = CALDCO_1MHZ;
        BCSCTL1 = CALBC1_1MHZ;
        DCOCTL = CALDCO_1MHZ;
        // P1.1 = RXD, P1.2=TXD
        P1SEL = BIT1 + BIT2 ;
        // P1.1 = RXD, P1.2=TXD
        P1SEL2 = BIT1 + BIT2 ;
        // SMCLK
        UCA0CTL1 |= UCSSEL_2;
        // 1MHz 9600
        UCA0BR0 = 104;
        // 1MHz 9600
        UCA0BR1 = 0;
        // Modulation UCBRSx = 1
        UCA0MCTL = UCBRS0;
        // **Initialize USCI state machine**
        UCA0CTL1 &= ~UCSWRST;
        // Enable USCI_A0 RX interrupt
        IE2 |= UCA0RXIE;


        // SMCLK = DCO = 1MHz
        BCSCTL2 &= ~(DIVS_3);
        // ADC input pin P1.3
        P1SEL |= BIT3;
        // ADC set-up function call
        initAdc();
        // Enable interrupts.
        __enable_interrupt();

        while(1)
        {
            // Wait for ADC Ref to settle
            __delay_cycles(1000);
            // Sampling and conversion start
            ADC10CTL0 |= ENC + ADC10SC;
            // Low Power Mode 0 with interrupts enabled
            __bis_SR_register(CPUOFF + GIE);
            //set adc value
            ADC_value = ADC10MEM;

        }

    }
}

//ADC10 interrupt
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    // Return to active mode
    __bic_SR_register_on_exit(CPUOFF);
}

//ADC set-up
void initAdc(void)
{

    // Channel 3, ADC10CLK/3
    ADC10CTL1 = INCH_3 + ADC10DIV_3 ;
    //Vcc and Vss reference, Sample and hold for 64 Clock cycles, ADC on, ADC interrupt enable
    ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;
    // ADC input enable P1.3
    ADC10AE0 |= BIT3;
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
  // USCI_A0 TX buffer ready?
  while (!(IFG2&UCA0TXIFG));

  //if input is A print out the ADC
  if(UCA0RXBUF == 65){
      UCA0TXBUF = ADC10MEM; //skips a clockcycle of setting adc_value
  }
  //else print out 66 
  else{
      UCA0TXBUF = 66;
  }

}


```

### MSP430F5529, MSP430G2553 MATLAB Code

```c
%Receive Serial Sensor 
%Gets 3 sensor reads over 3 seconds
%increased max y limit to 260
%Matt Mammarelli
%9/27/17


delete(instrfindall); %prevents cant use com port error

clear all;
clf
Ts = 1;   % Sampling time
t = 1:Ts:6; % support of signal

p=serial('COM8','BaudRate',9600,'DataBits',8); %have to change com to current one
set(p,'Parity','none');
set(p,'Terminator','LF');
fopen(p); %opens com port
fprintf(p,'A') %sends character A to UART to receive sensor info


pause(1); %delay one second
fprintf(p,'A')


pause(1); %delay one second
fprintf(p,'A')


veri=fscanf(p,'%u'); %gets UART sensor information

figure %creates figure
xlabel('Count')
ylabel('Sense Change')
title('Photoresistor Count vs. Sense')
grid on;
hold on;


for t=1:Ts:6
    if (mod(t,2)==1) %only use odd matrix indices, evens are return characters
    plot(t,uint8(veri(t)),'o'); %plots circles 
    end
    xlim ([0 6]) %x limit
    ylim ([0 260]) %y limit
    drawnow    
end

    

fclose(p); %closes serial connection
delete(p); %deletes serial variable


```
### MATLAB Plots
### MSP430F5529
![alt text](MATLAB/MSP430F5529/photoresister_matlab5529.png "photoresister_matlab5529 MATLAB")
### MSP430G2553
![alt text](MATLAB/MSP430G2553/matlab_photoresistor.png "photoresister_matlab MATLAB")

## RGB and MATLAB MSP430F5529
For this part, the MSP430F5529 would use the ADC for the photoresistor to convert the analog signal to a digital one. 
Based on this value, PWM was used to change the brightness of the red rgb.
It would simultaneously transmit the current value of the photoresistor over UART to display on a MATLAB plot over a period of three seconds.
 
### MSP430F5529 RGB and UART Code

```c
/*
Matt Mammarelli
9/18/17
ECE 09342-2
*/

//MSP430F5529 Visualizing Data Matlab and RGB
//red rgb changes with photoresistor
//matlab plots the data
//Will transmit the current sensor value when character A is sent to processor over UART

#include <msp430.h>

unsigned int ADC_value=0;

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  ADC12CTL0 = ADC12SHT02 + ADC12ON;         // Sampling time, ADC12 on
  ADC12CTL1 = ADC12SHP;                     // Use sampling timer
  ADC12IE = 0x01;                           // Enable interrupt
  ADC12CTL0 |= ADC12ENC;
  P6SEL |= 0x01;                            // P6.0 ADC option select
  P1DIR |= 0x01;                            // P1.0 output


  //uart **************************************************************************************************
  // P3.3, P3.4 transmit/receive
  P3SEL = BIT3+BIT4;
  // Put state machine in reset
  UCA0CTL1 |= UCSWRST;
  // SMCLK
  UCA0CTL1 |= UCSSEL_2;
  // 1MHz 9600 baud
  UCA0BR0 = 6;
  // 1MHz 9600
  UCA0BR1 = 0;
  //sets m control register
  UCA0MCTL = UCBRS_0 + UCBRF_13 + UCOS16;
  //sets control register
  UCA0CTL1 &= ~UCSWRST;
  //enable interrupt
  UCA0IE |= UCRXIE;
  //*******************************************************************************************************


  //rgb pwm *****************************************************************************************

     // P1.2 output
     P1DIR |= BIT2;

     // P1.2  options select GPIO
     P1SEL |= BIT2;

     // PWM Period about 1khz
     TA0CCR0 = 1024;

     // CCR1 reset/set
     TA0CCTL1 = OUTMOD_7;


     // SMCLK, up mode, clear TAR
     TA0CTL = TASSEL_2 + MC_1 + TACLR;

     //***************************************************************************************************


  while (1)
  {
    ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
    __bis_SR_register(LPM0_bits + GIE);     // LPM0, ADC12_ISR will force exit
    __no_operation();                       // For debugger
  }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(ADC12IV,34))
  {
  case  0: break;                           // Vector  0:  No interrupt
  case  2: break;                           // Vector  2:  ADC overflow
  case  4: break;                           // Vector  4:  ADC timing overflow
  case  6:                                  // Vector  6:  ADC12IFG0

        ADC_value = ADC12MEM0;
        TA0CCR1 = ADC12MEM0/5; //changes duty cycle

    __bic_SR_register_on_exit(LPM0_bits);   // Exit active CPU
  case  8: break;                           // Vector  8:  ADC12IFG1
  case 10: break;                           // Vector 10:  ADC12IFG2
  case 12: break;                           // Vector 12:  ADC12IFG3
  case 14: break;                           // Vector 14:  ADC12IFG4
  case 16: break;                           // Vector 16:  ADC12IFG5
  case 18: break;                           // Vector 18:  ADC12IFG6
  case 20: break;                           // Vector 20:  ADC12IFG7
  case 22: break;                           // Vector 22:  ADC12IFG8
  case 24: break;                           // Vector 24:  ADC12IFG9
  case 26: break;                           // Vector 26:  ADC12IFG10
  case 28: break;                           // Vector 28:  ADC12IFG11
  case 30: break;                           // Vector 30:  ADC12IFG12
  case 32: break;                           // Vector 32:  ADC12IFG13
  case 34: break;                           // Vector 34:  ADC12IFG14
  default: break;
  }
}




//uart interrupt vector
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
  switch(__even_in_range(UCA0IV,4))
  {
  case 0:break;   // Vector 0 - no interrupt
  case 2:{
      while (!(UCA0IFG&UCTXIFG));  // USCI_A0 TX buffer check

      //if input is A print out the ADC
        if(UCA0RXBUF == 65){
            UCA0TXBUF = ADC_value; //skips a clockcycle of setting adc_value
        }
        //else print out 66
        else{
            UCA0TXBUF = 66;
        }

          break;

  }

  case 4:break;    // Vector 4 - TXIFG
  default: break;
  }
}



```

### MSP430F5529 MATLAB Code

```c
%Receive Serial Sensor MSP430F5529
%Gets 3 sensor reads over 3 seconds
%increased max y limit to 260
%Matt Mammarelli
%9/27/17


delete(instrfindall); %prevents cant use com port error

clear all;
clf
Ts = 1;   % Sampling time
t = 1:Ts:6; % support of signal

p=serial('COM8','BaudRate',9600,'DataBits',8); %have to change com to current one
set(p,'Parity','none');
set(p,'Terminator','LF');
fopen(p); %opens com port
fprintf(p,'A') %sends character A to UART to receive sensor info


pause(1); %delay one second
fprintf(p,'A')


pause(1); %delay one second
fprintf(p,'A')


veri=fscanf(p,'%u'); %gets UART sensor information

figure %creates figure
xlabel('Count')
ylabel('Sense Change')
title('Photoresistor Count vs. Sense')
grid on;
hold on;


for t=1:Ts:6
    if (mod(t,2)==1) %only use odd matrix indices, evens are return characters
    plot(t,uint8(veri(t)),'o'); %plots circles 
    end
    xlim ([0 6]) %x limit
    ylim ([0 260]) %y limit
    drawnow    
end

    

fclose(p); %closes serial connection
delete(p); %deletes serial variable


```
### Combined MATLAB,RGB Plot
![alt text](CombinedMATLABRGB/MSP430F5529/combined_matlab.png "Combined MATLAB RGB")