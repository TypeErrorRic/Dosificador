#include <Celdad_Carga.h>
#include <Memoria.h>
#include <Config.h>
//Interrupciones del sistema:
#include <avr/interrupt.h>

// Parametros:
volatile double Celdad_Carga::data[SIZE_ARREGLO] = {};
volatile unsigned int Celdad_Carga::counter = 0;
bool Celdad_Carga::medicion = false;

// Incializar la celdad de carga:
HX711 Celdad_Carga::celdadCarga = HX711();

// Inicializar estructura de operación de datos:
static pvMecionParams Mediciones;

// Valores de configuración del registro.
#define MAX_VALUE 65525 // En realidad es 65530, pero así da por debajo de este Valor.
#define PRESCALER 1024
#define MIN_VALUE 157 // Valor minimo del contador OCR1A

// Valor maximo en milisegundo ingresable:
#define TIME_MAX ((unsigned int)((((float)PRESCALER / F_CPU) * MAX_VALUE) * 1000))

// Valores de calculos anteriores del sistema de celdad de carga:
static unsigned long past_timer;
static float auxmed;

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
        // Configurando el valor asignado a OCR1A
        time = time < TIME_MAX ? time : TIME_MAX;
        unsigned int aux = ((time * (F_CPU / PRESCALER)) / 1000);
        OCR1A = aux <= MIN_VALUE ? MIN_VALUE : aux;
        // ACTIVANDO MODO DE INTERRUPCIONES:
        sei();
        // Medición activada:
        medicion = true;
        // Esperar a que el
        celdadCarga.read();
        // Funciones para incializar el HX711:
        celdadCarga.set_offset(*FACTOR_CELDADCARGA); // Establece la tara.
        celdadCarga.set_scale(*SCALE);
        // Permite determinar si la escala va a hacer en kilogramos o gramos.
        Mediciones.flagCaptureMedicion = true;
        // Refrezcar medidas de los datos:
        for (short i = 0; i < APROX_PARA_VALOR_CELDAD_CARGA; i++)
        {
            Mediciones.flagCaptureMedicion = true;
            Captura_dato();
        }
        // Inicializar sistema:
        auxmed = getCeldadcargaValue();
        past_timer = millis();
        // Counter:
        counter = 3;
    }
}

// Realizar estructura:
pvResultsMedicion Medidas;

// Función de captura y realización de derivada del sistema:
void Captura_dato()
{
    if (Mediciones.flagCaptureMedicion)
    {
        if (Mediciones.doDerivada)
        {
            // Dervidad:
            Medidas.resultDev = (auxmed - getCeldadcargaValue()) / (millis() - past_timer);
            past_timer = millis();
            auxmed = getCeldadcargaValue();
            Mediciones.doDerivada = false;
        }
        else
        {
            // Tomar el valor del HX711:
            Celdad_Carga::data[Celdad_Carga::counter % SIZE_ARREGLO] = Celdad_Carga::celdadCarga.get_units(5);
            // Prender o apagar led de Confirmación de toma de datos.
            Mediciones.flagCaptureMedicion = false;
        }
    }
}

// Interrupcción:
ISR(TIMER1_COMPA_vect)
{
    // Determinar si se toma medición de un dato:
    Mediciones.flagCaptureMedicion = true;
    Celdad_Carga::counter++;
    // Determinar si se hace la derivada:
    if ((Celdad_Carga::counter % 10) == 0)
        Mediciones.doDerivada = true;
    // Reiniciar el contador.
    TCNT1 = 0;
    digitalWrite(13, !digitalRead(13));
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

        // Reiniciar arreglo de valores:
        for (short i = 0; i < SIZE_ARREGLO; i++)
        {
            Celdad_Carga::data[i] = 0;
        }
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
        escribirLcd<String>("Calibrando", 0, 0);
        escribirLcd<String>("Celdad...", 1, 0);
        // Tiempo de espera para retirar peso.
        delay(4000);
        // Funciones para determinar la calibración:
        Celdad_Carga::celdadCarga.set_scale(1000);
        Celdad_Carga::celdadCarga.begin(DOUT, CLOCK);
        Celdad_Carga::celdadCarga.tare(20);
        escribirLcd<String>("Coloque un peso", 0, 0, true);
        escribirLcd<String>("500 g", 1, 0);
        // Tiempo de espera para retirar peso.
        delay(4000);
        escribirLcd<String>("Realizando ", 0, 0, true);
        escribirLcd<String>("Medicion.", 1, 0);
        delay(2000);
        // Toma de valores para la calibración.
        cli();
        for (int i = 0; i < 100; i++)
        {
            // Tomar valor de la Escala:
            SCALE += Celdad_Carga::celdadCarga.get_value();
        }
        // Cargar valores de la memoria.
        ++(FACTOR_CELDADCARGA = Celdad_Carga::celdadCarga.get_offset());
        SCALE = (*SCALE / 100);
        ++(SCALE = *SCALE / PESO_COMPROBACION);
        sei();
        // Finalización de calibración.
        escribirLcd<String>("Calibrado.", 0, 0, true);
        delay(2000);
        // Inicializar captura de datos.
        HX_711.configTime(num);
        HX_711.begin();
        // Led de verificación de datos.
        pinMode(13, OUTPUT);
    }
    if (!Celdad_Carga::medicion)
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

