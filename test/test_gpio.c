#ifdef TEST

#include "unity.h"

#include "gpio.h"

enum{PA,PB,PC,NUM_PUERTOS};
enum{
    PA0,PA7,PA8,PA15,PB5,PC13,
    NUM_PINES
};
typedef struct RPuerto{
    GPIO_TypeDef registros[1];
    uint32_t accionesSet;
    uint32_t accionesReset;
}RPuerto;
typedef struct Prueba{
    RPuerto puertos[NUM_PUERTOS];
    Pin *pines[NUM_PINES];
}Prueba;

static Prueba prueba;

#define RPuerto_RESET ((RPuerto){.registros={{.CRL=UINT32_C(0x44444444),.CRH=UINT32_C(0x44444444)}}})
void setUp(void)
{
    prueba = (Prueba){};
    for (unsigned i=0;i<NUM_PUERTOS;++i){
        prueba.puertos[i] = RPuerto_RESET;
    }
    prueba.pines[PA0 ] = Pin_new(prueba.puertos[PA].registros,0 );
    prueba.pines[PA7 ] = Pin_new(prueba.puertos[PA].registros,7 );
    prueba.pines[PA8 ] = Pin_new(prueba.puertos[PA].registros,8 );
    prueba.pines[PA15] = Pin_new(prueba.puertos[PA].registros,15 );
    prueba.pines[PB5 ] = Pin_new(prueba.puertos[PB].registros,5 );
    prueba.pines[PC13] = Pin_new(prueba.puertos[PC].registros,13);
}

static void procesaSetReset(void)
{
    for(unsigned i=0;i<NUM_PUERTOS;++i){
        GPIO_TypeDef *const registros = prueba.puertos[i].registros;
        const uint32_t accionesSet = registros->BSRR & ((1<<16)-1); 
        const uint32_t accionesReset = (registros->BRR | (registros->BSRR >> 16)) & ((1<<16)-1);
        TEST_ASSERT_EQUAL_UINT32(0,accionesReset&accionesSet);
        registros->ODR &= ~accionesReset;
        registros->ODR |= accionesSet;
        prueba.puertos[i].accionesReset = accionesReset; 
        prueba.puertos[i].accionesSet = accionesSet;
        registros->BRR = 0; 
        registros->BSRR = 0; 
    }
}

void tearDown(void)
{
    for (unsigned i=0;i<NUM_PINES;++i){
        struct Pin *const pin = prueba.pines[i];
        if(pin) Pin_delete(pin);
    }
}

void test_pinEntrada(void)
{
    TEST_ASSERT_NOT_NULL(prueba.pines[PA0]);
    Pin_configura(prueba.pines[PA0],Pin_ENTRADA);
    TEST_ASSERT_NOT_NULL(prueba.pines[PA7]);
    Pin_configura(prueba.pines[PA7],Pin_ENTRADA_PULL_DOWN);
    TEST_ASSERT_NOT_NULL(prueba.pines[PA8]);
    Pin_configura(prueba.pines[PA8],Pin_ENTRADA_PULL_UP);
    TEST_ASSERT_NOT_NULL(prueba.pines[PC13]);
    Pin_configura(prueba.pines[PC13],Pin_ENTRADA_ANALOGICA);
    procesaSetReset();
    TEST_ASSERT_EQUAL_HEX32(0x84444444,prueba.puertos[PA].registros->CRL);
    TEST_ASSERT_EQUAL_HEX32(0x44444448,prueba.puertos[PA].registros->CRH);
    TEST_ASSERT_EQUAL_HEX32(0x44044444,prueba.puertos[PC].registros->CRH);
    TEST_ASSERT_EQUAL_HEX32(1<<7      ,prueba.puertos[PA].accionesReset);
    TEST_ASSERT_EQUAL_HEX32(1<<8      ,prueba.puertos[PA].accionesSet);

    TEST_ASSERT_EQUAL_HEX32(0x44444444,prueba.puertos[PB].registros->CRL);
    TEST_ASSERT_EQUAL_HEX32(0x44444444,prueba.puertos[PB].registros->CRH);
    TEST_ASSERT_EQUAL_HEX32(0x44444444,prueba.puertos[PC].registros->CRL);
    TEST_ASSERT_EQUAL_HEX32(0         ,prueba.puertos[PB].accionesReset);
    TEST_ASSERT_EQUAL_HEX32(0         ,prueba.puertos[PB].accionesSet);
    TEST_ASSERT_EQUAL_HEX32(0         ,prueba.puertos[PC].accionesReset);
    TEST_ASSERT_EQUAL_HEX32(0         ,prueba.puertos[PC].accionesSet);

}

