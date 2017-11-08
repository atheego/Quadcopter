/*
 *      	  I2C.h
 *
 *   Created on:  November 7, 2017
 *  Last Edited:  November 8, 2017
 *       Author:  Nick Gorab
 *        Board:  MSP430F5529
 */


#ifndef I2C_H_
#define I2C_H_

#include <msp430.h>

void i2cInit();													// Initializes I2C communication
void i2cSingleWrite(char address, char data);					// Writes single byte of data
void i2cSingleRead (char address, char *data);					// Reads single byte of data
void i2cMultipleWrite(char address, char *data, int size);		// Writes multiple bytes of data
void i2cMultipleRead (char address, char *data, int size);		// Reads multiple bytes of data


#endif /* I2C_H_ */