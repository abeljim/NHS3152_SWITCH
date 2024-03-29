#ifndef _ADXL343_H
#define _ADXL343_H

#include <stdint.h>
#include <stdbool.h>

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
#define ADXL343_ADDRESS (0x53) /**< Assumes ALT address pin low */
/*=========================================================================*/

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
#define ADXL3XX_REG_DEVID (0x00)        /**< Device ID */
#define ADXL3XX_REG_THRESH_TAP (0x1D)   /**< Tap threshold */
#define ADXL3XX_REG_OFSX (0x1E)         /**< X-axis offset */
#define ADXL3XX_REG_OFSY (0x1F)         /**< Y-axis offset */
#define ADXL3XX_REG_OFSZ (0x20)         /**< Z-axis offset */
#define ADXL3XX_REG_DUR (0x21)          /**< Tap duration */
#define ADXL3XX_REG_LATENT (0x22)       /**< Tap latency */
#define ADXL3XX_REG_WINDOW (0x23)       /**< Tap window */
#define ADXL3XX_REG_THRESH_ACT (0x24)   /**< Activity threshold */
#define ADXL3XX_REG_THRESH_INACT (0x25) /**< Inactivity threshold */
#define ADXL3XX_REG_TIME_INACT (0x26)   /**< Inactivity time */
#define ADXL3XX_REG_ACT_INACT_CTL                                              \
  (0x27) /**< Axis enable control for activity and inactivity detection */
#define ADXL3XX_REG_THRESH_FF (0x28) /**< Free-fall threshold */
#define ADXL3XX_REG_TIME_FF (0x29)   /**< Free-fall time */
#define ADXL3XX_REG_TAP_AXES (0x2A)  /**< Axis control for single/double tap */
#define ADXL3XX_REG_ACT_TAP_STATUS (0x2B) /**< Source for single/double tap */
#define ADXL3XX_REG_BW_RATE (0x2C)     /**< Data rate and power mode control */
#define ADXL3XX_REG_POWER_CTL (0x2D)   /**< Power-saving features control */
#define ADXL3XX_REG_INT_ENABLE (0x2E)  /**< Interrupt enable control */
#define ADXL3XX_REG_INT_MAP (0x2F)     /**< Interrupt mapping control */
#define ADXL3XX_REG_INT_SOURCE (0x30)  /**< Source of interrupts */
#define ADXL3XX_REG_DATA_FORMAT (0x31) /**< Data format control */
#define ADXL3XX_REG_DATAX0 (0x32)      /**< X-axis data 0 */
#define ADXL3XX_REG_DATAX1 (0x33)      /**< X-axis data 1 */
#define ADXL3XX_REG_DATAY0 (0x34)      /**< Y-axis data 0 */
#define ADXL3XX_REG_DATAY1 (0x35)      /**< Y-axis data 1 */
#define ADXL3XX_REG_DATAZ0 (0x36)      /**< Z-axis data 0 */
#define ADXL3XX_REG_DATAZ1 (0x37)      /**< Z-axis data 1 */
#define ADXL3XX_REG_FIFO_CTL (0x38)    /**< FIFO control */
#define ADXL3XX_REG_FIFO_STATUS (0x39) /**< FIFO status */
/*=========================================================================*/

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
#define ADXL343_MG2G_MULTIPLIER (0.004) /**< 4mg per lsb */
/*=========================================================================*/

/** Used with register 0x2C (ADXL3XX_REG_BW_RATE) to set bandwidth */
typedef enum {
  ADXL343_DATARATE_3200_HZ = 0b1111, /**< 3200Hz Bandwidth */
  ADXL343_DATARATE_1600_HZ = 0b1110, /**< 1600Hz Bandwidth */
  ADXL343_DATARATE_800_HZ = 0b1101,  /**<  800Hz Bandwidth */
  ADXL343_DATARATE_400_HZ = 0b1100,  /**<  400Hz Bandwidth */
  ADXL343_DATARATE_200_HZ = 0b1011,  /**<  200Hz Bandwidth */
  ADXL343_DATARATE_100_HZ = 0b1010,  /**<  100Hz Bandwidth */
  ADXL343_DATARATE_50_HZ = 0b1001,   /**<   50Hz Bandwidth */
  ADXL343_DATARATE_25_HZ = 0b1000,   /**<   25Hz Bandwidth */
  ADXL343_DATARATE_12_5_HZ = 0b0111, /**< 12.5Hz Bandwidth */
  ADXL343_DATARATE_6_25HZ = 0b0110,  /**< 6.25Hz Bandwidth */
  ADXL343_DATARATE_3_13_HZ = 0b0101, /**< 3.13Hz Bandwidth */
  ADXL343_DATARATE_1_56_HZ = 0b0100, /**< 1.56Hz Bandwidth */
  ADXL343_DATARATE_0_78_HZ = 0b0011, /**< 0.78Hz Bandwidth */
  ADXL343_DATARATE_0_39_HZ = 0b0010, /**< 0.39Hz Bandwidth */
  ADXL343_DATARATE_0_20_HZ = 0b0001, /**< 0.20Hz Bandwidth */
  ADXL343_DATARATE_0_10_HZ = 0b0000, /**< 0.10Hz Bandwidth (default value) */

  ADXL3XX_DATARATE_3200_HZ = 0b1111, /**< 3200Hz Bandwidth */
  ADXL3XX_DATARATE_1600_HZ = 0b1110, /**< 1600Hz Bandwidth */
  ADXL3XX_DATARATE_800_HZ = 0b1101,  /**<  800Hz Bandwidth */
  ADXL3XX_DATARATE_400_HZ = 0b1100,  /**<  400Hz Bandwidth */
  ADXL3XX_DATARATE_200_HZ = 0b1011,  /**<  200Hz Bandwidth */
  ADXL3XX_DATARATE_100_HZ = 0b1010,  /**<  100Hz Bandwidth */
  ADXL3XX_DATARATE_50_HZ = 0b1001,   /**<   50Hz Bandwidth */
  ADXL3XX_DATARATE_25_HZ = 0b1000,   /**<   25Hz Bandwidth */
  ADXL3XX_DATARATE_12_5_HZ = 0b0111, /**< 12.5Hz Bandwidth */
  ADXL3XX_DATARATE_6_25HZ = 0b0110,  /**< 6.25Hz Bandwidth */
  ADXL3XX_DATARATE_3_13_HZ = 0b0101, /**< 3.13Hz Bandwidth */
  ADXL3XX_DATARATE_1_56_HZ = 0b0100, /**< 1.56Hz Bandwidth */
  ADXL3XX_DATARATE_0_78_HZ = 0b0011, /**< 0.78Hz Bandwidth */
  ADXL3XX_DATARATE_0_39_HZ = 0b0010, /**< 0.39Hz Bandwidth */
  ADXL3XX_DATARATE_0_20_HZ = 0b0001, /**< 0.20Hz Bandwidth */
  ADXL3XX_DATARATE_0_10_HZ = 0b0000  /**< 0.10Hz Bandwidth (default value) */
} adxl3xx_dataRate_t;

