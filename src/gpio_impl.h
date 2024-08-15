#ifndef GPIO_IMPL_H
#define GPIO_IMPL_H
#include "gpio.h"

struct Pin{
    GPIO_TypeDef *puerto;
    uint8_t numero;
};

#endif
