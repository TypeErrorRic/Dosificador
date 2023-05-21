#include <Config.h>
#include <Memoria.h>

/********************* IMPLEMENTACIÓN DE IMPRECIÓN EN LCD **************************/

LiquidCrystal lcd(7,6,5,4,3,2);

void setLCD()
{
    lcd.begin(16,2);
    lcd.setCursor(0, 0);
    lcd.print("Ready");
}
static void escribirEstado(short estado)
{
  lcd.setCursor(0,0);
  lcd.print("Estado:");
  lcd.print(estado);
}

LiquidCrystal &getLcd()
{
  return lcd;
}

/********************* IMPLEMENTACIÓN DE MODO OPERACIÓN **************************/
static short Modo_operacion = 0;
static unsigned long timer = 0;
static short count = 0;

//Función de inicialización de los modos de operación del sistema:
static bool Modo_seleccion()
{
    unsigned int time = millis();
    short count = 0;
    short aux = 1;
    while (millis() - time <= 5000UL)
    {
        if (CONMUTADOR == aux)
        {
            aux = 0;
            count++;
        }
        if (count >= 6)
        {
          timer = millis();
          return true;
        }
    }
    return false;
}

//Función de inicialización de los modos de operación del sistema:
short &Modo_Configuracion()
{
    if (Modo_seleccion())
    {
      switch (count)
      {
      case 0:
        if((millis() - timer) > 1000)
        {
          escribirLcd<String>("Modo de", 0, 0, true);
          escribirLcd<String>("Operacion.", 1,0);
          timer = millis();
          count++;
        }
        break;
      case 1:
        if((millis() - timer) > 1000)
        {
          escribirLcd<String>("1.Done", 0, 0, true);
          escribirLcd<String>("2.Regresión", 1, 0);
          timer = millis();
          count++;
        }
        break;
      case 2:
        if((millis() - timer) > 1000)
        {
          escribirLcd<String>("3. Memoria.", 0, 0, true);
          timer = millis();
          count++;
        }
        break;
      default:
        count = 0;
        break;
      }
      while ((Serial.available() > 0) && (Modo_operacion == 0))
      {
        switch ((char)Serial.read())
        {
        case '1':
          Modo_operacion = 0;
          break;
        case '2':
          Modo_operacion = 1;
          break;
        case '3':
          Modo_operacion = 2;
          break;
        default:
          break;
        }
      }
    }
    else
        Modo_operacion = 0;
    return Modo_operacion;
}

/*********** IMPLEMENTACIÓN DE FUNCIONES PARA EL MANEJO DEL FLUJO DEL PROGRAMA ****************/

static short Estado = 0; // Número de estados.
static short tipo = 0;   // Tipo de Envase.

// Revision de las diferentes entradas del sistema.
void RevisionSensoresInit()
{
    // Revisar cada entrada:
    *REGENTRADAS = 0x00;
}

// Revision de los sensores de cada sistema:
void Revision_variables(bool(*revisarTolva)(void), void(*llenarTolva)(void), 
                      bool(*revisarEnvase)(short &), bool(*llenado)(void), void(*alerta)(short type, bool state))
{
  switch (Estado)
  {
  case 0:
    alerta(4, false);
    break;
  case 1: // Revisa si hay un envase en la zona de envasado.
    if(revisarEnvase(tipo)) {MRECONOCIMIENTO_ENVASE(1); MCICLO_LLENADO(0);}
    else MRECONOCIMIENTO_ENVASE(0) {;}
    break;
  case 2: // Revisa si se ha quitado el envase de la zona de envasado.
    if(!revisarEnvase(tipo)) {MRECONOCIMIENTO_ENVASE(0); MCICLO_LLENADO(0);}
    alerta(1, true);
    break;
  case 3: // Revisa si la tolva dosificadora tiene suficiente material para realizar un llenado.
    if(revisarTolva()) {MSENSORTOLVA(1);}
    else MSENSORTOLVA(0) {;}
    break;
  case 4: // Revisa si la tolva dosificadora ya se ha llenado.
    llenarTolva();
    break;
  case 5: // Revisa si se ha colocado un envase en la zona de envasado.
    if(revisarEnvase(tipo)) 
    {
      MRECONOCIMIENTO_ENVASE(1);
      MTIPO_ENVASE(tipo);
      if(llenado()) {MCICLO_LLENADO(1);}
    }
    break;
  case 6: // Revisa si el envase no reconocido se ha quitado de la zona de envasado.
    if(!revisarEnvase(tipo)) {MRECONOCIMIENTO_ENVASE(0)};
    alerta(2, true);
    break;
  case 7: // Revisa si el envase se ha llenado correctamente.
    if(!revisarEnvase(tipo)) {MRECONOCIMIENTO_ENVASE(0)};
    if(llenado()) {MCICLO_LLENADO(1);}
    break;
  case 8: // Revisa si se ha quitado el envase reconocido pero con material del ciclo de envasado.
    if(!revisarEnvase(tipo)) {MRECONOCIMIENTO_ENVASE(0); MCICLO_LLENADO(0);}
    alerta(3, true);
  default:
    break;
  }
}

