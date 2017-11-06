/*
 * I2C_Accel_MPU6050
 *
 * Version 1:  Read raw accelerometer X, Y and Z data continuously
 *
 *  P1.6                    UCB0SCL
 *  P1.7                    UCB0SDA
 *
 * MPU-6050 Accelerometer & Gyro
 * NOTE:  Default state of the MPU-6050 is SLEEP mode.
 *        Wake up by writing 0x00 to the PWR_MGMT_1 register
 * NOTE:  No-name version from Amazon has a 5V to 3.3V regulator, and Vcc MUST be 5V !!!
 *        10-kOhm pull-up resistors are included on the board
 *        330-Ohm resistors are included in series on SCL and SDA
 *                (safe to connect P1.6 & P1.7 directly to SCL and SDA)
 *
 * Slave address:   0x68 (AD0=0) or 0x69 (AD0=1)
 *
 * Z-data buffer addresses:
 *                  0x3B ACCEL_XOUT_H R ACCEL_XOUT[15:8]
 *                  0x3C ACCEL_XOUT_L R ACCEL_XOUT[ 7:0]
 *                  0x3D ACCEL_YOUT_H R ACCEL_YOUT[15:8]
 *                  0x3E ACCEL_YOUT_L R ACCEL_YOUT[ 7:0]
 *                  0x3F ACCEL_ZOUT_H R ACCEL_ZOUT[15:8]
 *                  0x40 ACCEL_ZOUT_L R ACCEL_ZOUT[ 7:0]
 *
 * pins not used:   INT (interrupt for data ready in the 1024-byte FIFO bufer)
 *                  XCL, XDA (external clock and data lines for MPU-6050 I2C bus)
 *
 * Reading the raw values: disable sleep mode
 *                  0x6B PWR_MGMT_1 --> set to 0
 *
 */
#include <msp430.h>

/*
 * main.c
 */

unsigned char RX_Data[6];
unsigned char TX_Data[2];
unsigned char RX_ByteCtr;
unsigned char TX_ByteCtr;

int xAccel;
int yAccel;
int zAccel;

unsigned char slaveAddress = 0x68;  // Set slave address for MPU-6050
                    // 0x68 for ADD pin=0
                    // 0x69 for ADD pin=1

const unsigned char PWR_MGMT_1   = 0x6B;    // MPU-6050 register address
const unsigned char ACCEL_XOUT_H = 0x3B;    // MPU-6050 register address
const unsigned char ACCEL_XOUT_L = 0x3C;    // MPU-6050 register address
const unsigned char ACCEL_YOUT_H = 0x3D;    // MPU-6050 register address
const unsigned char ACCEL_YOUT_L = 0x3E;    // MPU-6050 register address
const unsigned char ACCEL_ZOUT_H = 0x3F;    // MPU-6050 register address
const unsigned char ACCEL_ZOUT_L = 0x40;    // MPU-6050 register address

void i2cInit(void);
void i2cWrite(unsigned char);
void i2cRead(unsigned char);

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;               // Stop WDT

    // set up I2C pins
    P4SEL |= BIT1 + BIT2;                   // Assign I2C pins to USCI_B0


    // Initialize the I2C state machine
    i2cInit();

    // Wake up the MPU-6050
    slaveAddress = 0x68;                    // MPU-6050 address
    TX_Data[1] = 0x6B;                      // address of PWR_MGMT_1 register
    TX_Data[0] = 0x00;                      // set register to zero (wakes up the MPU-6050)
    TX_ByteCtr = 2;
    i2cWrite(slaveAddress);

    while (1)
    {
        // Point to the ACCEL_ZOUT_H register in the MPU-6050
        slaveAddress = 0x68;                    // MPU-6050 address
        TX_Data[0] = 0x3B;                  // register address
        TX_ByteCtr = 1;
        i2cWrite(slaveAddress);

        // Read the two bytes of data and store them in zAccel
        slaveAddress = 0x68;                    // MPU-6050 address
        RX_ByteCtr = 6;
        i2cRead(slaveAddress);

        xAccel  = RX_Data[5] << 8;             // MSB
        xAccel |= RX_Data[4];                   // LSB
        yAccel  = RX_Data[3] << 8;              // MSB
        yAccel |= RX_Data[2];                   // LSB
        zAccel  = RX_Data[1] << 8;              // MSB
        zAccel |= RX_Data[0];                   // LSB


        // do something with the data

        __delay_cycles(200);
        __no_operation();                       // Set breakpoint >>here<< and read

    }
}

