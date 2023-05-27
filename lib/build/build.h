/**
 * @file Proyecto de Taller de Ingeniería.
 * @author Ricardo Pabón Serna.(ricardo.pabon@correounivalle.edu.co)
 * @brief Este programa se encarga de describir el flujo de ejecución del programa
 * @version 1.0
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2022
 */

#ifndef BUILD_H
#define BUILD_H

// Librerias creadas:
#include <Config.h> //Desarrollado por Ricardo Pabón Serna.
#include <Regresion_Cuadratica.h> //Desarrollado Por Ricardo Pabón Serna.
#include <Derivada.h> //Desarollado por Ricardo Pabón Serna.

#define TIEMPO_REVISION     500 //El tiempo en que se va a efectuar la revisión de los sensor.

/************ FUNCIONES PARA EL CALCULO DE LA REGRESION CUADRATICA **************/

void initRegresionCuadratica();
void doRegresionCuadratica();

/**
 * @note Para la comprobación de los calculos realizador por el programa <<Regresion_Cuadratica.h>
 *       Debe descomentar lqas funcionalidades de test y Get_Matriz.
 */

/***************** FUNCIONES PARA EL CALCULO DE LA DERIVADA *********************/

void doDerivada(); //Realizar la derivada.

/***************** FUNCIONES PARA EL MANEJO DEL DISPENSADOR *********************/

#define PIN_SENSOR_TOLVA    9 //Pin de identificación de sensr tolva.

bool stateTolva(); //Devuelve el estado de la Tolva dispensadora. True llena / False Vacia.
void fillTolva(); //Activa la función de llenado de la Tolva dispensadora.
void offTolva(); //Dectiene la Función de llenado de la Tolva dispensadora.

/********************* FUNCIONES DEl SISTEMA DE ENVASADO ************************/

#define PIN_ENVASADO 	10 //Pin de identificación de llenado de envase.

bool revisarEnvase(short &Tipo); //Función que permite conocer el tipo de envase. True si hay un envase / False si no hay.
void llenandoEnvase(); //Activa el llenado del envase.

#define PIN_CICLO_LLENADO      11 //Pin del sensor de revisión de llenado.

bool revisarLLenado();  //Verfica el llenado del envase.
float stopLllenadoEnvase(); //Detiene el llenado del envase.

/********************* FUNCIONES PARA SISTEMA DE ALERTA ************************/

#define ALARMA          12 //Pin de la Alarma.

void initAlarma(); //Inizialiar el sistema de Alarma
void alarma(short type, bool state); //Activa la alarma correspondiente al evento que suceda.

#endif
