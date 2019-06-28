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

extern I2C_HandleTypeDef i2c1;
extern I2C_HandleTypeDef i2c2;

void MX_I2C1_Init();

void MX_I2C2_Init();

void MX_I2C2_Dispatch();

#ifdef __cplusplus
}
#endif

#endif //__I2C_H
