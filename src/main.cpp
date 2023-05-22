/**
 * @file Proyecto de Taller de Ingeniería.
 * @author Ricardo Pabón Serna.(ricardo.pabon@correounivalle.edu.co)
 * @brief Este programa se encarga de describir el flujo de ejecución del programa
 * @version 0.1
 * @date 2022-10-17
 *
 * @copyright Copyright (c) 2022
 */
#include <Build.h>
#include <Memoria.h>
/**
 * @brief Función que se encarga de configurar los parametros inciales de las funciones.
 */
void setup()
{
  // Inicializar trasmición:
  Serial.begin(VELOCIDA_TX);
  setLCD();
  //Configuracion Base del Sistema:
  pinMode(PORTCONMUT, INPUT); // Pin del conmutador.
  RevisionSensoresInit(); // Revisar sensores.
  pinMode(LED_CONMUTADOR, OUTPUT); //Pin verificación LCD.
  initRegresionCuadratica();
  initAlarma();
  //Mensaje de finalización de configuración.
  Serial.println("Listo");
}

/**
 * @brief Bucle incial de ejecución.
 */
void loop()
{
  switch (Modo_Configuracion())
  {
  case 0:
    Serial.println("Flujo del diagrama");
    while (1)
      flujo_ejecucion_programa(stateTolva, fillTolva, offTolva, revisarEnvase, 
        revisarLLenado, llenandoEnvase, stopLllenadoEnvase, alarma);
    break;
  case 1:
    Serial.println("Usar Regresion.");
    break;
  case 2:
    EjecucionMemoria();
    break;
  case 3:
    Serial.println("Usar Derivada.");
    break;
  default:
    break;
  }
}

/**
 * @note Para la comprobación de los calculos realizador por el programa <<Regresion_Cuadratica.h>
 *       Debe descomentar lqas funcionalidades de test y Get_Matriz.
 * @attention Cualquier inquietud informar al propietario del código.
 */