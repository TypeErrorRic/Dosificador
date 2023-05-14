/**
 * @file Memoria.h.
 * @author Ricardo Pabón Serna.(ricardo.pabon@correounivalle.edu.co)
 * @brief Manejo de la memoria EEPROM.
 * @version 0.1
 * @date 2023-05-14
 *
 * @copyright Copyright (c) 2023
 */

#ifndef REGISTRO_FLUJO
#define REGISTRO_FLUJO

#include <Arduino.h>

/***************** MANEJO DE BITS SOBRE LOS ESTADOS DE ENTRADA *********************/

#define PORTCONMUT      7 //Pin reservado exclusivamente para el conmutador.
#define CONMUTADOR      ((*((volatile uint8_t*)_SFR_MEM_ADDR(PIND)) & (1 << PORTCONMUT)) >> PORTCONMUT)

/*************************** REGISTRO DE ENTRADA ***********************************/
typedef struct
{
    unsigned char bit0:1; //Conmutador
    unsigned char bit1:1; //Sensor de nivel de la tolva.
    unsigned char bit2:1; //Reconocimiento Si hay un envase o No.
    unsigned char bit3:1; //El envase corresponde a uno conocido.
    unsigned char bit4:1; //Tipo de envase.
    unsigned char bit5:1; //Tipo de envase.
    unsigned char bit6:1; //Ciclo de llenado completado
    unsigned char bit7:1; //N/A
}regEntrada;

#define REGENTRADAS             ((volatile unsigned char*)(RAMEND + 31)) 

//Lectura de los valores del reg entrada:
#define SENSAR_TOLVA            (((volatile regEntrada*)REGENTRADAS)->bit1)
#define RECONOCIMIENTO_ENVASE   (((volatile regEntrada*)REGENTRADAS)->bit2)
#define ENVASE_CORRECTO         (((volatile regEntrada*)REGENTRADAS)->bit3)
#define CICLO_LLENADO           (((volatile regEntrada*)REGENTRADAS)->bit6)
#define ENVASE_1                (((volatile regEntrada*)REGENTRADAS)->bit4)
#define ENVASE_2                (((volatile regEntrada*)REGENTRADAS)->bit5)


//Escritura de los valores del reg entrada:
#define SETTERBIT(bit)          (*REGENTRADAS |= (1 << (bit)))
#define LIMPIARBIT(bit)         (*REGENTRADAS &= ~(1 << (bit)))

//Modificar el bit de Sensor tolva:
#define MSENSORTOLVA(value) \
    if ((value) == 0) {LIMPIARBIT(1);} \
    else if ((value) == 1) {SETTERBIT(1);}

//Modificar el bit del Reconocimiento del envase:
#define MRECONOCIMIENTO_ENVASE(value) \
    if ((value) == 0) {LIMPIARBIT(2); } \
    else if ((value) == 1) {SETTERBIT(2);}

//Modificar el bit de Envase correcto:
#define MENVASE_CORRECTO(value) \
    if ((value) == 0) {LIMPIARBIT(3); } \
    else if ((value) == 1) {SETTERBIT(3);}

//Modificicar el bit de ciclo de llenado:
#define MCICLO_LLENADO(value) \
    if ((value) == 0) {LIMPIARBIT(6);} \
    else if ((value) == 1) {SETTERBIT(6);}

/*********** Modificar los bits de llenado **********/
//Modificar el bit msb:
#define MODIFICAR_1(value) \
    if ((value) == 0) {LIMPIARBIT(4);} \
    else if ((value) == 1) {SETTERBIT(4);}

//Modificar el bit lsb:
#define MODIFICAR_0(value) \
    if ((value) == 0) {LIMPIARBIT(5);} \
    else if ((value) == 1) {SETTERBIT(5);}

//Función para identificar el tipo de envase:
inline int tipoEnvase()
{
    int tipo = 0;
    if(ENVASE_1)
    {
        if(ENVASE_2)
            tipo = 3;
        else
            tipo = 2;
    }
    else
    {
        if(ENVASE_2)
            tipo = 1;
        else
            tipo = 0;
    }
    return tipo;
}

#define TIPO_ENVASE tipoEnvase() //Tipo de envase:

//Permite obtener una copia del registro.
inline volatile unsigned char getRegEntrada() {return *REGENTRADAS;} 

#define MTIPO_ENVASE(type) do { \
    switch (type) { \
        case 0: \
            MODIFICAR_1(0); \
            MODIFICAR_0(0); \
            break; \
        case 1: \
            MODIFICAR_1(0); \
            MODIFICAR_0(1); \
            break; \
        case 2: \
            MODIFICAR_1(1); \
            MODIFICAR_0(0); \
            break; \
        case 3: \
            MODIFICAR_1(1); \
            MODIFICAR_0(1); \
            break; \
        default: \
            break; \
    } \
} while (0)


#endif