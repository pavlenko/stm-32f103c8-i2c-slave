#ifndef __I2C_H
#define __I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "main.h"

#define I2C2_OWN_ADDRESS_1 (0x30U)
#define I2C2_OWN_ADDRESS_2 (0x00U)

extern I2C_HandleTypeDef i2c2;

void MX_I2C2_Init();

void restartComm(void);

#ifdef __cplusplus
}
#endif

#endif //__I2C_H
