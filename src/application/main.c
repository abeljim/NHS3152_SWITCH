#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "ndeft2t/ndeft2t.h"
#include "SEGGER_RTT.h"
#include "adxl343.h"

/* ------------------------------------------------------------------------- */

#define LOCALE "en" /**< Language used when creating TEXT records. */
#define MIME "nhs31xx/example.ndef" /**< Mime type used when creating MIME records. */

#define ADC_OFF 0
#define ADC_MAX  4095

/** The URL will be used in a single-record NDEF message. */
#define MAX_URI_PAYLOAD (254 - NDEFT2T_MSG_OVERHEAD(true, NDEFT2T_URI_RECORD_OVERHEAD(true)))

/**
 * The text and the MIME data are always presented together, in a dual-record NDEF message.
 * Payload length is split evenly between TEXT and MIME.
 */
#define MAX_TEXT_PAYLOAD (254 - (NDEFT2T_MSG_OVERHEAD(true, \
        NDEFT2T_TEXT_RECORD_OVERHEAD(true, sizeof(LOCALE) - 1) + \
        NDEFT2T_MIME_RECORD_OVERHEAD(true, sizeof(MIME) - 1)) / 2))
static uint8_t sText[MAX_TEXT_PAYLOAD] = "0 Hello World";

/** @copydoc MAX_TEXT_PAYLOAD */
#define MAX_MIME_PAYLOAD MAX_TEXT_PAYLOAD
static uint8_t sBytes[MAX_MIME_PAYLOAD] = {0, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE};

/* ------------------------------------------------------------------------- */

/**
 * Used to determine which NDEF message must be generated.
 * - @c true: generate a single-record NDEF message containing a URL.
 * - @c false: generate a dual-record NDEF message containing a TEXT and a MIME record.
 */

static volatile bool sButtonPressed = false; /** @c true when the WAKEUP button is pressed on the Demo PCB */
static volatile bool sMsgAvailable = false; /** @c true when a new NDEF message has been written by the tag reader. */
static volatile bool sFieldPresent = true; /** @c true when an NFC field is detected and the tag is selected. */

static void GenerateNdef_TextMime(void);
static void ParseNdef(void);
void initDAC(void);
void setDAC(int value);

/* ------------------------------------------------------------------------- */

/**
 * Handler for PIO0_0 / WAKEUP pin.
 * Overrides the WEAK function in the startup module.
 */
void PIO0_IRQHandler(void)
{
    Chip_GPIO_ClearInts(NSS_GPIO, 0, 1);
    sButtonPressed = true; /* Handled in main loop */
}

/**
 * Called under interrupt.
 * @see NDEFT2T_FIELD_STATUS_CB
 * @see pNdeft2t_FieldStatus_Cb_t
 */
void App_FieldStatusCb(bool status)
{
    if (status) {
        LED_On(LED_RED);
    }
    else {
        LED_Off(LED_RED);
    }
    sFieldPresent = status; /* Handled in main loop */
}

/**
 * Called under interrupt.
 * @see NDEFT2T_MSG_AVAILABLE_CB
 * @see pNdeft2t_MsgAvailable_Cb_t
 */
void App_MsgAvailableCb(void)
{
    sMsgAvailable = true; /* Handled in main loop */
}

/* ------------------------------------------------------------------------- */

/** Generates a dual-record NDEF message containing a TEXT and a MIME record, and copies it to the NFC shared memory. */
static void GenerateNdef_TextMime(void)
{
    uint8_t instance[NDEFT2T_INSTANCE_SIZE];
    uint8_t buffer[NFC_SHARED_MEM_BYTE_SIZE ];
    NDEFT2T_CREATE_RECORD_INFO_T textRecordInfo = {.pString = (uint8_t *)"en" /* language code */,
                                                   .shortRecord = true,
                                                   .uriCode = 0 /* don't care */};
    NDEFT2T_CREATE_RECORD_INFO_T mimeRecordInfo = {.pString = (uint8_t *)MIME /* mime type */,
                                                   .shortRecord = true,
                                                   .uriCode = 0 /* don't care */};
    NDEFT2T_CreateMessage(instance, buffer, NFC_SHARED_MEM_BYTE_SIZE, true);
    if (NDEFT2T_CreateTextRecord(instance, &textRecordInfo)) {
        if (NDEFT2T_WriteRecordPayload(instance, sText, sizeof(sText) - 1 /* exclude NUL char */)) {
            NDEFT2T_CommitRecord(instance);
        }
    }
    if (NDEFT2T_CreateMimeRecord(instance, &mimeRecordInfo)) {
        if (NDEFT2T_WriteRecordPayload(instance, sBytes, sizeof(sBytes))) {
            NDEFT2T_CommitRecord(instance);
        }
    }
    NDEFT2T_CommitMessage(instance); /* Copies the generated message to NFC shared memory. */
}

