#ifndef CELDAD_CARGA_H
#define CELDAD_CARGA_H

// Librerias para la celdad de carga.
#include <HX711.h>

#define DOUT A1  // Puerto de Trasmición de datos.
#define CLOCK A0 // Puerto para establecer la señal de reloj.

#ifndef F_CPU
#define F_CPU 16000000ul
#endif

#define APROX_PARA_VALOR_CELDAD_CARGA 3 //Número de datos tomados por la celdad de carga.

#define CRITERIO_ENVASE 10 //Criterio minimo para identificar un envase.

#define PESO_COMPROBACION 1

// Clase para acceder a las funciones de HX711 por medio de Asociación.
class Celdad_Carga
{
private:
    unsigned int time; //Tiempo entre toma de datos.

public:
    static bool medicion; //Permite definir si se está realizando la medición
    static HX711 celdadCarga; //Clase con las operaciones de obtención de pesos.
    static volatile unsigned int counter; //Contador de iteraciones
    static volatile double data[APROX_PARA_VALOR_CELDAD_CARGA]; //Valores de la regresión cuadratica.
    Celdad_Carga() { celdadCarga.begin(DOUT, CLOCK); } // Inicializar HX711.
    void stop(); //Detiene la toma de datos.
    void begin(); //Inicia la toma de datos.
    void configTime(unsigned int num); //Configura el tiempo entre toma de datos
};

// Funciones para cargar los valores de la celdad de carga en la heap.
void initCeldad(unsigned int num);
// Detiene las mediciones.
void stopMediciones();
//Obtener el valor de la celdad de carga.
float getCeldadcargaValue();

#define TIME_COMPROBACION       100 //Tiempo entre comprobaciones para permitir saber si hay un envase

//Confirma si hay un envase o no en el sitio de envasado.
bool confirmarEnvase(unsigned int num = TIME_COMPROBACION);

#endif