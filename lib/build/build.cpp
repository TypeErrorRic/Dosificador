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
}

/************ IMPLEMETNACIÓN PARA EL CALCULO DE LA DERIVADA **************/

void doDerivada()
{
}

/************ IMPLEMETNACIÓN DE LA DISPENSADORA DE GRANO **************/
static unsigned long prev = 0;
// Devuelve el estado de la Tolva dispensadora.
bool stateTolva()
{
	if(getEstado() == 3) prev = 0;
	//Simulación:
	if (prev > 0ul)
	{
		if ((millis() - prev) >= 5000UL)
		{
			prev = 0ul;
			return false;
		}
		else
			return true;
	}
	else
	{
		if(SENSAR_TOLVA)
			return true;
		else
			return false;
	}
}

// Activa la función de llenado de la Tolva dispensadora.
void fillTolva()
{
	// Aquiva el programa:
	Serial.println("Llenado...");
	prev = millis();
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
	if (Serial.available() > 0)
	{
		switch ((char)Serial.read())
		{
		case '1':
			if (RECONOCIMIENTO_ENVASE) {MRECONOCIMIENTO_ENVASE(0);}
			else
				MRECONOCIMIENTO_ENVASE(1) { ; }
			break;
		default:
			break;
		}
	}
	//Programa de reconocimiento de tipo:
	if (RECONOCIMIENTO_ENVASE)
	{
		if (getEstado() == 5)
		{
			Serial.readString();
			Serial.println("Se ha reconido un envase.");
			Serial.println("Ingrese el tipo de envase: ");
			// Programa de reconocimiento de envase:
			while (1)
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
					case '4':
						Tipo = 3;
						MCICLO_LLENADO(1);
						return true;
						break;
					default:
						Tipo = 10;
						Serial.println("El envase no pertene a uno de los 3");
						return true;
						break;
					}
					if(Tipo != 10)
					{
						Serial.print("Tipo: ");
        		Serial.println(TIPO_ENVASE);
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
	prev = millis();
}

// Verfica el llenado del envase.
bool revisarLLenado()
{
	if (prev > 0ul)
	{
		if ((millis() - prev) >= 5000UL)
		{
			prev = 0ul;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if(CICLO_LLENADO)
			return true;
		else
			return false;
	}
}

// Detiene el llenado del envase.
float stopLllenadoEnvase()
{
	Serial.println("El envase se ha llenado con exito");
	return 10;
}

/********************* IMPLEMENTACIÓN PARA SISTEMA DE ALERTA ************************/

// Activa la alarma correspondiente al evento que suceda.
void alarma(short type, bool state)
{
	digitalWrite(ALARMA, state);
}

// Inizialiar el sistema de Alarma
void initAlarma()
{
	pinMode(ALARMA, OUTPUT);
}