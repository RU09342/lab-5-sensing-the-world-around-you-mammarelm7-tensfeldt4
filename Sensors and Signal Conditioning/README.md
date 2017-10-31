# Sensors and Signal Conditioning
The photoresistor, photodiode, and phototransistor are devices that sense light and generate an analog resistance or volage because of it.
Each of these sensors need certain circuits to work which are described below.
The analog signal is converted to a digital signal using the analog to digital converter (ADC) in the MSP430G2553.
This data is then transmitted over 9600 baud uart whenever the character 'A' is sent to the MSP430 and viewed in Realterm. 

## MSP430G2553 code
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

## Breadboard
![alt text](images/breadboard.JPG "breadboard")

Left:Phototransistor Circuit, Middle:Photodiode Circuit, Right: Photoresistor Circuit

## Photoresistor Circuit
![alt text](images/photoresistor.png "Photoresistor")

This circuit is just the photoresistor and a current limiting 1k resistor.
This sensor works fine without any amplification.

## Photoresistor Serial
![alt text](images/photoresistorSerial.png "PhotoresistorSerial")

This is the serial as a flashlight is moved closer and farther away from the sensor.
94 is the ambient without a light and increases when the flashlight is pointed on it.
It decreases when the light is reduced.

## Photodiode Circuit
![alt text](images/photodiode.png "Photodiode")

This circuit requires an inverting op amp with negative feedback in order to work.
The gain of the circuit is directly proportional to the feedback resistor R4 and is characterized by the 
function Vo/Vin = -Rf/Rin where Rin is represented by the 0.7 voltage drop across the photodiode.
The larger the value of the feedback resistor, the larger the gain for this circuit. This being true,
it was found that any value over 300k created enough amplification for this device.

## Photodiode Serial
![alt text](images/photodiodeSerial.png "PhotodiodeSerial")

This is the serial as a flashlight is moved closer and farther away from the sensor. 
This sensor is different from the photoresistor as it is more directional and represents ambient light as 0.
As the flashlight gets closer, the value increases up to 22.

## Phototransistor Circuit
![alt text](images/phototransistor.png "Phototransistor")

This circuit consists of a phototransistor with a 2k current limiting resistor hooked up from the emitter to ground,
since it is a NPN device. The base is controlled by the amount of light that is sensed. Once it 
reaches a threshold, current will flow from the collector to the emitter. This circuit requires 
amplification in order to sense properly. The collector is connected to an op amp 
that is configured as a buffer. A buffer converts a high input impedance to a low output
impedance in order to prevent loads from affecting the gain of the circuit.
The output of this buffer is connected to an inverting op amp that has a 100nF capacitor
in parallel with the feedback resistor in order to cut out high frequencies as a form of debouncing.
The gain of this circuit is characterized as Vo/Vin = -Rf/Rin. A gain of 10 was chosen by choosing
Rf to be 100k and Rin to be 10k. Since you are technically inverting the signal here, you should have a third
stage that inverts the signal back. However, this third stage did not work in practice so this two stage circuit was used instead. 

## Phototransistor Serial
![alt text](images/phototransistorSerial.png "PhototransistorSerial")

This is the serial as a flashlight is moved closer and farther away from the sensor. 
This sensor is different from the photoresistor as it is more directional and represents ambient light as 0.
As the flashlight gets closer, the value increases up to 22.