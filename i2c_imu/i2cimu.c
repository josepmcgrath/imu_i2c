/*
 * Copyright (c) 2016-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *    ======== i2cimu.c ========
 */
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
//#include <ti/drivers/UART.h>
#include <ti/display/Display.h>
#include <SparkFun_BNO080_Library.h>
//#include <UartLog.h>

/* Example/Board Header files */
#include "Board.h"

#define TASKSTACKSIZE       640

static Display_Handle display;
// pin configuration
static PIN_Config SBP_configTable[] =
{
     CC2640R2_LAUNCHXL_DIO21 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

static PIN_State sbpPins;
static PIN_Handle hSbpPins;
static uint8_t imu_nrst = 0;
/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    I2C_Handle i2c;
    I2C_Params i2cParams;

    /* Call driver init functions */
    Display_init();
    GPIO_init();
    I2C_init();

    // Open pin structure for use
      hSbpPins = PIN_open(&sbpPins, SBP_configTable);
      /* POWER CYCLE IMU */
      // set imu_nrst to 0 to reset
      imu_nrst = 0;
      //PIN_setPortOutputValue(hSbpPins, (imu_nrst << CC2640R2_LAUNCHXL_DIO21));
      //sleep(2);
      // set back to 1
      //imu_nrst = 1;
      //PIN_setPortOutputValue(hSbpPins, (imu_nrst << CC2640R2_LAUNCHXL_DIO21));
    /* Configure the LED pin */
    GPIO_setConfig(Board_GPIO_LED0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Open the HOST display for output */
    display = Display_open(Display_Type_UART, NULL);
    if (display == NULL)
    {
        while (1)
            ;
    }

    /* Turn on user LED */
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
    Display_printf(display, 0, 0, "Starting the i2c_imu communication\n");

    /* Create I2C for usage */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2c = I2C_open(Board_I2C_TMP, &i2cParams);
    if (i2c == NULL)
    {
        Display_printf(display, 0, 0, "Error Initializing I2C\n");
        while (1)
            ;
    }
    else
    {
        Display_printf(display, 0, 0, "I2C Initialized!\n");
    }
    /*
    uint8_t packetLength = 5; //Add four bytes for the header

    //Do I2C

    uint8_t txBuffer[32]; // need to store transmission
    uint8_t rxBuffer[32]; // don't really need

        // Place data in txBuffer
        // all transmissions automatically start with the 7 bit slave address
        // 4 byte packet header
        txBuffer[0] = packetLength & 0xFF; // Packet length LSB
        txBuffer[1] = packetLength >> 8; // Packet length MSB (right shift to get rid of lower byte)
        txBuffer[2] = 1; // Channel number
        txBuffer[3] = 0; // Send the sequence number, increments with each packet sent, different counter for each channel
        // data
        txBuffer[4] = 1;
        //if(packetLength > I2C_BUFFER_LENGTH) return(false); //You are trying to send too much. Break into smaller packets.

        // initialize new i2c transaction
        I2C_Transaction i2cTransaction;

        i2cTransaction.slaveAddress = 0x4B;
        i2cTransaction.writeBuf = txBuffer;
        i2cTransaction.writeCount = packetLength;
        i2cTransaction.readBuf = rxBuffer;
        i2cTransaction.readCount = 0;

        if (I2C_transfer(i2c, &i2cTransaction)) // returns true if in BLOCKING and successful transfer, or if in CALLBACK
        {
            Display_printf(display, 0, 0, "Success");
        }
    */
    /* Initialize IMU */


    if (IMU_begin(i2c))
    {
        Display_printf(display, 0, 0, "IMU Initialized!\n");
    }
    else
    {
        Display_printf(display, 0, 0, "Error Initializing IMU\n");
    }


    // Start measurements
    IMU_enableRotationVector(i2c, 1); // send data updates every 50 ms
    IMU_enableGyro(i2c, 1);
    IMU_enableAccelerometer(i2c, 1);
    //sleep(10);
    // Take continuous samples and print them out onto the console
    int j = 0;
    while (j < 20000)
    {
        if (IMU_dataAvailable(i2c))
        {
            float quatI = IMU_getQuatI();
            float quatJ = IMU_getQuatJ();
            float quatK = IMU_getQuatK();
            float quatReal = IMU_getQuatReal();
            float quatRadianAccuracy = IMU_getQuatRadianAccuracy();

            float x = IMU_getAccelX();
            float y = IMU_getAccelY();
            float z = IMU_getAccelZ();

            float xg = IMU_getGyroX();
            float yg = IMU_getGyroY();
            float zg = IMU_getGyroZ();

            Display_printf(display,0,0,"%f", z); // first two values will be zero for some reason
        }
        j++;
        // Sleep for 1 second
        //sleep(1);
    }

    /* Close I2C */
    I2C_close(i2c);
    Display_printf(display, 0, 0, "I2C closed!\n");

    return (NULL);
}
