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

/////////////////////////LIBRERIA CON ARDUINO//////////////////////////////
#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Librerias creadas:
#include <Regresion_Cuadratica.h> //Desarrollado Por Ricardo Pabón Serna.
#include "../../include/Registro_flujo.h" //Desarrollado por Ricardo Pabón Serna.
#include <Memoria.h> //Desarrollado por Ricardo Pabón Serna.
#include <derivada.h> //Desarollado por Ricardo Pabón Serna.

//CONFIGURACIÓN DEL ARDUINO:
#define VELOCIDA_TX     9600 //Velocidad Baudrate de trasmición.
#define NFILAS          5 //Número de datos tomados.

/*********************FUNCIONES DE IMPRECIÓN EN LCD **************************/
//Inicializar LCD
void setLCD();
//Escribir en la LCD:
void ecribirLcd(int estado);

/************ FUNCIONES PARA EL CALCULO DE LA REGRESION CUADRATICA **************/

void initRegresionCuadratica();
//void initRegistroMemoria();
void doRegresionCuadratica();

/**
 * @note Para la comprobación de los calculos realizador por el programa <<Regresion_Cuadratica.h>
 *       Debe descomentar lqas funcionalidades de test y Get_Matriz.
 */

/**************** FUNCIONES PARA EL MANEJO DEl FLUJO DEL PROGRAMA *********************/

void RevisionSensoresInit();
void Revision_variables();
/**
 * @note Las funciones para el control del flujo de programa está desarrolladas en el
 *       Paradigma de programación orientada a registros.
 */

#endif
