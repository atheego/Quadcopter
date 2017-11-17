/*
 *            initializer.c
 *
 *   Created on:  November 12, 2017
 *  Last Edited:  November 12, 2017
 *       Author:  Nick Gorab
 *        Board:  MSP430FR5994
 */

/***********************************************************************************************************\
*                                                                                                           *
*  The purpose of this file is to enable all of the relevant registers used in this code to avoid clutter.  *
*                                                                                                           *
\***********************************************************************************************************/

#include "initializer.h"


/************************\
*                        *
*   ADC Initialization   *
*                        *
\************************/

void adcInit(void){
    P1SEL0    |= BIT2;          // ADC readings taken on A2 (Pin 1.2)
    P1SEL1    |= BIT2;          // ADC readings taken on A2 (Pin 1.2)
    ADC12CTL0  = ADC12ON        // Turns on ADC12
               + ADC12SHT0_8;   // Sets sampling time
    ADC12CTL1  = ADC12SHP;      // Signal comes from sampling timer instead of input
    ADC12MCTL0 = ADC12INCH_2;   // Selects input channel 2 (A2)
}



/************************\
*                        *
*   LED Initialization   *
*                        *
\************************/

void ledInit(void){
    P1DIR |= BIT0;       // Red LED. Real exciting function, I know.
}



/************************\
*                        *
* TIMER_B Initialization *
*                        *
\************************/

void timerBInit(void){
    TB0CCTL0 = CCIE;        // Enables timer interrupts
    TB0CTL   = TBSSEL_1     // SMCLK
             + MC_1;        // Runs in up-mode
    TB0CCTL1 = OUTMOD_7;    // Set/Reset Mode
    TB0CCTL2 = OUTMOD_7;    // Set/Reset Mode
    TB0CCTL3 = OUTMOD_7;    // Set/Reset Mode
    TB0CCTL4 = OUTMOD_7;    // Set/Reset Mode
    TB0CCR0  = 100;         // Counts up to 100
}



/************************\
*                        *
* TIMER_A Initialization *
*                        *
\************************/

void timerAInit(void){
    TA0CTL   = TASSEL_2 + MC_2 + ID_3;  // SMCLK, Up-Mode, Predivider 8
    TA0CCTL1 = OUTMOD_3;                // Reset/Set behavior
    TA0CCR1  = 32767;                   // CCR1 value for 50% duty
}

/************************\
*                        *
*   I2C Initialization   *
*                        *
\************************/

void i2cInit(){
    P3SEL0    |= BIT0       // SDA line for I2C using UCB0
               + BIT1;      // SCL line for I2C using UCB0
    P3SEL1    |= BIT0       // SDA line for I2C using UCB0
               + BIT1;      // SCL line for I2C using UCB0
    UCB0CTL1  |= UCSWRST;   // USCI held in reset for initialization
    UCB0CTLW0  = UCMST      // Master Mode
               + UCMODE_3   // I2C Mode
               + UCSYNC;    // Synchronus Mode
    UCB2BRW    = 0x0008;    // SMCLK/8
    UCB2CTLW1 |= UCASTP_2;  // Sends stop bit when UCTBCNT is reached
    UCB2CTL1  &= ~UCSWRST;  // Starts state machine
}



/************************\
*                        *
*   PID Initialization   *
*                        *
\************************/

void pidInit(void){
  P1DIR  |= BIT4     // Motor 1 PWM output
         |  BIT5     // Motor 2 PWM output
         |  BIT6     // Motor 3 PWM output
         |  BIT7;    // Motor 4 PWM output
  TA0CTL  = TASSEL_2
         |  ID_0
         |  MC_1;
}
