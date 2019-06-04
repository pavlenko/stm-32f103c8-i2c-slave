#ifndef __I2C_H
#define __I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "main.h"

extern I2C_HandleTypeDef i2c2;

void MX_I2C2_Init();

#ifdef __cplusplus
}
#endif

#endif //__I2C_H
