#include "adxl343.h"
#include "board.h"
#include <stdint.h>

/**************************************************************************/
/*!
    @brief  Writes 8-bits to the specified destination register

    @param data The register + data to write
    @param size size of register + data

    @return Bool of status
*/
/**************************************************************************/

bool adxl343_writeRegister(uint8_t data[], uint8_t size) {
    I2C_XFER_T xfer = {.slaveAddr = ADXL343_ADDRESS,
                       .txBuff = data,
                       .txSz = size,
                       .rxBuff = NULL,
                       .rxSz = 0};
    I2C_STATUS_T status = Chip_I2C_MasterTransfer(I2C0, &xfer);
    return status == I2C_STATUS_DONE;
}

/**************************************************************************/
/*!
    @brief  Reads 8-bits from the specified register

    @param reg register to read

    @param data pointer to data to read

    @param size of data array

    @return bool of status
*/
/**************************************************************************/
bool adxl343_readRegister(uint8_t reg, uint8_t data[], uint8_t size) {
    I2C_XFER_T xfer = {.slaveAddr = ADXL343_ADDRESS,
                       .txBuff = &reg,
                       .txSz = 1,
                       .rxBuff = data,
                       .rxSz = size};
    I2C_STATUS_T status = Chip_I2C_MasterTransfer(I2C0, &xfer);
    return status == I2C_STATUS_DONE;

}

/**************************************************************************/
/*!
    @brief  Read the device ID (can be used to check connection)

    @return The 8-bit device ID
*/
/**************************************************************************/
uint8_t adxl343_getDeviceID(void) {
    // Check device ID register
    uint8_t deviceID;
    if (adxl343_readRegister(ADXL3XX_REG_DEVID, &deviceID, 1)) {
        return deviceID;
    }
    return 0;
}

// /**************************************************************************/
// /*!
//     @brief  Enables (1) or disables (0) the interrupts on the specified
//             interrupt pin.

//     @param cfg The bitfield of the interrupts to enable or disable.

//     @return True if the operation was successful, otherwise false.
// */
// /**************************************************************************/
// bool adxl343_enableInterrupts(int_config cfg) {
//   /* Update the INT_ENABLE register with 'config'. */
//   writeRegister(ADXL3XX_REG_INT_ENABLE, cfg.value);

//   /* ToDo: Add proper error checking! */
//   return true;
// }

// /**************************************************************************/
// /*!
//     @brief  'Maps' the specific interrupt to either pin INT1 (bit=0),
//             of pin INT2 (bit=1).

//     @param cfg The bitfield of the interrupts to enable or disable.

//     @return True if the operation was successful, otherwise false.
// */
// /**************************************************************************/
// bool adxl343_mapInterrupts(int_config cfg) {
//   /* Update the INT_MAP register with 'config'. */
//   writeRegister(ADXL3XX_REG_INT_MAP, cfg.value);

//   /* ToDo: Add proper error checking! */
//   return true;
// }

// /**************************************************************************/
// /*!
//     @brief  Reads the status of the interrupt pins. Reading this register
//             also clears or deasserts any currently active interrupt.

//     @return The 8-bit content of the INT_SOURCE register.
// */
// /**************************************************************************/
// uint8_t adxl343_checkInterrupts(void) {
//   return readRegister(ADXL3XX_REG_INT_SOURCE);
// }

// /**************************************************************************/
// /*!
//     @brief  Gets the most recent X axis value

//     @return The 16-bit signed value for the X axis
// */
// /**************************************************************************/
// int16_t adxl343_getX(void) { return read16(ADXL3XX_REG_DATAX0); }

// /**************************************************************************/
// /*!
//     @brief  Gets the most recent Y axis value

//     @return The 16-bit signed value for the Y axis
// */
// /**************************************************************************/
// int16_t adxl343_getY(void) { return read16(ADXL3XX_REG_DATAY0); }

// /**************************************************************************/
// /*!
//     @brief  Gets the most recent Z axis value

//     @return The 16-bit signed value for the Z axis
// */
// /**************************************************************************/
// int16_t adxl343_getZ(void) { return read16(ADXL3XX_REG_DATAZ0); }