/** Used with register 0x31 (ADXL3XX_REG_DATA_FORMAT) to set g range */
typedef enum {
  ADXL343_RANGE_16_G = 0b11, /**< +/- 16g */
  ADXL343_RANGE_8_G = 0b10,  /**< +/- 8g */
  ADXL343_RANGE_4_G = 0b01,  /**< +/- 4g */
  ADXL343_RANGE_2_G = 0b00,  /**< +/- 2g (default value) */

  ADXL34X_RANGE_16_G = 0b11, /**< +/- 16g */
  ADXL34X_RANGE_8_G = 0b10,  /**< +/- 8g */
  ADXL34X_RANGE_4_G = 0b01,  /**< +/- 4g */
  ADXL34X_RANGE_2_G = 0b00   /**< +/- 2g (default value) */
} adxl34x_range_t;

/** Possible interrupts sources on the ADXL343. */
union int_config {
  uint8_t value; /**< Composite 8-bit value of the bitfield.*/
  struct {
    uint8_t overrun : 1;    /**< Bit 0 */
    uint8_t watermark : 1;  /**< Bit 1 */
    uint8_t freefall : 1;   /**< Bit 2 */
    uint8_t inactivity : 1; /**< Bit 3 */
    uint8_t activity : 1;   /**< Bit 4 */
    uint8_t double_tap : 1; /**< Bit 5 */
    uint8_t single_tap : 1; /**< Bit 6 */
    uint8_t data_ready : 1; /**< Bit 7 */
  } bits;                   /**< Individual bits in the bitfield. */
};

/** Possible interrupt pin outputs on the ADXL343. */
typedef enum {
  ADXL343_INT1 = 0,
  ADXL343_INT2 = 1,
  ADXL3XX_INT1 = 0,
  ADXL3XX_INT2 = 1,
} adxl3xx_int_pin;


// Adafruit_ADXL343(int32_t sensorID);
// Adafruit_ADXL343(int32_t sensorID, TwoWire *wireBus);
// Adafruit_ADXL343(uint8_t clock, uint8_t miso, uint8_t mosi, uint8_t cs,
//                 int32_t sensorID = -1);
// Adafruit_ADXL343(uint8_t cs, SPIClass *theSPI, int32_t sensorID = -1);

bool adxl343_begin();
void adxl343_setRange(adxl34x_range_t range);
adxl34x_range_t adxl343_getRange(void);
void adxl343_setDataRate(adxl3xx_dataRate_t dataRate);
adxl3xx_dataRate_t adxl343_getDataRate(void);

uint8_t adxl343_getDeviceID(void);
bool adxl343_writeRegister(uint8_t data[], uint8_t size);
bool adxl343_readRegister(uint8_t reg, uint8_t data[], uint8_t size);
int16_t adxl343_read16(uint8_t reg);

//bool adxl343_enableInterrupts(int_config cfg);
//bool adxl343_mapInterrupts(int_config cfg);
//uint8_t adxl343_checkInterrupts(void);

void adxl343_getTrimOffsets(int8_t *x, int8_t *y, int8_t *z);
void adxl343_setTrimOffsets(int8_t x, int8_t y, int8_t z);

int16_t adxl343_getX(void);
int16_t adxl343_getY(void);
int16_t adxl343_getZ(void);
bool adxl343_getXYZ(int16_t *x, int16_t *y, int16_t *z);


#endif