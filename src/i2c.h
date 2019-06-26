#ifndef __I2C_H
#define __I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "main.h"

// Real 7 bits slave address value is: b0011110 mean in uint8_t equivalent at 0x1E
#define I2C2_OWN_ADDRESS_1 0x3C
#define I2C2_OWN_ADDRESS_2 0xFF

#define I2C_RX_BUFFER_MAX 32
#define I2C_TX_BUFFER_MAX 32

typedef enum {
    I2C_SLAVE_MODE_TRANSMIT,
    I2C_SLAVE_MODE_RECEIVE,
    I2C_SLAVE_MODE_LISTEN,
} I2C_SlaveMode_t;

typedef struct {
    I2C_HandleTypeDef *handle;
    uint8_t rxBufferData[I2C_RX_BUFFER_MAX];
    uint8_t rxBufferSize;
    uint8_t txBufferData[I2C_TX_BUFFER_MAX];
    uint8_t txBufferSize;
    I2C_SlaveMode_t slaveMode;
    void (*slaveRXCallback) (uint8_t *data, uint8_t size);
    void (*slaveTXCallback) (void);
} I2C_API_t;

extern I2C_HandleTypeDef i2c1;
extern I2C_HandleTypeDef i2c2;
extern I2C_API_t i2c2_api;

void MX_I2C1_Init();

void MX_I2C2_Init();

void MX_I2C_attachSlaveRXEvent(I2C_API_t *i2c, void (*callback) (uint8_t *data, uint8_t size));

void MX_I2C_attachSlaveTXEvent(I2C_API_t *i2c, void (*callback) (void));

HAL_StatusTypeDef MX_I2C_slaveTransmit(I2C_API_t *i2c, uint8_t *data, uint8_t size);

#ifdef __cplusplus
}
#endif

#endif //__I2C_H
