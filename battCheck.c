/*
 *            battCheck.c
 *
 *   Created on:  November 12, 2017
 *  Last Edited:  November 12, 2017
 *       Author:  Nick Gorab
 *        Board:  MSP430FR5994
 */

#include <msp430.h>
#include "initializer.h"
#include "I2C.h"

int battStat = 0;



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
}   }



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

    while(1){
        battCheck();            // Returns the battery voltage
        switch(battStat){
            case 0:
                P1OUT |=  BIT1; // Turns on the green LED
                P1OUT &= ~BIT0; // Turns off the red LED
            break;
            case 1:
                P1OUT |=  BIT0; // Turns on the red LED
                P1OUT &= ~BIT1; // Turns off the green LED
            break;
}   }   }