// Devolver el valor del HX711 de las 3 ultimas mediciones:
float getCeldadcargaValue()
{
    double aux = 0;
    // Obtener el valor medio de las mediciones de la celdad de carga:
    if (Celdad_Carga::medicion)
    {
        // Deshabilitar interupciones:
        cli();
        for (short i = 0; i < APROX_PARA_VALOR_CELDAD_CARGA; i++)
            aux += Celdad_Carga::data[(((Celdad_Carga::counter - APROX_PARA_VALOR_CELDAD_CARGA) + i) % SIZE_ARREGLO)];
        // Habilitar interupciones:
        sei();
        return aux / APROX_PARA_VALOR_CELDAD_CARGA;
    }
    return 0;
}

#define TIME_COMPROBACION_GLOBAL ((unsigned int)TIME_COMPROBACION * SIZE_ARREGLO)

static float getaValueFull()
{
    float result = 0;
    cli();
    for (short i = 0; i < SIZE_ARREGLO; i++)
    {
        result += Celdad_Carga::data[i];
    }
    sei();
    return result;
}

/**
 * @brief Confirma si hay un envase o no en el sitio de envasado.
 * @param num Tiempo en milisegundos.
 * @param concurrente Permite realizar tareas mientras se hace la confirmación del envase.
 * @param stop Para la comprobacion en tiempo concurrente.
 */
bool confirmarEnvase()
{
    unsigned long T = millis();
    // Inicializar la celdad de carga.
    if (!Celdad_Carga::medicion)
    {
        // Inicializar toma de datos.
        initCeldad((TIME_COMPROBACION));
        // Tiempo de toma de datos:
        while ((millis() - T) < (TIME_COMPROBACION_GLOBAL + 10))
        {
            Mediciones.flagCaptureMedicion = true;
            Captura_dato();
        }
        // Detener toma de datos
        stopMediciones();
    }
    // Verificar si hay un envase o no:
    if (getaValueFull() >= CRITERIO_ENVASE)
        return true;
    else
        return false;
    // Reiniciar arreglo de valores:
    for (short i = 0; i < SIZE_ARREGLO; i++)
    {
        Celdad_Carga::data[i] = 0;
    }
    return false;
}

static short definirEnvase(float valor)
{
    // Comprobar si es Primer Envase:
    if ((valor > (ENVASE_1_PESO - VALOR_DESFASE)) && (valor < (ENVASE_1_PESO + VALOR_DESFASE)))
        return 1;
    // Comprobar si es sengundo Envase
    else if ((valor > (ENVASE_2_PESO - VALOR_DESFASE)) && (valor < (ENVASE_2_PESO + VALOR_DESFASE)))
        return 2;
    // Comprobar si es Tercer Envase
    else if ((valor > (ENVASE_3_PESO - VALOR_DESFASE)) && (valor < (ENVASE_3_PESO + VALOR_DESFASE)))
        return 3;
    // Envase no reconocido:
    else
        return 0;
}

bool reconocerEnvase(short &tipo)
{
    unsigned long T = millis();
    // Inicializar la celdad de carga.
    if (!Celdad_Carga::medicion)
    {
        // Inicializar toma de datos.
        initCeldad((TIME_COMPROBACION));
        // Tiempo de toma de datos:
        while ((millis() - T) < (TIME_COMPROBACION_GLOBAL + 10))
        {
            Mediciones.flagCaptureMedicion = true;
            Captura_dato();
        }
        // Detener toma de datos
        stopMediciones();
    }
    switch (definirEnvase(getaValueFull()))
    {
    case 0:
        // No es ningun envase retorna false:
        // Reiniciar arreglo de valores:
        for (short i = 0; i < SIZE_ARREGLO; i++)
        {
            Celdad_Carga::data[i] = 0;
        }
        return false;
        break;
    default:
        // Verificar que tipo de envase es:
        tipo = definirEnvase(getaValueFull());
        break;
    }
    // Reiniciar arreglo de valores:
    for (short i = 0; i < SIZE_ARREGLO; i++)
    {
        Celdad_Carga::data[i] = 0;
    }
    return true;
}