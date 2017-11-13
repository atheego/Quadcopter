/*
 *            i2c.c
 *
 *   Created on:  November 12, 2017
 *  Last Edited:  November 12, 2017
 *       Author:  Nick Gorab
 *        Board:  MSP430FR5994
 */

#include "I2C.h"


void i2cSingleWrite(char address, char data){
    UCB2I2CSA = address;            // Sets slave address
    UCB2CTL1 &= ~UCSWRST;           // Disables reset, USCI starts operation
    UCB2CTL1 |= UCTR                // TX Mode
              + UCTXSTT;            // Sends start condition
    while(!(UCB2IFG & UCTXIFG));    // Waits until the TX buffer is clear
    UCB2TXBUF = data;               // Sends data
    UCB2CTL1 |= UCTXSTP;            // Stop condition
    while(!(UCB2CTL1 & UCTXSTP));   // Waits for stop condition to be sent
    UCB2IFG  &= ~UCTXIFG;           // Clears TX flag
}


void i2cSingleRead(char address, char *data){
    UCB2I2CSA = address;            // Sets slave address
    UCB2CTL1 &= ~UCSWRST;           // Disables reset, USCI starts operation
    UCB2CTL1 &= ~UCTR;              // RX Mode
    UCB2CTL1 |= UCTXSTT;            // Sends start condition
    while(UCB0CTL1 & UCTXSTT);      // Waits until start condition is sent
    UCB2CTL1 |= UCTXSTP;            // Sends stop byte
    *data = UCB2RXBUF;
}


void i2cMultipleWrite(char address, char *data, int size){
    int byteCtr = 0;                    // Initializes counter
    UCB2I2CSA = address;                // Sets slave address
    UCB2CTL1 &= ~UCSWRST;               // Disables reset, USCI starts operation
    UCB2CTL1 |= UCTR                    // Enters TX mode
              + UCTXSTT;                // Sends start condition
    while (byteCtr <= size){            // Only operates if there are available bytes
        while(!(UCB2IFG & UCTXIFG));    // Waits for the transmitting buffer to be empty
        UCB2TXBUF = *data;              // Transmits byte
        byteCtr++;                      // Increment byte counter
        data++;                         // Increment data address
    }
    UCB2CTL1 |= UCTXSTP;                // Stop condition
    while(!(UCB2CTL1 & UCTXSTP));       // Waits for stop condition to be sent
    UCB2IFG  &= ~UCTXIFG;               // Clears TX flag
}


void i2cMultipleRead(char address, char *data, int size){
    int byteCtr = 0;                    // Initializes counter
    UCB2I2CSA = address;                // Sets slave address
    UCB2CTL1 &= ~UCSWRST;               // Disables reset, USCI starts operation
    UCB2CTL1 &= ~UCTR;                  // Enters RX mode
    UCB2CTL1 |= UCTXSTT;                // Sends start condition
    while (byteCtr <= size){            // Checks to make sure there is still something to recieve
        while(!(UCB2IFG & UCRXIFG));    // Waits for the receiving buffer to be empty
        *data = UCB2RXBUF;              // Incoming data is sent to the RX buffer
        byteCtr++;                      // Increment byte counter
        data++;                         // Increment data counter
    }
    UCB2CTL1 |= UCTXSTP;                // Sends stop condition
    while(!(UCB2CTL1 & UCTXSTP));       // Waits for stop condition to be sent
    UCB2IFG  &= ~UCRXIFG;               // Clears RX flag
}

