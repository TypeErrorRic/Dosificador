//Librerías:
#include <Arduino.h>
#include <Regresion_Cuadratica.h> 
#include <Operaciones.h>
//Funciones de testeo:
#include "..\include\Testeo.h"

/**
 * @file Proyecto de Taller de Ingeniería.
 * @author Ricardo Pabón Serna.(ricardo.pabon@correounivalle.edu.co)
 * @brief Este programa se encarga de realizar el ajuste de un grupo de pares ordenados en una
 *        una curva cuadratica, devolviendo los parametros de la curva para su correspondiente análisis.
 * @version 0.1
 * @date 2022-10-17 
 *
 * @copyright Copyright (c) 2022
 */


//Ambito de las declaraciones de variables:
namespace Variables
{
    constexpr int VELOCIDAD_DATOS{9600};
    const int Nfilas = 5; // Precisión de la medida tomada.
    struct Valores_Sensores //Valores Para el calculo de la matriz.
    {
      float x[Nfilas]{}; // Medidas de la masa con respecto al eje X.
      float y[Nfilas]{}; // Medidas de la masa con respecto al eje Y.
    };
};

//Estucutra que almacena los valores de los sensores.
Variables::Valores_Sensores Medidas{};
//Insanciación de un obecto de la clase::<Regresion_Cuadratica.h>
Regresion_Cuadratica Matriz(Variables::Nfilas, Medidas.x, Medidas.y);

/**
 * @brief Función que se encarga de configurar los parametros inciales de las funciones.
 */
void setup() 
{
  Serial.begin(Variables::VELOCIDAD_DATOS); //Inicializando el Puerto serial con la velocidad de trasmición de datos.
  for(int i=0;i<Variables::Nfilas;i++)
  {
    Medidas.x[i] = 0;
    Medidas.y[i] = 0;
  }
  Matriz.reset();
  pinMode(LED_BUILTIN, OUTPUT); //Configuración del led.
  Serial.println("Setup conluido correctamente.");
}


/**
 * @brief Bucle incial de ejecución.
 */
void loop() 
{
  int contador_num{0};
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Calculando....");
  //Bucle principal de toma de datos:
  do
  {
    delay(1000);
    Testeo::Serial_events(Medidas.x[contador_num], Medidas.y[contador_num], Matriz.get_Realizar());
    contador_num++;
    // Si se ha realizado la toma de valores con un patron incapaz de ser acomodados en una curva cuadratica. Lazara error.
    if (Matriz.get_Realizar())
    {
      Serial.print("Coordenada #: ");  
      Serial.print(contador_num);
      Serial.println(" Tomada correctamente.");
    }
  }
  while (Matriz.Update(Medidas.x[contador_num-1], Medidas.y[contador_num-1]));
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("..................");
  Serial.println("Datos Ingresados:");
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  //Si se ha realizado la toma de valores con un patron incapaz de ser acomodados en una curva cuadratica. Lazara error.
  if(Matriz.get_Realizar())
  {
    Matriz.get_Matriz();
    Matriz.Calcular();
  }
  else Serial.print("Reiniciar.");
  delay(1000);
}

/**
 * @note Para la comprobación de los calculos realizador por el programa <<Regresion_Cuadratica.h>
 *       Debe descomentar lqas funcionalidades de test y Get_Matriz.
 * @attention Cualquier inquietud informar al propietario del código.
 */