/*
 *            battCheck.c
 *
 *   Created on:  November 12, 2017
 *  Last Edited:  November 12, 2017
 *       Author:  Nick Gorab
 *        Board:  MSP430FR5994
 */

#include "system.h"

volatile int battStat = 0;
volatile int Kp       = 10000;
volatile int Ki       = 6000;
volatile int Kd       = 350;
volatile float error,
               oldError,
               iError,
               dError,
               dt,
               u;



/***********************\
 *                     *
 * Batt Check Function *
 *                     *
\***********************/

void battCheck(void){
    ADC12CTL0 |= ADC12ENC       // Enables ADC
              |  ADC12SC;       // Takes a reading
    if(ADC12MEM0 < 3105) {      // If battry voltage < 2.5 V
        battStat = 1;           // Raises low batt flag
    } else {
        battStat = 0;           // Continue normal operation
    }
    switch(battStat){
        case 0:                 // Battery is good
            TA0CTL |=  MC_0     // Stops timer
                   |   TACLR;   // Clears timer
            P1SEL0 &= ~BIT0;    // Sets P1.0 to GPIO output
            P1OUT   =  BIT0;    // Turns the LED on 
        break;      
        case 1:                 // Battery is low
            P1SEL0  = BIT0;     // Sets P1.0 to TA0.1 control
            TA0CTL |= MC_2;     // Starts the clock in continous mode
        break;
    }
}






/***********************\
 *                     *
 *    Main Function    *
 *                     *
\***********************/

void main(void){
    WDTCTL = WDTPW | WDTHOLD;   // Disables Watchdog Timer
    PM5CTL0 &= ~LOCKLPM5;       // Disables high-impedance mode
    adcInit();                  // Initializes the ADC
    ledInit();                  // Initializes the LEDs
    timerAInit();               // Initializes TIMER_A
    while(1){
        battCheck();
    }
}