/** Parses the NDEF message in the NFC shared memory, and copies the TEXT and MIME payloads. */
static void ParseNdef(void)
{
    uint8_t instance[NDEFT2T_INSTANCE_SIZE];
    uint8_t buffer[NFC_SHARED_MEM_BYTE_SIZE];
    NDEFT2T_PARSE_RECORD_INFO_T recordInfo;
    int len = 0;
    uint8_t *pData = NULL;

    if (NDEFT2T_GetMessage(instance, buffer, NFC_SHARED_MEM_BYTE_SIZE)) {
        while (NDEFT2T_GetNextRecord(instance, &recordInfo) != false) {
            pData = (uint8_t *)NDEFT2T_GetRecordPayload(instance, &len);
            switch (recordInfo.type) {
                case NDEFT2T_RECORD_TYPE_TEXT:
                    if ((size_t)len <= MAX_TEXT_PAYLOAD) {
                        memcpy(sText, pData, (size_t)len);
                        memset(sText + len, 0, MAX_TEXT_PAYLOAD - (size_t)len);
                    }
                    break;
                case NDEFT2T_RECORD_TYPE_MIME:
                    if ((size_t)len <= MAX_MIME_PAYLOAD) {
                        memcpy(sBytes, pData, (size_t)len);
                        memset(sBytes + len, 0, MAX_MIME_PAYLOAD - (size_t)len);
                    }
                    break;
                default:
                    /* ignore */
                    break;
            }
        }
    }

    if(sText[0] == '0') {
        setDAC(ADC_OFF);
    }
    else if (sText[0] == '1') {
        setDAC(ADC_MAX);
    }
}

void initDAC(void) {
    Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_ANA0_0, IOCON_FUNC_1);
    Chip_ADCDAC_Init(NSS_ADCDAC0);
    Chip_ADCDAC_SetMuxDAC(NSS_ADCDAC0, ADCDAC_IO_ANA0_0);
    Chip_ADCDAC_SetModeDAC(NSS_ADCDAC0, ADCDAC_CONTINUOUS);
}

void setDAC(int value) {
    Chip_ADCDAC_WriteOutputDAC(NSS_ADCDAC0, value);
}

/* ------------------------------------------------------------------------- */

/** Called under interrupt. */
void I2C0_IRQHandler(void)
{
    if (Chip_I2C_IsMasterActive(I2C0)) {
        Chip_I2C_MasterStateHandler(I2C0);
    }
    else {
        Chip_I2C_SlaveStateHandler(I2C0);
    }
}

static void Master_Init(void)
{
    if (SYSTEMCLOCK == NSS_SFRO_FREQUENCY) {
        Chip_Flash_SetHighPowerMode(true);
    }
    Chip_Clock_System_SetClockFreq(SYSTEMCLOCK);
    Board_Init();

    Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_4, IOCON_FUNC_1 | IOCON_I2CMODE_STDFAST);
    Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_5, IOCON_FUNC_1 | IOCON_I2CMODE_STDFAST);

    Chip_SysCon_Peripheral_DeassertReset(SYSCON_PERIPHERAL_RESET_I2C0);

    Chip_I2C_Init(I2C0);
    Chip_I2C_SetClockRate(I2C0, I2C_BITRATE);

    /* Finish initialization for master I2C communication. */
    Chip_I2C_SetMasterEventHandler(I2C0, Chip_I2C_EventHandler);
    NVIC_EnableIRQ(I2C0_IRQn);

    /* Extra initialization required for master-build functionality:
     * - prepare NDEF message creation
     * - Use pin 3 for i2c pull-up - assuming R3 and R4 are stuffed.
     */
    NDEFT2T_Init();
    Chip_IOCON_SetPinConfig(NSS_IOCON, IOCON_PIO0_3, IOCON_FUNC_0);
    Chip_GPIO_SetPinDIROutput(NSS_GPIO, 0, 3);
    Chip_GPIO_SetPinOutHigh(NSS_GPIO, 0, 3);
}

