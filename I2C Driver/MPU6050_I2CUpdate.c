/*
 *      MSP430-Copter Accel.
 *
 *   Created on:  October 31, 2017
 *  Last Edited:  October 31, 2017
 *       Author:  Nick Gorab
 *        Board:  MSP430F5529
 */

#include <msp430.h>

unsigned char RX_Data[6];
unsigned char TX_Data[2];
unsigned char RX_ByteCtr;
unsigned char TX_ByteCtr;
unsigned char slaveAddress = 0x68; 

int xValue;
int yValue;
int zValue;


void i2cInit(void){
    UCB1CTL1 |= UCSWRST;                    // Enable SW reset
    UCB1CTL0 = UCMST + UCMODE_3 + UCSYNC;   // I2C Master, synchronous mode
    UCB1CTL1 = UCSSEL_2 + UCSWRST;          // Use SMCLK, keep SW reset
    UCB1BR0 = 10;                           // SMCLK/12
    UCB1BR1 = 0;                            // No Modulation
    UCB1CTL1 &= ~UCSWRST;                   // Clears reset, resumes operation
}

void i2cWrite(unsigned char address) {
    __disable_interrupt();
    UCB1I2CSA = address;                // Sets address
    UCB1IE |= UCTXIE;                   // Enables TX Interrupt
    while(UCB1CTL1 & UCTXSTP);          // Ensures the stop condition is sent
    UCB1CTL1 |= UCTR + UCTXSTT;         // Sets TX mode and sends start condition
    __delay_cycles(200);                // Gives message some time to send
    __bis_SR_register(LPM0_bits + GIE);  // Enters LPM0 and enbles global interrupt
}

void i2cRead(unsigned char address){
    __disable_interrupt();
    UCB1I2CSA = address;                // Sets address
    UCB1IE |= UCRXIE;                   // Enables RX interrupt
    while(UCB1CTL1 & UCTXSTP);          // Ensures stop condition sent
    UCB1CTL1 &= ~UCTR;                  // Enters RX mode
    UCB1CTL1 |= UCTXSTT;                // Sends start Condition
    __delay_cycles(200);                // Gives message time to send
    __bis_SR_register(LPM0_bits + GIE); // Enters LPM0 and enables global interupt
}

void mpuInit(void){
    TX_Data[0] = 0x6B;              // PWR_MGMT_1 register
    TX_Data[0] = 0x00;              // Sets register value to zero, wakes up MPU
    TX_ByteCtr = 2;                 // Specifies the amount of bytes sent
    i2cWrite(slaveAddress);         // Writes to the register
}

void mpuGyro(void){
    TX_Data[0] = 0x43;              // GYRO_XOUT_H register
    TX_ByteCtr = 1;                 // Specifies the amount of bytes sent
    i2cWrite(slaveAddress);                     // Writes to the register
}

void mpuAccel(void){
    TX_Data[0] = 0x3B;              // ACCEL_XOUT_H register
    TX_ByteCtr = 1;                 // Specifies the amount of bytes sent
    i2cWrite(slaveAddress);
}

int main(void){
    WDTCTL = WDTPW + WDTHOLD;       // Disables Watchdog Timer

    i2cInit();                      // Initializes I2C communication
    mpuInit();                      // Sets up MPU6050 for communication

    while(1) {

        mpuGyro();
        
        RX_ByteCtr = 6;             // Specifies the amount of incoming bytes
        i2cRead(slaveAddress);      // Reads the data coming from the MPU

        xValue = RX_Data[5] << 8;   // Shifts X value MSP
        xValue = RX_Data[4];        // X value LSB
        yValue = RX_Data[3] << 8;   // Shifts Y value MSP
        yValue = RX_Data[2];        // Y value LSB 
        zValue = RX_Data[1] << 8;   // Shifts Z value MSP
        zValue = RX_Data[0];        // Z value LSB

        __delay_cycles(200);           // Gives program some time to breathe?
        __no_operation();
    }


}

#pragma vector = USCI_B1_VECTOR
__interrupt void USCI_BI_ISR(void) {
    if(UCB1CTL1 & UCTR) {                           // Transmitting Mode

        if (TX_ByteCtr) {                           // If the full message hasn't sent
            TX_ByteCtr--;                           // Decrement TX byte count
            UCB1TXBUF = TX_Data[TX_ByteCtr];        // Send the value of the TX_Data array
        } else {                                    // IF there are no more bytes to send
            UCB1CTL1 |= UCTXSTP;                    // I2C stop condition
            UCB1IFG &= ~UCTXIFG;                    // Clear USCI_B0 TX int flag
            __bic_SR_register_on_exit(LPM0_bits);   // Exit LPM0
        }

    } else {                                        // Receiving Mode
        
        RX_ByteCtr--;                               // Decrement RX byte count
        if (RX_ByteCtr) {                           // While there are stil bytes to be received
            RX_Data[RX_ByteCtr] = UCB1RXBUF;        // Get received byte
            if (RX_ByteCtr == 1)                    // Only one byte left?
            UCB1CTL1 |= UCTXSTP;                    // Generate I2C stop condition
        } else {                                    // If there are no more bytes to be received
            RX_Data[RX_ByteCtr] = UCB1RXBUF;        // Recieves final byte
            __bic_SR_register_on_exit(LPM0_bits);   // Exit LPM0
        }
    }
}
