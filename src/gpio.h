#ifndef GPIO_H
#define GPIO_H

#ifndef TEST
#include "stm32f1xx.h"
#else
#include <stdint.h>
typedef struct GPIO_TypeDef{
    volatile uint32_t CRL;
    volatile uint32_t CRH;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t BRR;
    volatile uint32_t LCKR;
}GPIO_TypeDef;
#endif

typedef struct Pin Pin;
typedef enum ModoPin{
    Pin_ENTRADA,
    Pin_ENTRADA_PULL_DOWN,
    Pin_ENTRADA_PULL_UP,
    Pin_ENTRADA_ANALOGICA
} ModoPin;

Pin *Pin_new(GPIO_TypeDef *puerto,uint8_t numero);
void Pin_delete(Pin *self);
void Pin_configura(Pin *self,ModoPin p);
#endif // GPIO_H
