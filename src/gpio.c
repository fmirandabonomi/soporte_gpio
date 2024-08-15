#include "gpio.h"
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

struct Pin{
    GPIO_TypeDef *puerto;
    uint8_t numero;
};

Pin *Pin_new(GPIO_TypeDef *puerto,uint8_t numero)
{
    Pin *const pin = malloc(sizeof(*pin));
    if(pin){
        pin->numero = numero;
        pin->puerto = puerto;
    }
    return pin;
}
void Pin_delete(Pin *self)
{
    free(self);
}
void Pin_configura(Pin *self,ModoPin p)
{
    enum{MASCARA=0xF};
    assert(self);
    typeof(self->puerto->CRH) *const CR = (self->numero < 8) ? &self->puerto->CRL : &self->puerto->CRH; 
    const unsigned offset = ((unsigned)self->numero % 8U) * 4U;

    switch (p)
    {
    case Pin_ENTRADA:
        *CR = (*CR & ~(MASCARA<<offset)) | (0x4 << offset);
    break;case Pin_ENTRADA_ANALOGICA:
        *CR &= ~(MASCARA<<offset);
    break;case Pin_ENTRADA_PULL_UP:
        *CR = (*CR & ~(MASCARA<<offset)) | (0x8 << offset);
        self->puerto->BSRR = 1<<self->numero;
    break;case Pin_ENTRADA_PULL_DOWN:
        *CR = (*CR & ~(MASCARA<<offset)) | (0x8 << offset);
        self->puerto->BRR = 1<<self->numero;
    break;default:
    break;
    }
}
