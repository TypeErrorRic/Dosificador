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


/**
 * @brief Función que se encarga de configurar los parametros inciales de las funciones.
 */
void setup()
{
  // Inicializar trasmición:
  Serial.begin(VELOCIDA_TX);
  setLCD();
  //Configuracion Base del Sistema:
  //Inicialización de pines:
  pinMode(PORTCONMUT, INPUT); // Pin del conmutador.
  pinMode(PIN_ENVASADO, INPUT); //Pin del sistema de envasado.
  pinMode(PIN_SENSOR_TOLVA, INPUT); //Pin del sistema de envasado.
  pinMode(PIN_CICLO_LLENADO, INPUT); //Pin del sistema de ciclo de envasado.
  //Inicialización de funciones
  RevisionSensoresInit(); // Revisar sensores.
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
    while (1)
      flujo_ejecucion_programa(stateTolva, fillTolva, offTolva, revisarEnvase, 
        revisarLLenado, llenandoEnvase, stopLllenadoEnvase, alarma);
    break;
  case 1:
    escribirLcd<String>("Realizando", 0, 0, true);
    escribirLcd<String>("Reg Cuadratica.", 1, 0);
    doRegresionCuadratica();
    break;
  case 2:
    EjecucionMemoria();
    break;
  case 3:
    doDerivada();
    break;
  default:
    break;
  }
}

/**
 * @note Implemenar visualización de derivada y Regresión.
*/

/**
 * @attention Cualquier inquietud informar al propietario del código.
 */