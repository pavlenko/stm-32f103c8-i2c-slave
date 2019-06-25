#ifndef __I2C_H
#define __I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "main.h"

#define I2C2_OWN_ADDRESS_1 (0x3E)
#define I2C2_OWN_ADDRESS_2 (0xFF)

extern I2C_HandleTypeDef i2c2;

void (*MX_I2C2_slaveRXCallback) (uint8_t *data, uint8_t size);
void (*MX_I2C2_slaveTXCallback) (void);

void MX_I2C2_Init();

void MX_I2C_attachSlaveRXEvent(I2C_HandleTypeDef *i2c, void (*callback) (uint8_t *data, uint8_t size));

void MX_I2C_attachSlaveTXEvent(I2C_HandleTypeDef *i2c, void (*callback) (void));

#ifdef __cplusplus
}
#endif

#endif //__I2C_H
