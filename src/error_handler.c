#include "error_handler.h"

#include <stm32f1xx_hal.h>

void Error_Handler(char *file, int line) {
    while (1) {
        __NOP();
    }
}
