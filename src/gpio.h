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
#include <stdbool.h>

typedef struct Pin Pin;
typedef enum ModoPin{
    Pin_ENTRADA,
    Pin_ENTRADA_PULL_DOWN,
    Pin_ENTRADA_PULL_UP,
    Pin_ENTRADA_ANALOGICA,
    Pin_SALIDA_LENTA,
    Pin_SALIDA_MEDIA,
    Pin_SALIDA_RAPIDA,
    Pin_DRENADOR_ABIERTO = 0x40,
    Pin_FUNCION_ALTERNATIVA = 0x80
} ModoPin;

/**
 * @brief Crea un objeto pin para un pin dado dentro de un puerto dado
 * 
 * @param puerto El puerto
 * @param numero El pin
 * @return Pin* El objeto pin.
 */
Pin *Pin_new(GPIO_TypeDef *puerto,uint8_t numero);
/**
 * @brief Elimina un objeto pin creado por Pin_new
 * @note A partir de su eliminación el objeto pin no es válido
 * @param self El pin
 */
void Pin_delete(Pin *self);
/**
 * @brief Configura un pin.
 * 
 * @param self El pin
 * @param p La configuración. Pin_ENTRADA* o Pin_SALIDA*, las configuraciones de salida pueden ser
 * modificadas por las banderas Pin_DRENADOR_ABIERTO y/o Pin_FUNCION_ALTERNATIVA haciendo un or bit
 * a bit '|' con la opción Pin_SALIDA* correspondiente
 */
void Pin_configura(Pin *self,ModoPin p);

/**
 * @brief Lee el nivel de un pin capturado por su buffer de entrada
 * 
 * @param self El pin
 * @retval true El nivel del pin es ALTO
 * @retval false El nivel del pin es BAJO
 */
bool Pin_lee(Pin *self);

/**
 * @brief Establece el buffer de salida de un pin en estado BAJO
 * 
 * @param self El pin
 */
void Pin_ponAlto(Pin *self);
/**
 * @brief Establece el buffer de salida de un pin en estado ALTO
 * 
 * @param self El pin
 */
void Pin_ponBajo(Pin *self);
/**
 * @brief Obtiene el estado del buffer de salida de un pin
 * 
 * @param self El pin
 * @retval true El buffer de salida está configurado en ALTO
 * @retval false El buffer de salida está configurado en BAJO
 */
bool Pin_obtEstadoSalida(Pin *self);

/**
 * @brief Conmuta el estado del buffer de salida de un pin. BAJO -> ALTO, ALTO -> BAJO
 * 
 * @param self El pin
 */
void Pin_conmuta(Pin *self);
#endif // GPIO_H