void test_pinSalida(void)
{
    Pin_configura(prueba.pines[PA0], Pin_SALIDA_LENTA);
    Pin_configura(prueba.pines[PA7], Pin_SALIDA_MEDIA);
    Pin_configura(prueba.pines[PA8], Pin_SALIDA_RAPIDA);
    TEST_ASSERT_NOT_NULL(prueba.pines[PA15]);
    Pin_configura(prueba.pines[PA15],Pin_SALIDA_LENTA  | Pin_DRENADOR_ABIERTO);
    Pin_configura(prueba.pines[PB5], Pin_SALIDA_RAPIDA | Pin_FUNCION_ALTERNATIVA);
    Pin_configura(prueba.pines[PC13],Pin_SALIDA_MEDIA  | Pin_DRENADOR_ABIERTO | Pin_FUNCION_ALTERNATIVA);
    procesaSetReset();
    TEST_ASSERT_EQUAL_HEX32(0x14444442,prueba.puertos[PA].registros->CRL);
    TEST_ASSERT_EQUAL_HEX32(0x64444443,prueba.puertos[PA].registros->CRH);
    TEST_ASSERT_EQUAL_HEX32(0x44B44444,prueba.puertos[PB].registros->CRL);
    TEST_ASSERT_EQUAL_HEX32(0x44D44444,prueba.puertos[PC].registros->CRH);
    
    TEST_ASSERT_EQUAL_HEX32(0x44444444,prueba.puertos[PB].registros->CRH);
    TEST_ASSERT_EQUAL_HEX32(0x44444444,prueba.puertos[PC].registros->CRL);
    TEST_ASSERT_EQUAL_UINT32(0        ,prueba.puertos[PA].accionesReset);
    TEST_ASSERT_EQUAL_UINT32(0        ,prueba.puertos[PA].accionesSet);
    TEST_ASSERT_EQUAL_UINT32(0        ,prueba.puertos[PB].accionesReset);
    TEST_ASSERT_EQUAL_UINT32(0        ,prueba.puertos[PB].accionesSet);
    TEST_ASSERT_EQUAL_UINT32(0        ,prueba.puertos[PC].accionesReset);
    TEST_ASSERT_EQUAL_UINT32(0        ,prueba.puertos[PC].accionesSet);
}

void test_pinLee(void)
{
    Pin_configura(prueba.pines[PA0], Pin_ENTRADA);
    TEST_ASSERT_FALSE(Pin_lee(prueba.pines[PA0]));
    prueba.puertos[PA].registros->IDR = 1;
    TEST_ASSERT_TRUE(Pin_lee(prueba.pines[PA0]));
}

void test_pinEscribe(void)
{
    Pin_configura(prueba.pines[PA15], Pin_SALIDA_LENTA);
    Pin_configura(prueba.pines[PA0], Pin_SALIDA_LENTA);
    Pin_ponAlto(prueba.pines[PA15]);
    Pin_ponBajo(prueba.pines[PA0]);
    procesaSetReset();
    TEST_ASSERT_EQUAL_UINT32(1    , prueba.puertos[PA].accionesReset);
    TEST_ASSERT_EQUAL_UINT32(1<<15, prueba.puertos[PA].accionesSet);
    TEST_ASSERT_EQUAL_UINT32(0    , prueba.puertos[PB].accionesReset);
    TEST_ASSERT_EQUAL_UINT32(0    , prueba.puertos[PB].accionesSet);
    TEST_ASSERT_EQUAL_UINT32(0    , prueba.puertos[PC].accionesReset);
    TEST_ASSERT_EQUAL_UINT32(0    , prueba.puertos[PC].accionesSet);
}

void test_pinObtEstadoSalida(void)
{
    Pin_configura(prueba.pines[PA15], Pin_SALIDA_LENTA);
    TEST_ASSERT_FALSE(Pin_obtEstadoSalida(prueba.pines[PA15]));
    Pin_ponAlto(prueba.pines[PA15]);
    procesaSetReset();
    TEST_ASSERT_TRUE(Pin_obtEstadoSalida(prueba.pines[PA15]));
}

void test_pinConmutaEstado(void)
{
    Pin_configura(prueba.pines[PC13], Pin_SALIDA_LENTA);
    Pin_conmuta(prueba.pines[PC13]);
    procesaSetReset();

    TEST_ASSERT_EQUAL_UINT32(0    , prueba.puertos[PA].accionesReset);
    TEST_ASSERT_EQUAL_UINT32(0    , prueba.puertos[PA].accionesSet);
    TEST_ASSERT_EQUAL_UINT32(0    , prueba.puertos[PB].accionesReset);
    TEST_ASSERT_EQUAL_UINT32(0    , prueba.puertos[PB].accionesSet);
    TEST_ASSERT_EQUAL_UINT32(0    , prueba.puertos[PC].accionesReset);
    TEST_ASSERT_EQUAL_UINT32(1<<13, prueba.puertos[PC].accionesSet);

    Pin_conmuta(prueba.pines[PC13]);
    procesaSetReset();

    TEST_ASSERT_EQUAL_UINT32(0    , prueba.puertos[PA].accionesReset);
    TEST_ASSERT_EQUAL_UINT32(0    , prueba.puertos[PA].accionesSet);
    TEST_ASSERT_EQUAL_UINT32(0    , prueba.puertos[PB].accionesReset);
    TEST_ASSERT_EQUAL_UINT32(0    , prueba.puertos[PB].accionesSet);
    TEST_ASSERT_EQUAL_UINT32(1<<13, prueba.puertos[PC].accionesReset);
    TEST_ASSERT_EQUAL_UINT32(0    , prueba.puertos[PC].accionesSet);
}


#endif // TEST
