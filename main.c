/*
 *            main.c
 *   Created on:  November 7, 2017
 *  Last Edited:  November 7, 2017
 *       Author:  Nick Gorab
 *        Board:  MSP430F5529
 */

#include <msp430.h>
#include <I2C.h>

int xAccel;
int yAccel;
int zAccel;

unsigned char slaveAddr = 0x68;
unsigned char TX_Data[2];
unsigned char RX_Data[6];

const unsigned char accel = 0x3B;
const unsigned char gyro  = 0xB5; // Not correct


void main(void){
    WDTCTL = WDTPW + WDTHOLD;
    i2cInit();
    TX_Data[0] = 0x6B;
    TX_Data[1] = 0x00;
    i2cMultipleWrite(slaveAddr, TX_Data, 2);
    while(1){
        i2cSingleWrite(slaveAddr, accel);
        i2cMultipleRead(slaveAddr, RX_Data, 6);

        xAccel  = RX_Data[5] << 8;
        xAccel |= RX_Data[4];
        yAccel  = RX_Data[3] << 8;
        yAccel |= RX_Data[2];
        zAccel  = RX_Data[1] << 8;
        zAccel |= RX_Data[0];

        __delay_cycles(200);
    }
}
