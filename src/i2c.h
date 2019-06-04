#ifndef __I2C_H
#define __I2C_H

#include <stm32f1xx_hal_i2c.h>

#ifdef __cplusplus
extern "C" {
#endif

extern I2C_HandleTypeDef i2c2;

void MX_I2C2_Init();

#ifdef __cplusplus
}
#endif

#endif //__I2C_H