/**************************************************************************/
/*!
    @brief  Reads 3x16-bits from the x, y, and z data register
    @param x reference to return x acceleration data
    @param y reference to return y acceleration data
    @param z reference to return z acceleration data
    @return True if the operation was successful, otherwise false.
*/
/**************************************************************************/
bool adxl343_getXYZ(int16_t *x, int16_t *y, int16_t *z) {
    int16_t buffer[] = {0, 0, 0};

    if(adxl343_readRegister(ADXL3XX_REG_DATAX0, (uint8_t *)&buffer, 6)) {
        *x = buffer[0];
        *y = buffer[1];
        *z = buffer[2];
        return true;
    }
    else {
        return false;
    }

}

/**************************************************************************/
/*!
    @brief  Setups the HW (reads coefficients values, etc.)
    @param  i2caddr The 7-bit I2C address to find the ADXL on
    @return True if the sensor was successfully initialised.
*/
/**************************************************************************/
bool adxl343_begin() {

    /* Check connection */
    uint8_t deviceid = adxl343_getDeviceID();
    if (deviceid != 0xE5) {
        /* No ADXL343 detected ... return false */
        return false;
    }

    // TODO implement range
    // _range = ADXL343_RANGE_2_G;


    // Default tap detection level (2G, 31.25ms duration, single tap only)
    // If only the single tap function is in use, the single tap interrupt
    // is triggered when the acceleration goes below the threshold, as
    // long as DUR has not been exceeded.
    // TODO add INIT checks
    uint8_t message[2] = {ADXL3XX_REG_INT_ENABLE, 0}; // Disable interrupts to start
    if(!adxl343_writeRegister(message, 2))
    {
        return false;
    }
    message[0] = ADXL3XX_REG_THRESH_TAP; // 62.5 mg/LSB (so 0xFF = 16 g)
    message[1] = 20; 
    if(!adxl343_writeRegister(message, 2)) 
    {
        return false;
    }
    message[0] = ADXL3XX_REG_DUR; // Max tap duration, 625 µs/LSB
    message[1] = 50; 
    if(!adxl343_writeRegister(message, 2)) 
    {
        return false;
    }
    message[0] = ADXL3XX_REG_LATENT; // Tap latency, 1.25 ms/LSB, 0=no double tap
    message[1] = 0; 
    if(!adxl343_writeRegister(message, 2)) 
    {
        return false;
    }
    message[0] = ADXL3XX_REG_WINDOW; // Waiting period,  1.25 ms/LSB, 0=no double tap
    message[1] = 0; 
    if(!adxl343_writeRegister(message, 2)) 
    {
        return false;
    }
    message[0] = ADXL3XX_REG_TAP_AXES; // Enable the XYZ axis for tap
    message[1] = 0x7; 
    if(!adxl343_writeRegister(message, 2)) 
    {
        return false;
    }
    message[0] = ADXL3XX_REG_POWER_CTL; // Enable measurements
    message[1] = 0x08; 
    if(!adxl343_writeRegister(message, 2)) 
    {
        return false;
    }

    return true; // If all operations have been executed successfully

}

// /**************************************************************************/
// /*!
//     @brief  Sets the g range for the accelerometer

//     @param range The range to set, based on adxl34x_range_t
// */
// /**************************************************************************/
// void adxl343_setRange(adxl34x_range_t range) {
//   Adafruit_BusIO_Register data_format_reg =
//       Adafruit_BusIO_Register(i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC,
//                               ADXL3XX_REG_DATA_FORMAT, 1);

//   Adafruit_BusIO_RegisterBits range_bits =
//       Adafruit_BusIO_RegisterBits(&data_format_reg, 2, 0);

//   Adafruit_BusIO_RegisterBits full_range_bit =
//       Adafruit_BusIO_RegisterBits(&data_format_reg, 1, 3);

//   /* Update the data rate */
//   range_bits.write(range);
//   /* Make sure that the FULL-RES bit is enabled for range scaling */
//   full_range_bit.write(true);

//   /* Keep track of the current range (to avoid readbacks) */
//   _range = range;
// }

// /**************************************************************************/
// /*!
//     @brief  Sets the g range for the accelerometer

