#include "main.h"

#include "i2c.h"
#include "rcc.h"

#include <stm32f1xx_hal.h>

int main() {
    HAL_Init();

    MX_RCC_Init();
    MX_I2C2_Init();

    while (true) {
        __NOP();
    }
}