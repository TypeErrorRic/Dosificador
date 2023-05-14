#include <BUILD.h> //Desarrollado por Ricardo Pabón Serna.
// Archivo de cabecera.
#include "C:\Users\ricardo\Desktop\Taller_Ing_II\include\Selecion_datos.h"

/********************* IMPLEMENTACIÓN DE IMPRECIÓN EN LCD **************************/

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setLCD()
{
    lcd.init(); // initialize the lcd
    lcd.backlight();
    Serial.begin(9600);
    lcd.setCursor(0, 0);
    lcd.print("Ready");
}

void ecribirLcd(int estado)
{
    lcd.clear();
    lcd.print("Estado: ");
    lcd.print(estado);
}

/*********** IMPLEMENTACIÓN DE FUNCIONES PARA EL MANEJO DEL FLUJO DEL PROGRAMA ****************/

// Revision de las diferentes entradas del sistema.
void RevisionSensoresInit()
{
    // Revisar cada entrada:
    *REGENTRADAS = 0x00;
}

// Revision de los sensores de cada sistema:
void Revision_variables()
{
    while (Serial.available() > 0)
    {
        char aux = (char)Serial.read();
        switch (aux)
        {
        case '1':
            MSENSORTOLVA(1);
            Serial.println("Se ha llenado la tolva Dosificadora.");
            break;
        case '2':
            if (RECONOCIMIENTO_ENVASE)
            {
                Serial.println("El envase se ha retirado");
                MRECONOCIMIENTO_ENVASE(0);
                MENVASE_CORRECTO(0);
                MCICLO_LLENADO(0);
                MTIPO_ENVASE(0);
            }
            else
            {
                MRECONOCIMIENTO_ENVASE(1);
                Serial.println("Se ha reconocido un envase.");
            }
            break;
        case '3':
            if (!ENVASE_CORRECTO)
            {
                MRECONOCIMIENTO_ENVASE(1);
                MENVASE_CORRECTO(1);
                Serial.println("Tipo Envase:");
                while(TIPO_ENVASE == 0) while(Serial.available() > 0)
                {
                    switch ((char)Serial.read())
                    {
                    case '1':
                        MTIPO_ENVASE(1);
                        break;
                    case '2':
                        MTIPO_ENVASE(2);
                        break;
                    case '3':
                        MTIPO_ENVASE(3);
                        break;
                    default:
                        break;
                    }
                }
            }
            else
            {
                MRECONOCIMIENTO_ENVASE(1);
                MENVASE_CORRECTO(0);
                Serial.println("El envase no pertene a uno de los 3");
            }
            break;
        case '6':
            MCICLO_LLENADO(1);
            MSENSORTOLVA(0);
            Serial.println("El envase se ha llenado con exito");
            break;
        default:
            break;
        }
    }
}

/************ IMPLEMETNACIÓN PARA EL CALCULO DE LA REGRESION CUADRATICA **************/

static struct // Valores Para el calculo de la matriz.
{
    float x[NFILAS]{}; // Medidas de la masa con respecto al eje X.
    float y[NFILAS]{}; // Medidas de la masa con respecto al eje Y.
} Valores_Sensores;

// Insanciación de un obecto de la clase::<Regresion_Cuadratica.h>
static Regresion_Cuadratica Matriz(NFILAS, Valores_Sensores.x, Valores_Sensores.y);

// Setup para la realización de la regresión Cuadratica:
void initRegresionCuadratica()
{
    for (int i = 0; i < NFILAS; i++)
    {
        Valores_Sensores.x[i] = 0;
        Valores_Sensores.y[i] = 0;
    }
    Matriz.reset();
}

// Realización de la Obtención de los datos Para la Regresión Cuadratica:
void doRegresionCuadratica()
{
    int contador_num{0};
    Serial.println("Calculando....");
    // Bucle principal de toma de datos:
    do
    {
        delay(1000);
        Serial_events(Valores_Sensores.x[contador_num], Valores_Sensores.y[contador_num], Matriz.get_Realizar());
        contador_num++;
        // Si se ha realizado la toma de valores con un patron incapaz de ser acomodados en una curva cuadratica. Lazara error.
        if (Matriz.get_Realizar())
        {
            Serial.print("Coordenada #: ");
            Serial.print(contador_num);
            Serial.println(" Tomada correctamente.");
        }
    } while (Matriz.Update(Valores_Sensores.x[contador_num - 1], Valores_Sensores.y[contador_num - 1]));
    digitalWrite(LED_BUILTIN, LOW);
    // Si se ha realizado la toma de valores con un patron incapaz de ser acomodados en una curva cuadratica. Lazara error.
    if (Matriz.get_Realizar())
    {
        Matriz.get_Matriz();
        Matriz.Calcular();
    }
    else
        Serial.print("Reiniciar.");
    delay(1000);
}
