#include <Celdad_Carga.h>
#include <Memoria.h>
#include <Config.h>
#include <avr/interrupt.h>

volatile double Celdad_Carga::data[APROX_PARA_VALOR_CELDAD_CARGA] = {};
volatile unsigned int Celdad_Carga::counter = 0;
bool Celdad_Carga::medicion = false;

HX711 Celdad_Carga::celdadCarga = HX711();

// Valores de configuración del registro.
#define MAX_VALUE 65525 //En realidad es 65530, pero así da por debajo de este Valor.
#define PRESCALER 1024
#define MIN_VALUE 157 //Valor minimo del contador OCR1A

//Valor maximo en milisegundo ingresable:
#define TIME_MAX    ((unsigned int)((((float) PRESCALER / F_CPU) * MAX_VALUE)*1000))

// Inicializar las mediciones:
void Celdad_Carga::begin()
{
    if (!medicion)
    {
        // Deshabilitar las interupciones por un momento.
        cli();
        // Reiniciar registros.
        TCCR1A = 0x00;
        TCCR1B = 0x00;
        // Configurar registros.
        TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10); // PRESCALER 1024.
        TIMSK1 |= (1 << OCIE1A);
        //Configurando el valor asignado a OCR1A
        time = time < TIME_MAX ? time: TIME_MAX;                     
        unsigned int aux = ((time * (F_CPU / PRESCALER)) / 1000);
        OCR1A = aux <= MIN_VALUE ? MIN_VALUE : aux;
        // ACTIVANDO MODO DE INTERRUPCIONES:
        sei();
        // Medición activada:
        medicion = true;
        // Funciones para incializar el HX711:
        celdadCarga.set_offset(*FACTOR_CELDADCARGA); // Establece la tara.
        celdadCarga.set_scale(*SCALE);               // Permite determinar si la escala va a hacer en kilogramos o gramos.
    }
}

// Interrupcción:
ISR(TIMER1_COMPA_vect)
{
    // Tomar el valor del HX711:
    Celdad_Carga::data[Celdad_Carga::counter % APROX_PARA_VALOR_CELDAD_CARGA] = Celdad_Carga::celdadCarga.get_value();
    Celdad_Carga::counter++;
    // Prender o apagar led de Confirmación de toma de datos.
    digitalWrite(13, !digitalRead(13));
    // Reiniciar el contador.
    TCNT1 = 0;
}

// Deteiene el funcionamiento de las mediciones.
void Celdad_Carga::stop()
{
    if (medicion)
    {
        // Deshabilitar el timer 1.
        cli();
        TIMSK1 &= ~(1 << OCIE1A);
        TCCR1A = 0x00;
        TCCR1B = 0x00;
        TCNT1 = 0;
        sei();

        // Volver contador a 0.
        Celdad_Carga::counter = 0;
        medicion = false;
    }
}

void Celdad_Carga::configTime(unsigned int num)
{
    this->time = num;
}

/*******************FUNCIONES PARA ACCESO A LOS DATOS******************/

// Instancia de celdad de carga para realizar mediciones.
static Celdad_Carga HX_711;

/**
 * @brief Inicializar las mediciones.
 * @param num Tiempo en milisegundos.
 */
void initCeldad(unsigned int num)
{
    if (*FACTOR_CELDADCARGA == 0)
    {
        // Calibración de la Celdad de Carga:
        escribirLcd<String>("Calibrando", 0, 0, true);
        escribirLcd<String>("Celdad...", 0, 1, false);
        delay(2000); // Tiempo de espera para retirar peso.
        // Funciones para determinar la calibración:
        Celdad_Carga::celdadCarga.set_scale();
        Celdad_Carga::celdadCarga.begin(DOUT, CLOCK);
        Celdad_Carga::celdadCarga.tare(20);
        // Toma de valores para la calibración.
        for (int i = 0; i < 100; i++)
        {
            SCALE += Celdad_Carga::celdadCarga.get_value();
        }
        // Cargar valores de la memoria.
        ++(FACTOR_CELDADCARGA = Celdad_Carga::celdadCarga.get_offset());
        ++(SCALE = *SCALE / 100);
    }
    if (!HX_711.medicion)
    {
        // Inicializar captura de datos.
        HX_711.configTime(num);
        HX_711.begin();
        // Led de verificación de datos.
        pinMode(13, OUTPUT);
    }
}

// Detener las mediciones.
void stopMediciones()
{
    HX_711.stop();
}

// Devolver el valor del HX711.
float getCeldadcargaValue()
{
    double aux = 0;
    for (short i = 0; i < APROX_PARA_VALOR_CELDAD_CARGA; i++)
        aux += Celdad_Carga::data[i];
    return aux / APROX_PARA_VALOR_CELDAD_CARGA;
}

#define TIME_COMPROBACION_GLOBAL  ((unsigned int) TIME_COMPROBACION * APROX_PARA_VALOR_CELDAD_CARGA)

/**
 * @brief Confirma si hay un envase o no en el sitio de envasado.
 * @param num Tiempo en milisegundos.
 * @param concurrente Permite realizar tareas mientras se hace la confirmación del envase.
 * @param stop Para la comprobacion en tiempo concurrente.
 */

bool confirmarEnvase(unsigned int num)
{
    unsigned long T = millis();
    // Inicializar la celdad de carga.
    if (!Celdad_Carga::medicion)
    {
        if (num <= TIME_COMPROBACION)
        {
            // Inicializar toma de datos.
            initCeldad((TIME_COMPROBACION));
            // Tiempo de toma de datos:
            while ((millis() - T) < (TIME_COMPROBACION_GLOBAL + 10));
            // Detener toma de datos
            stopMediciones();
        }
        else if (num > TIME_COMPROBACION)
        {
            // Inicializar toma de datos.
            initCeldad(TIME_COMPROBACION);
            // Tomar valores de confirmación de peso nuevos.
            while ((millis() - T) < (TIME_COMPROBACION_GLOBAL + 10));
            // Detener toma de datos
            stopMediciones();
            //Iniciar de nuevo.
            initCeldad(num);
        }
        // Verificar si hay un envase o no:
        if (getCeldadcargaValue() >= CRITERIO_ENVASE)
            return true;
        else
            return false;
    }
    else
        return false;
}