uint8_t scanI2C()
{
    Master_Init();

    Chip_Clock_System_BusyWait_ms(1000); // Might not be need: to stop bricking

    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);

    //SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n\r\n");
    //SEGGER_RTT_WriteString(0, "###### Testing SEGGER_printf() ######\r\n");

    uint16_t offset = 0;
    uint8_t text[I2C_SLAVE_TX_SIZE + 1];
    // I2C_STATUS_T i2c_status = 0;
    // I2C_XFER_T i2cPacket;
    I2C_XFER_T i2cPacket = {.slaveAddr = 0,
            .txBuff = (uint8_t *)&offset,
            .txSz = I2C_MASTER_TX_SIZE,
            .rxBuff = text,
            .rxSz = I2C_SLAVE_TX_SIZE};
    while(1)
    {   SEGGER_RTT_WriteString(0, "Scanning i2c Address\r\n");
        for(uint8_t i = 0; i < 128; i++)
        {
            i2cPacket.slaveAddr = i;            
            I2C_STATUS_T i2c_status = Chip_I2C_MasterTransfer(I2C0, &i2cPacket);

            
            if(i2c_status == 0)
            {
                SEGGER_RTT_printf(0, "Address: %d: ", i);
                SEGGER_RTT_printf(0, "AWK\n", i);
            }

            Chip_Clock_System_BusyWait_ms(10);
        }

        SEGGER_RTT_WriteString(0, "Scanning Complete\r\n");
        Chip_Clock_System_BusyWait_ms(2000);
    }


    return 0;    
}

int main(void)
{
    Master_Init();

    Chip_Clock_System_BusyWait_ms(1000); // Might not be need: to stop bricking

    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);

    SEGGER_RTT_printf(0, "Program Started\n");

    if(!adxl343_begin())
    {
        SEGGER_RTT_printf(0, "Failed ADXL343 INIT\n");
        return 0;
    }

    SEGGER_RTT_printf(0, "SUCCESS ADXL343 INIT\n");

    while(1)
    {   
        int16_t x, y, z;
        if(adxl343_getXYZ(&x, &y, &z))
        {
            SEGGER_RTT_printf(0, "ACCEL: %d, %d, %d\n", x, y, z);
        }
        else {
            SEGGER_RTT_printf(0, "Bad DATA\n");
        }
        Chip_Clock_System_BusyWait_ms(100);
    }


    return 0;
}

/*
int main(void)
{
    Board_Init();

    Chip_Clock_System_BusyWait_ms(1000); // Might not be need: to stop bricking

    while (1) {
        LED_Toggle(LED_RED);
        Chip_Clock_System_BusyWait_ms(250);
    }

    NDEFT2T_Init();
    NVIC_EnableIRQ(PIO0_IRQn); // PIO0_IRQHandler is called when this interrupt fires. 
    Chip_GPIO_EnableInt(NSS_GPIO, 0, 1);
    initDAC();

    Chip_Clock_System_BusyWait_ms(1000);




    for (;;) {
        if (sFieldPresent) { // Update the NDEF message once when there is an NFC field 

            GenerateNdef_TextMime();
            // Update the payloads for the next message.
            // sText[0] = (uint8_t)((sText[0] == '9') ? '0' : (sText[0] + 1));
            // sBytes[0]++;
        
        }
        while (sFieldPresent) {
            if (sMsgAvailable) {
                sMsgAvailable = false;
                ParseNdef();
            }
            Chip_Clock_System_BusyWait_ms(10);
        }
    }

    return 0;
}*/