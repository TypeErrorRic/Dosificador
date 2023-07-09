#include <Celdad_Carga.h>
#include <Memoria.h>
#include <Config.h>
// Interrupciones del sistema:
#include <avr/interrupt.h>

// Parametros:
volatile double Celdad_Carga::data[APROX_PARA_VALOR_CELDAD_CARGA] = {};
volatile unsigned int Celdad_Carga::counter = 0;
bool Celdad_Carga::medicion = false;
bool Celdad_Carga::configHx711 = true;

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
        if (configHx711)
        {
            // Esperar a que el
            celdadCarga.read();
            // Funciones para incializar el HX711:
            celdadCarga.set_offset(*FACTOR_CELDADCARGA); // Establece la tara.
            celdadCarga.set_scale(*SCALE);
            configHx711 = false;
        }
        // Medición activada:
        medicion = true;
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
        counter = 0;
        Medidas.changeMedicion = true;
        Medidas.medicionHx = 0;
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
            Celdad_Carga::data[Celdad_Carga::counter % APROX_PARA_VALOR_CELDAD_CARGA] = Celdad_Carga::celdadCarga.get_units(5);
            Celdad_Carga::counter++;
        }
        else
        {
            // Tomar el valor del HX711:
            Celdad_Carga::data[Celdad_Carga::counter % APROX_PARA_VALOR_CELDAD_CARGA] = Celdad_Carga::celdadCarga.get_units(5);
            Celdad_Carga::counter++;
            escribirLcd<int>(Celdad_Carga::counter, 0, 0, true);
            // Restalecer banderas:
            Mediciones.flagCaptureMedicion = false;
            Medidas.changeMedicion = true;
        }
    }
}

// Interrupcción:
ISR(TIMER1_COMPA_vect)
{
    // Determinar si se toma medición de un dato:
    Mediciones.flagCaptureMedicion = true;
    // Determinar si se hace la derivada:
    if ((Celdad_Carga::counter % 10) == 0)
        Mediciones.doDerivada = true;
    // Reiniciar el contador.
    TCNT1 = 0;
    // FUNCIONAMINETO DE LA INTERRUPCIÓN CORRECTO:
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
        for (short i = 0; i < APROX_PARA_VALOR_CELDAD_CARGA; i++)
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
    if (Celdad_Carga::medicion && Medidas.changeMedicion)
    {
        // Deshabilitar interupciones:
        cli();
        for (short i = 0; i < APROX_PARA_VALOR_CELDAD_CARGA; i++)
            aux += Celdad_Carga::data[i];
        // Habilitar interupciones:
        sei();
        Medidas.medicionHx = aux / APROX_PARA_VALOR_CELDAD_CARGA;
        Medidas.changeMedicion = false;
    }
    return Medidas.medicionHx;
}

#define TIME_COMPROBACION_GLOBAL ((unsigned int)TIME_COMPROBACION * SIZE_ARREGLO)

static float getaValueFull()
{
    float result = 0;
    //Obtener media con 10 datos:
    for (short i = 0; i < SIZE_ARREGLO; i++)
    {
        result += Celdad_Carga::celdadCarga.get_units(5);
    }
    return (result / SIZE_ARREGLO);
}

/**
 * @brief Confirma si hay un envase o no en el sitio de envasado.
 * @param num Tiempo en milisegundos.
 * @param concurrente Permite realizar tareas mientras se hace la confirmación del envase.
 * @param stop Para la comprobacion en tiempo concurrente.
 */
bool confirmarEnvase()
{
    float resultfinal = 0;
    // Inicializar la celdad de carga.
    if (!Celdad_Carga::medicion)
    {
        initCeldad(100);
        // Tiempo de toma de datos:
        while (Celdad_Carga::counter <= APROX_PARA_VALOR_CELDAD_CARGA)
        {
            // Capturando datos:
            Captura_dato();
        }
        float resultConfirm2 = getCeldadcargaValue();
        // Detener toma de datos
        stopMediciones();
        // Tiempo de confirmación
        delay(TIME_COMPROBACION);
        // Comprobación del dato Tomado:
        float resultConfirm1 = getaValueFull();
        // Obteniendo dato final:
        if (((resultConfirm1 - resultConfirm2) <= VALOR_DESFASE) && ((resultConfirm1 - resultConfirm2) >= (VALOR_DESFASE * (-1))))
            resultfinal = (resultConfirm1 + resultConfirm2) / 2;
        else
        {
            // Colocar el envase a último momento
            if ((resultConfirm1 > CRITERIO_ENVASE) && ((resultConfirm1 - resultConfirm2) > (VALOR_DESFASE + 20)))
                resultfinal = resultConfirm1;
            // No se coloco un envase.
        }
    }
    else
    {
        resultfinal = getCeldadcargaValue();
    }
    // Verificar si hay un envase o no:
    if (resultfinal >= CRITERIO_ENVASE)
        return true;
    else
        return false;
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
    float resultfinal = 0;
    // Inicializar la celdad de carga.
    if (!Celdad_Carga::medicion)
    {
        // Inicializar la celdad de carga.
        initCeldad(100);
        // Tiempo de toma de datos:
        while (Celdad_Carga::counter <= APROX_PARA_VALOR_CELDAD_CARGA)
        {
            // Capturando datos:
            Captura_dato();
        }
        float resultConfirm2 = getCeldadcargaValue();
        // Detener toma de datos
        stopMediciones();
        // Tiempo de confirmación
        delay(TIME_COMPROBACION);
        // Comprobación del dato Tomado:
        float resultConfirm1 = getaValueFull();
        // Obteniendo dato final:
        if (((resultConfirm1 - resultConfirm2) <= VALOR_DESFASE) && ((resultConfirm1 - resultConfirm2) >= (VALOR_DESFASE * (-1))))
            resultfinal = (resultConfirm1 + resultConfirm2) / 2;
        else
        {
            // Colocar el envase a último momento
            if ((resultConfirm1 > CRITERIO_ENVASE) && ((resultConfirm1 - resultConfirm2) > (VALOR_DESFASE + 20)))
                resultfinal = resultConfirm1;
            // No se coloco un envase.
        }
    }
    // Determinar el envase:
    switch (definirEnvase(resultfinal))
    {
    case 0:
        // No es ningun envase retorna false:
        return false;
        break;
    default:
        // Verificar que tipo de envase es:
        tipo = definirEnvase(resultfinal);
        break;
    }
    return true;
}