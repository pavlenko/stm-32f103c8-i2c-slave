#include "main.h"
#include "rcc.h"

#include <stm32f1xx_hal.h>

int main() {
    HAL_Init();

    MX_RCC_Init();

    while (true) {
        __NOP();
    }
}