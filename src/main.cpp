/**
 * @file Proyecto de Taller de Ingeniería.
 * @author Ricardo Pabón Serna.(ricardo.pabon@correounivalle.edu.co)
 * @brief Este programa se encarga de describir el flujo de ejecución del programa
 * @version 0.1
 * @date 2022-10-17
 *
 * @copyright Copyright (c) 2022
 */

#include <build.h>

int Estado = 0;

/**
 * @brief Función que se encarga de configurar los parametros inciales de las funciones.
 */
void setup()
{
  pinMode(PORTCONMUT, INPUT); // Pin del conmutador.
  pinMode(8, OUTPUT);
  initRegresionCuadratica();
  // Inicializar trasmición:
  Serial.begin(VELOCIDA_TX);
  setLCD();
  // Revisar sensores:
  RevisionSensoresInit();
  Serial.println("Listo");
}

/**
 * @brief Bucle incial de ejecución.
 */
void loop()
{
  if (CONMUTADOR)
    digitalWrite(8, HIGH);
  else
    digitalWrite(8, LOW);
  Revision_variables();
  switch (Estado)
  {
  case 0: // Revisa si el conmutador está encendido.
    if (CONMUTADOR)
      Estado = 1;
    else
      Estado = 0;
    break;
  case 1: // Revisa si hay un envase en la zona de envasado.
    if (CONMUTADOR)
    {
      if (RECONOCIMIENTO_ENVASE)
        Estado = 2;
      else
        Estado = 3;
    }
    else
      Estado = 0;
    break;
  case 2: // Revisa si se ha quitado el envase de la zona de envasado.
    if (CONMUTADOR)
    {
      if (RECONOCIMIENTO_ENVASE)
      {
        // Revisar Envase polling.
        Estado = 2;
      }
      else
        Estado = 3;
    }
    else
      Estado = 0;
    break;
  case 3: // Revisa si la tolva dosificadora tiene suficiente material para realizar un llenado.
    if (SENSAR_TOLVA)
      Estado = 5;
    else
      Estado = 4;
    break;
  case 4: // Revisa si la tolva dosificadora ya se ha llenado.
    Serial.println("LLenando...");
    while (!SENSAR_TOLVA)
    {
      Revision_variables();
      delay(1000);
      // Revisar sensor de la tolva por polling.
    }
    Estado = 5;
    break;
  case 5: // Revisa si se ha colocado un envase en la zona de envasado.
    if (CONMUTADOR)
    {
      if (RECONOCIMIENTO_ENVASE)
      {
        if (ENVASE_CORRECTO)
        {
          if (CICLO_LLENADO)
            Estado = 8;
          else
          {
            Estado = 7;
          }
        }
        else
        {
          Estado = 6;
        }
      }
      else
      {
        Estado = 5;
      }
    }
    else
    {
      Estado = 0;
    }
    break;
  case 6: // Revisa si el envase no reconocido se ha quitado de la zona de envasado.
    if (CONMUTADOR)
    {
      if (RECONOCIMIENTO_ENVASE)
      {
        // Revisar si el envase se quito. Polling
        Estado = 6;
      }
      else
        Estado = 5;
    }
    else
      Estado = 0;
    break;
  case 7: // Revisa si el envase se ha llenado correctamente.
    if (!CICLO_LLENADO)
    {
      // Ya se lleno.
      if (RECONOCIMIENTO_ENVASE)
      {
        Estado = 7;
        Serial.print("Tipo: ");
        Serial.println(TIPO_ENVASE);
      }
      else
      {
        if (CONMUTADOR)
          Estado = 3;
        else
          Estado = 0;
      }
    }
    else
    {
      NUM_CICLO_FINAL += 1;
      if (CONMUTADOR)
        Estado = 1;
      else
        Estado = 0;
    }
    break;
  case 8: // Revisa si se ha quitado el envase reconocido pero con material del ciclo de envasado.
    if (CONMUTADOR)
    {
      if (RECONOCIMIENTO_ENVASE)
        Estado = 8;
      else
        Estado = 5;
    }
    break;
  default:
    break;
  }
  ecribirLcd(Estado);
  //Tiempo de ejecución:
  delay(500);
}
/**
 * @note Para la comprobación de los calculos realizador por el programa <<Regresion_Cuadratica.h>
 *       Debe descomentar lqas funcionalidades de test y Get_Matriz.
 * @attention Cualquier inquietud informar al propietario del código.
 */