/**
 * @file Memoria.h.
 * @author Ricardo Pabón Serna.(ricardo.pabon@correounivalle.edu.co)
 * @brief Manejo de la memoria EEPROM.
 * @version 0.1
 * @date 2023-05-14
 *
 * @copyright Copyright (c) 2023
 */

#include <Memoria.h>

//Inicialización de la longitud de la EEPROM
template<typename T, void (*func_ptr)(T&, classMemoria&, unsigned short&, T)>
const uint16_t Memoria<T, func_ptr>::length = E2END + 1;

//Inicialización de la EEPROM
template<typename T, void (*func_ptr)(T&, classMemoria&, unsigned short&, T)>
classMemoria Memoria<T, func_ptr>::EEPROM;

//Instancias de la plantilla Memoria creadas:
Memoria<int, Sumar<int>> NUM_ENVASES;
Memoria<unsigned long, Sumar<unsigned long>> FACTOR_CELDADCARGA;
Memoria<float, Sumar<float>> X2;
Memoria<float, Sumar<float>> X;
Memoria<float, Sumar<float>> A;

void Escribir_dato(uint8_t dato, unsigned int dirrecion)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE));
	/* Set up address and Data Registers */
	EEAR = dirrecion;
	EEDR = dato;
	/* Write logical one to EEMPE */
	EECR |= (1<<EEMPE);
	/* Start eeprom write by setting EEPE */
	EECR |= (1<<EEPE);
}


uint8_t Leer_dato(unsigned int adress)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE));
	/* Set up address register */
	EEAR = adress;
	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);
	/* Return data from Data Register */
	return EEDR;
}

Memoria<float, Array<float>> PESO(true);

void LIMPIAR()
{
     for(unsigned short i = 0; i < PESO.size(); i++) {(PESO.Ep()).write(i, 0xff); }
}

static unsigned short conservar = 0;

void size_Escrito(unsigned short& dato, unsigned short aumento)
{
    dato = aumento + conservar;
    conservar = dato;
}

unsigned short getConserva()
{
    return conservar;
}