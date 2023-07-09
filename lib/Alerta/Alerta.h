#ifndef ALERTAS_H
#define ALERTAS_H

#include <Config.h>

#define PIN_ALERTA                7
#define DELAY_TIME_VISUALIZACION  1000UL
#define DELAY_TIME_SONIDO         1000UL

// PARAMETROS PARA LA CONFIGURACIÓN DEL SONIDO:
// Divisores de sonido:
#define DIVISOR_S1            4
#define DIVISOR_S2_RECONOCE   5
#define DIVISOR_S2_FALLA      4
#define DIVISOR_S3            4
#define DIVISOR_S4            4
#define DIVISOR_S6            2

class Alerta
{
private:
  const short bozzer;

public:
  Alerta(short pin)
      : bozzer(pin)                         { pinMode(bozzer, OUTPUT); }
  void S0();                                // mensaje de estado s1, esperando accionamineto conmutador.
  void S1();                                // mensaje de alerta de envase en la salida.
  void S2();                                // alerta identificacion del tipo de envase.
  void S3();                                // alerta de nivel de tolva.
  void S4();                                // alerta de alimentador encendido.
  void S5();                                // Alerta de llenado de recipiente.
  void S6();                                // Alerta de llenado terminado.
  void Desactivar();                        //Desactivar Alarmas.
};

extern Alerta Alertas;

//Mostrar mensajes sobre el ciclo de envasado:
#define botonCancelar       8
#define botonArriba         9
#define botonSeleccionar    10
#define botonAbajo          11
#define interruptorMaestro  12

void setupPantalla();
void MostrarPantalla(bool isEnvasadoEnable);
void setupInteruptMassage();

#endif