//Función para comprobar el estado del conmutador:
static void stateConmutador()
{
  if (CONMUTADOR) digitalWrite(LED_CONMUTADOR, HIGH);
  else digitalWrite(LED_CONMUTADOR, LOW);
}

// Flujo de Ejcución del sistema.
void flujo_ejecucion_programa(bool(*revisarTolva)(void), void(*llenarTolva)(void), void(*ApagarTolva)(void), 
                              bool(*revisarEnvase)(short &), bool(*llenado)(void), void(*doLlenado)(void),
                              float (*stopLlenado)(void), void(*alerta)(short type, bool state))
{
  //Revision de todos los sensores:
  Revision_variables(revisarTolva, llenarTolva, revisarEnvase, llenado, alerta);
  stateConmutador(); //Revisa el estado del Conmutador.
  //Flujo del programa:
  switch (Estado)
  {
  case 0: // Encender Conmutador
    if (CONMUTADOR) Estado = 1;
    else Estado = 0;
    break;
  case 1: // ¿Hay un envase?
    if (CONMUTADOR)
    {
      if (RECONOCIMIENTO_ENVASE) Estado = 2;
      else Estado = 3;
    }
    else Estado = 0;
    break;
  case 2: // Alerta de Envase.
    if (CONMUTADOR)
    {
      if (RECONOCIMIENTO_ENVASE) Estado = 2;
      else {Estado = 3; alerta(1, false);}
    }
    else Estado = 0;
    break;
  case 3: // ¿Cuál es el estado de la tolva?
    if (revisarTolva()) {Estado = 5; if(!SENSAR_TOLVA) {MSENSORTOLVA(1);}}
    else Estado = 4;
    break;
  case 4: // Encender Alimentador
    while (revisarTolva()) {stateConmutador();}// Revisar sensor de la tolva por polling.
    MSENSORTOLVA(1);// Se ha llenado.
    ApagarTolva(); // Apagar_Dispensador.
    Estado = 5;
  case 5: // Reconocimiento de envase
    if (CONMUTADOR)
    {
      if (RECONOCIMIENTO_ENVASE)
      {
        if (ENVASE_CORRECTO)
        {
          if (CICLO_LLENADO) Estado = 8;
          else 
          {
            doLlenado();
            Estado = 7;
          }
        }
        else Estado = 6;
      }
      else Estado = 5;
    }
    else Estado = 0;
    break;
  case 6: // Alerta de no reconocimiento del envase.
    if (CONMUTADOR)
    {
      if (RECONOCIMIENTO_ENVASE) Estado = 6; // Revisar si el envase se quito. Polling
      else {Estado = 5; alerta(2, false);};
    }
    else Estado = 0;
    break;
  case 7: // Llenado
    if (!CICLO_LLENADO)
    {
      // Ya se lleno.
      if (RECONOCIMIENTO_ENVASE) Estado = 7;
      else
      {
        if (CONMUTADOR) Estado = 3;
        else Estado = 0;
      }
    }
    else
    {
      NUM_CICLO_FINAL += 1;
      PESO[*NUM_CICLO_FINAL] = stopLlenado();
      MSENSORTOLVA(0);
      if (CONMUTADOR) Estado = 1;
      else Estado = 0;
    }
    break;
  case 8: // Alerta de reconocimiento de envase ya dosificado.
    if (CONMUTADOR)
    {
      if (RECONOCIMIENTO_ENVASE) Estado = 8;
      else {Estado = 5; alerta(3, false);};
    }
    else Estado = 0;
    break;
  default:
    break;
  }
  escribirEstado(Estado);
  //Tiempo de ejecución:
  delay(DELAY_EJE);
}

/*********** IMPLEMENTACIÓN DE FUNCIÓN PARA OBTENER EL ESTADO DEL SISTEMA ****************/

const short &getEstado()
{
  return Estado;
}