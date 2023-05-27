#include <Build.h> //Desarrollado por Ricardo Pabón Serna.
#include <Memoria.h>
// Archivo de cabecera.
#include "C:\Users\ricardo\Desktop\Taller_Ing_II\include\Selecion_datos.h"

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
	// Si se ha realizado la toma de valores con un patron incapaz de ser acomodados en una curva cuadratica. Lazara error.
	if (Matriz.get_Realizar())
	{
		Matriz.get_Matriz();
		Matriz.Calcular();
	}
	++(A = Matriz.Get_valor_a());
	++(X = Matriz.Get_valor_x1());
	++(X2 = Matriz.Get_valor_x2());
	++(VARIABLE_REST = 1);
}

/************ IMPLEMETNACIÓN PARA EL CALCULO DE LA DERIVADA **************/
//Función de interpretación del convertidor analogico.
static float Medir_Peso()
{
	return ((float)(analogRead(A0)/(float)1023)*1000);
};

//Realizar la derivada.
void doDerivada()
{
	getLcd().clear();
	while(1) 
	{
		escribirLcd<String>("Peso:", 0,0);
		escribirLcd<float>(Medir_Peso(), 0, 9);
		escribirLcd<String>(" G", 0, 11);
		escribirLcd<String>("Dervida:", 1,0);
		getLcd().print(Derivada(Medir_Peso(), Medir_Peso, 500));
	}
}

/************ IMPLEMETNACIÓN DE LA DISPENSADORA DE GRANO **************/
// Devuelve el estado de la Tolva dispensadora.
bool stateTolva()
{
	if (digitalRead(PIN_SENSOR_TOLVA))
	{
		delay(TIEMPO_REVISION);
		return digitalRead(PIN_SENSOR_TOLVA);
	}
	else
		return false;
}

// Activa la función de llenado de la Tolva dispensadora.
void fillTolva()
{
	// Aquiva el programa:
	Serial.println("Llenado...");
}

// Dectiene la Función de llenado de la Tolva dispensadora.
void offTolva()
{
	// Aquiva el programa:
	Serial.println("Se ha llenado la tolva Dosificadora.");
}

/************ IMPLEMETACIÓN DEl SISTEMA DE ENVASADO **************/

// Función que permite conocer el tipo de envase.
bool revisarEnvase(short &Tipo)
{
	// Programa de reconocimiento de envase:
	if (digitalRead(PIN_ENVASADO) && !RECONOCIMIENTO_ENVASE)
	{
		delay(TIEMPO_REVISION);
		if (digitalRead(PIN_ENVASADO)) {MRECONOCIMIENTO_ENVASE(1);}
	}
	else
	{
		if (RECONOCIMIENTO_ENVASE && !digitalRead(PIN_ENVASADO))
		{
			delay(TIEMPO_REVISION);
			if (!digitalRead(PIN_ENVASADO)) {MRECONOCIMIENTO_ENVASE(0);}
		}
	}
	// Programa de reconocimiento de tipo:
	if (RECONOCIMIENTO_ENVASE)
	{
		if (getEstado() == 5)
		{
			Serial.readString();
			Serial.println("Se ha reconido un envase.");
			Serial.println("Ingrese el tipo de envase: ");
			// Programa de reconocimiento de envase:
			while (1)
			{
				while (Serial.available() > 0)
				{
					switch ((char)Serial.read())
					{
					case '1':
						Tipo = 1;
						return true;
						break;
					case '2':
						Tipo = 2;
						return true;
						break;
					case '3':
						Tipo = 3;
						return true;
						break;
					default:
						Tipo = 10;
						Serial.println("El envase no pertene a uno de los 3");
						return true;
						break;
					}
					if (Tipo != 10)
					{
						Serial.print("Tipo: ");
						Serial.println(TIPO_ENVASE);
					}
				}
				if(!digitalRead(PIN_ENVASADO)) //Revisar si el envase sigue en el puesto.
				{
					MRECONOCIMIENTO_ENVASE(0);
					Serial.println("Se retiro el envase.");
					return false;
				}	
			}
		}
		return true;
	}
	else
	{
		if ((getEstado() == 2) || (getEstado() == 8) || (getEstado() == 6))
			Serial.println("El envase se ha retirado");
		return false;
	}
}

// Activa el llenado del envase.
void llenandoEnvase()
{
	// Aqui va el programa de llenado.
	Serial.println("Llenando envase.");
}

// Verfica el llenado del envase.
bool revisarLLenado()
{
	// Programa para revisar se lleno el envase.
	if (digitalRead(PIN_CICLO_LLENADO))
	{
		delay(TIEMPO_REVISION);
		return digitalRead(PIN_CICLO_LLENADO);
	}
	else
		return false;
}

// Detiene el llenado del envase.
float stopLllenadoEnvase()
{
	// Programa para detención del llenado del envase.
	Serial.println("El envase se ha llenado con exito");
	return 10;
}

/********************* IMPLEMENTACIÓN PARA SISTEMA DE ALERTA ************************/

// Activa la alarma correspondiente al evento que suceda.
void alarma(short type, bool state)
{
	if(state)escribirLcd<String>("!ALARMA!", 0,0, true);
	if(state)escribirLcd<short>(type, 1,0);
	if(state)delay(DELAY_EJE);
	digitalWrite(ALARMA, state);
}

// Inizialiar el sistema de Alarma
void initAlarma()
{
	pinMode(ALARMA, OUTPUT);
}