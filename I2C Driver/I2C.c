/*
 *      	  I2C.c
 *
 *   Created on:  November 7, 2017
 *  Last Edited:  November 8, 2017
 *       Author:  Nick Gorab
 *        Board:  MSP430F5529
 */

#include <I2C.h>

void i2cInit(){
	P3SEL    |= BIT0 		// SDA line for I2C using UCB0
		      + BIT1;   	// SCL line for I2C using UCB0
	UCB0CTL1 |= UCSWRST;	// USCI held in reset for initialization
	UCB0CTL0  = UCMST		// Master Mode
			  + UCMODE_3	// I2C Mode
			  + UCSYNC		// Synchronus Mode
	UCB0BR0   = 12;			// Clock prescaler, SMCLK/12
	UCB0BR1   = 0;			// No Modulation
}



void i2cSingleWrite(char address, char data){
	UCB0I2CSA = address;			// Sets slave address
	UCB0CTL1 &= ~UCSWRST;			// Disables reset, USCI starts operation
	UCB0CTL1 |= UCTR 				// TX Mode
			  + UCTXSTT;			// Sends start condition
	while(!(UCB0IFG & UCTXIFG));	// Waits until the TX buffer is clear
	UCB0TXBUF = data;				// Sends data
	UCB0CTL1 |= UCTXSTP;			// Stop condition
	while(!(UCB1CTL1 & UCTXSTP));	// Waits for stop condition to be sent
	UCB0IFG  &= ~UCTXIFG;			// Clears TX flag 
}



void i2cSingleRead(char address, char *data){
	UCB0I2CSA = address;			// Sets slave address
	UCB0CTL1 &= ~UCSWRST;			// Disables reset, USCI starts operation
	UCB0CTL1 &= ~UCTR 				// RX Mode
	UCB0CTL1 |= UCTXSTT;			// Sends start condition
	while(UCB0CTL1 & UCTXSTT);		// Waits until start condition is sent
	UCB0CTL1 |= UCTXSTP;			// Sends stop byte
	*data = UCB0RXBUF;
}



void i2cMultipleWrite(char address, char *data, int size){
	int byteCtr = 0						// Initializes counter
	UCB0I2CSA = address;				// Sets slave address
	UCB0CTL1 &= ~UCSWRST;				// Disables reset, USCI starts operation
	UCB0CTL1 |= UCTR 					// Enters TX mode
			  + UCTXSTT;				// Sends start condition
	while (byteCtr <= size){			// Only operates if there are available bytes
		while(!(UCB0IFG & UCTXIFG));	// Waits for the transmitting buffer to be empty
		UCB0TXBUF = *data;				// Transmits byte
		byteCtr++;						// Increment byte counter
		data++;							// Increment data address
	}
	UCB0CTL1 |= UCTXSTP;				// Stop condition
	while(!(UCB1CTL1 & UCTXSTP));		// Waits for stop condition to be sent
	UCB0IFG  &= ~UCTXIFG;				// Clears TX flag 
}



void i2cMultipleRead(char address, char *data, int size){
	int byteCtr = 0;					// Initializes counter
	UCB0I2CSA = address;  				// Sets slave address
	UCB0CTL1 &= ~UCSWRST;    			// Disables reset, USCI starts operation
 	UCB0CTL1 &= ~UCTR;       			// Enters RX mode
 	UCB0CTL1 |= UCTXSTT;     			// Sends start condition
 	while (byteCtr <= size){			// Checks to make sure there is still something to recieve
 		while(!(UCB0IFG & UCRXIFG));	// Waits for the receiving buffer to be empty
 		*data = UCB0RXBUF;				// Incoming data is sent to the RX buffer
 		byteCtr++;						// Increment byte counter
 		data++;							// Increment data counter
 	}
 	UCB0CTL1 |= UCTXSTP					// Sends stop condition
	while(!(UCB0CTL1 & UCTXSTP));		// Waits for stop condition to be sent
	UCB0IFG  &= ~UCRXIFG;				// Clears RX flag
}