//*********************************************************************************************
void i2cInit(void)
{
    // set up I2C module
    UCB1CTL1 |= UCSWRST;                // Enable SW reset
    UCB1CTL0 = UCMST + UCMODE_3 + UCSYNC;           // I2C Master, synchronous mode
    UCB1CTL1 = UCSSEL_2 + UCSWRST;          // Use SMCLK, keep SW reset
    UCB1BR0 = 10;                   // fSCL = SMCLK/12 = ~100kHz
    UCB1BR1 = 0;
    UCB1CTL1 &= ~UCSWRST;               // Clear SW reset, resume operation
}

//*********************************************************************************************
void i2cWrite(unsigned char address)
{
    __disable_interrupt();
    UCB1I2CSA = address;                // Load slave address
    UCB1IE |= UCTXIE;                // Enable TX interrupt
    while(UCB1CTL1 & UCTXSTP);          // Ensure stop condition sent
    UCB1CTL1 |= UCTR + UCTXSTT;         // TX mode and START condition
    __delay_cycles(200);
    __bis_SR_register(CPUOFF + GIE);        // sleep until UCB0TXIFG is set ...
}

//*********************************************************************************************
void i2cRead(unsigned char address)
{
    __disable_interrupt();
    UCB1I2CSA = address;                // Load slave address
    UCB1IE |= UCRXIE;                // Enable RX interrupt
    while(UCB1CTL1 & UCTXSTP);          // Ensure stop condition sent
    UCB1CTL1 &= ~UCTR;              // RX mode
    UCB1CTL1 |= UCTXSTT;                // Start Condition
    __delay_cycles(200);
    __bis_SR_register(CPUOFF + GIE);        // sleep until UCB0RXIFG is set ...
}

/**********************************************************************************************/
// USCIAB0TX_ISR
#pragma vector = USCI_B1_VECTOR
__interrupt void USCI_B1_ISR(void)
{
    if(UCB1CTL1 & UCTR)                 // TX mode (UCTR == 1)
    {
        if (TX_ByteCtr)                     // TRUE if more bytes remain
        {
            TX_ByteCtr--;               // Decrement TX byte counter
            UCB1TXBUF = TX_Data[TX_ByteCtr];    // Load TX buffer
        }
        else                        // no more bytes to send
        {
            UCB1CTL1 |= UCTXSTP;            // I2C stop condition
            UCB1IFG &= ~UCTXIFG;         // Clear USCI_B0 TX int flag
            __bic_SR_register_on_exit(CPUOFF);  // Exit LPM0
        }
    }
    else // (UCTR == 0)                 // RX mode
    {
        RX_ByteCtr--;                       // Decrement RX byte counter
        if (RX_ByteCtr)                     // RxByteCtr != 0
        {
            RX_Data[RX_ByteCtr] = UCB1RXBUF;    // Get received byte
            if (RX_ByteCtr == 1)            // Only one byte left?
            UCB1CTL1 |= UCTXSTP;            // Generate I2C stop condition
        }
        else                        // RxByteCtr == 0
        {
            RX_Data[RX_ByteCtr] = UCB1RXBUF;    // Get final received byte
            __bic_SR_register_on_exit(CPUOFF);  // Exit LPM0
        }
    }
}
