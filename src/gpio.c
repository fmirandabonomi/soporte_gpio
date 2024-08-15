#include "gpio_impl.h"
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>


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

#define obtOpcionesSalida(p) ((p & 0xC0) >> 4)
#define escribeConfig(cr_,pos_,valor_) (*(cr_) = (*(cr_) & ~(0xF << (pos_))) | (((valor_) & 0xF) << (pos_)))
#define obtDescriptorConfig(p) (p & ~0xC0)

void Pin_configura(Pin *self,ModoPin p)
{
    assert(self);
    typeof(self->puerto->CRH) *const CR = (self->numero < 8) ? &self->puerto->CRL : &self->puerto->CRH; 
    const unsigned offset = ((unsigned)self->numero % 8U) * 4U;

    switch (obtDescriptorConfig(p))
    {
    case Pin_ENTRADA:
        escribeConfig(CR,offset,0x4);
    break;case Pin_ENTRADA_ANALOGICA:
        escribeConfig(CR,offset,0);
    break;case Pin_ENTRADA_PULL_UP:
        escribeConfig(CR,offset,0x8);
        self->puerto->BSRR = 1<<self->numero;
    break;case Pin_ENTRADA_PULL_DOWN:
        escribeConfig(CR,offset,0x8);
        self->puerto->BRR = 1<<self->numero;
    break;case Pin_SALIDA_LENTA:
        escribeConfig(CR,offset,0x2 | obtOpcionesSalida(p));
    break;case Pin_SALIDA_MEDIA:
        escribeConfig(CR,offset,0x1 | obtOpcionesSalida(p));
    break;case Pin_SALIDA_RAPIDA:
        escribeConfig(CR,offset,0x3 | obtOpcionesSalida(p));
    break;default:
    break;
    }
}

bool Pin_lee(Pin *self)
{
    return self->puerto->IDR & (1<<self->numero);
}

void Pin_ponAlto(Pin *self)
{
    self->puerto->BSRR = 1 << self->numero;
}
void Pin_ponBajo(Pin *self)
{
    self->puerto->BRR = 1 << self->numero;
}
bool Pin_obtEstadoSalida(Pin *self)
{
    return self->puerto->ODR & (1<<self->numero);
}

void Pin_conmuta(Pin *self)
{
    if(Pin_obtEstadoSalida(self)){
        Pin_ponBajo(self);
    }else{
        Pin_ponAlto(self);
    }
}