//     @return The adxl34x_range_t value corresponding to the sensors range
// */
// /**************************************************************************/
// adxl34x_range_t adxl343_getRange(void) {
//   Adafruit_BusIO_Register data_format_reg =
//       Adafruit_BusIO_Register(i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC,
//                               ADXL3XX_REG_DATA_FORMAT, 1);

//   Adafruit_BusIO_RegisterBits range_bits =
//       Adafruit_BusIO_RegisterBits(&data_format_reg, 2, 0);
//   return (adxl34x_range_t)range_bits.read();
// }

// /**************************************************************************/
// /*!
//     @brief  Sets the data rate for the ADXL343 (controls power consumption)

//     @param dataRate The data rate to set, based on adxl3xx_dataRate_t
// */
// /**************************************************************************/
// void adxl343_setDataRate(adxl3xx_dataRate_t dataRate) {
//   /* Note: The LOW_POWER bits are currently ignored and we always keep
//      the device in 'normal' mode */
//   writeRegister(ADXL3XX_REG_BW_RATE, dataRate);
// }

// /**************************************************************************/
// /*!
//     @brief  Sets the data rate for the ADXL343 (controls power consumption)

//     @return The current data rate, based on adxl3xx_dataRate_t
// */
// /**************************************************************************/
// adxl3xx_dataRate_t adxl343_getDataRate(void) {
//   Adafruit_BusIO_Register bw_rate_reg = Adafruit_BusIO_Register(
//       i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC, ADXL3XX_REG_BW_RATE, 1);

//   Adafruit_BusIO_RegisterBits rate_bits =
//       Adafruit_BusIO_RegisterBits(&bw_rate_reg, 4, 0);

//   return (adxl3xx_dataRate_t)rate_bits.read();
// }

// /**************************************************************************/
// /*!
//     @brief  Retrieves the X Y Z trim offsets, note that they are 4 bits signed
//     but we use int8_t to store and 'extend' the sign bit!
//     @param x Pointer to the x offset, from -5 to 4 (internally multiplied by 8
//    lsb)
//     @param y Pointer to the y offset, from -5 to 4 (internally multiplied by 8
//    lsb)
//     @param z Pointer to the z offset, from -5 to 4 (internally multiplied by 8
//    lsb)
// */
// /**************************************************************************/
// void adxl343_getTrimOffsets(int8_t *x, int8_t *y, int8_t *z) {
//   Adafruit_BusIO_Register x_off = Adafruit_BusIO_Register(
//       i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC, ADXL3XX_REG_OFSX, 1);
//   if (x != NULL)
//     *x = x_off.read();
//   Adafruit_BusIO_Register y_off = Adafruit_BusIO_Register(
//       i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC, ADXL3XX_REG_OFSY, 1);
//   if (y != NULL)
//     *y = y_off.read();
//   Adafruit_BusIO_Register z_off = Adafruit_BusIO_Register(
//       i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC, ADXL3XX_REG_OFSZ, 1);
//   if (z != NULL)
//     *z = z_off.read();

//   return;
// }

// /**************************************************************************/
// /*!
//     @brief  Sets the X Y Z trim offsets, note that they are 4 bits signed
//     but we use int8_t to store and 'extend' the sign bit!
//     @param x The x offset, from -5 to 4 (internally multiplied by 8 lsb)
//     @param y The y offset, from -5 to 4 (internally multiplied by 8 lsb)
//     @param z The z offset, from -5 to 4 (internally multiplied by 8 lsb)
// */
// /**************************************************************************/
// void adxl343_setTrimOffsets(int8_t x, int8_t y, int8_t z) {
//   Adafruit_BusIO_Register x_off = Adafruit_BusIO_Register(
//       i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC, ADXL3XX_REG_OFSX, 1);
//   x_off.write(x);

//   Adafruit_BusIO_Register y_off = Adafruit_BusIO_Register(
//       i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC, ADXL3XX_REG_OFSY, 1);
//   y_off.write(y);

//   Adafruit_BusIO_Register z_off = Adafruit_BusIO_Register(
//       i2c_dev, spi_dev, AD8_HIGH_TOREAD_AD7_HIGH_TOINC, ADXL3XX_REG_OFSZ, 1);
//   z_off.write(z);

//   return;
// }


