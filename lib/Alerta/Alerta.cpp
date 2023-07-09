#include <Alerta.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// CÓDIGO DEL FUNCIONAMIENTO DEL SISTEMA DE ALARMAS:
static unsigned long time_Alerta = 0;
static unsigned long time_Visual = 0;
static bool Cambio = true;
// Declaración de clase:
Alerta Alertas(PIN_ALERTA);

void setupInteruptMassage()
{
    //Configurar el puerto como entrada:
    DDRD &= ~(1 << PB0); //Pin D53.
    // Enabling INT0 interrupt
    EIMSK |= (1 << INT0);
    // Enabling INT0 is called at any logical change on INT0
    EICRA |= (1 << ISC00);
    sei();
}

ISR(INT0_vect)
{
    if (PIND2)
        mostrarMensaje = 0;
    else
        mostrarMensaje;
}

void Alerta::S0() // mensaje de estado s1, esperando accionamineto conmutador
{
    if (DELAY_TIME_SONIDO < (millis() - time_Alerta))
    {
        digitalWrite(bozzer, !digitalRead(bozzer));
        time_Alerta = millis();
    }
    if (DELAY_TIME_VISUALIZACION < (millis() - time_Visual))
    {
        if (Cambio)
        {
            escribirLcd<String>("Esperando", 0, 0, true);
            escribirLcd<String>("Accionamiento", 1, 0);
            Cambio = false;
        }
        else
        {
            escribirLcd<String>("Conmutador", 0, 0, true);
            Cambio = true;
        }
        time_Visual = millis();
    }
}

void Alerta::S1() // mensaje de alerta de envase en la salida
{
    escribirLcd<String>("Hay una lata en", 0, 0, true);
    escribirLcd<String>("La salida", 0, 1);
    if (((unsigned long)DELAY_TIME_SONIDO / DIVISOR_S1) < (millis() - time_Alerta))
    {
        digitalWrite(bozzer, !digitalRead(bozzer));
        time_Alerta = millis();
    }
}

void Alerta::S2() // alerta identificacion del tipo de envase
{
    if (DELAY_TIME_VISUALIZACION < (millis() - time_Visual))
    {
        if (Cambio)
        {
            escribirLcd<String>("Identificando", 0, 0, true);
            escribirLcd<String>("tipo recipiente.", 0, 1);
            Cambio = false;
        }
        else
        {
            if (*REGENTRADAS & ~(1 << 2)) // no hay Lata /*== 0000*/
            {
                escribirLcd<String>("No hay ", 0, 0, true);
                escribirLcd<String>("recipiente", 0, 1);
            }
            else if ((*REGENTRADAS & 0x3C) == ((1 << 2) | (1 << 3) | (1 << 5))) // lata tipo 1
            {
                escribirLcd<String>("Recipiente tipo", 0, 0, true);
                escribirLcd<String>("uno", 0, 1);
            }
            else if ((*REGENTRADAS & 0x3C) == ((1 << 2) | (1 << 3) | (1 << 4))) // lata tipo 2
            {
                escribirLcd<String>("Recipiente tipo", 0, 0, true);
                escribirLcd<String>("dos", 0, 1);
            }
            else if ((*REGENTRADAS & 0x3C) == ((1 << 2) | (1 << 3) | (1 << 5) | (1 << 4))) // lata tipo 3
            {
                escribirLcd<String>("Recipiente tipo", 0, 0, true);
                escribirLcd<String>("tres", 0, 1);
            }
            else if ((*REGENTRADAS & 0x0C) == ((1 << 2))) // falla en el sistema de deteccion de envase.
            {
                escribirLcd<String>("No se reconocio", 0, 0, true);
                escribirLcd<String>("el recipiente", 0, 1);
            }
            else // fallo lectura sensores
            {
                escribirLcd<String>("Fallo en el", 3, 0, true);
                escribirLcd<String>("sistema", 5, 1);
            }
            Cambio = true;
        }
        time_Visual = millis();
    }
    if ((*REGENTRADAS & 0x0C) == ((1 << 2)) && ((unsigned int)DELAY_TIME_SONIDO / DIVISOR_S2_RECONOCE < (millis() - time_Alerta)))
    {
        digitalWrite(bozzer, !digitalRead(bozzer));
        time_Alerta = millis();
    }
}

void Alerta::S3() // alerta de nivel de tolva
{
    if (DELAY_TIME_VISUALIZACION < (millis() - time_Visual))
    {
        if (Cambio)
        {
            escribirLcd<String>("Comprobando ", 0, 0, true);
            escribirLcd<String>("nivel tolva.", 0, 1);
            Cambio = false;
        }
        else
        {
            if ((*REGENTRADAS & 0x02) & ~(1 << 2)) // nivel bajo o vacio tolva
            {
                escribirLcd<String>("Nivel de tolva", 0, true);
                escribirLcd<String>("bajo.", 5, 1);
            }
            else
            {
                escribirLcd<String>("Nivel de tolva", 0, 0, true);
                escribirLcd<String>("maximo.", 3, 1);
            }
            Cambio = true;
        }
        time_Visual = millis();
    }
    if((*REGENTRADAS & 0x02) & ~(1 << 2) && (((unsigned int)DELAY_TIME_SONIDO / DIVISOR_S3) < (millis() - time_Alerta)))
    {
        digitalWrite(bozzer, !digitalRead(bozzer));
        time_Alerta = millis();
    }
}

void Alerta::S4() // alerta de alimentador encendido
{
    escribirLcd<String>("Alimentador", 2, 0);
    escribirLcd<String>("encendido", 3, 1);
    if(((unsigned int)DELAY_TIME_SONIDO / DIVISOR_S4) < (millis() - time_Alerta))
    {
        digitalWrite(bozzer, !digitalRead(bozzer));
        time_Alerta = millis();
    }
}

void Alerta::S5() // Alerta de llenado de recipiente
{
    escribirLcd<String>("Llenando ", 4, 0, true);
    escribirLcd<String>("recipiente", 3, 1);
}

void Alerta::S6() // Alerta de llenado terminado
{
    escribirLcd<String>("Recipiente ", 3, 0, true);
    escribirLcd<String>("llenado", 4, 1);
     if(((unsigned int)DELAY_TIME_SONIDO / DIVISOR_S6) < (millis() - time_Alerta))
    {
        digitalWrite(bozzer, !digitalRead(bozzer));
        time_Alerta = millis();
    }
}

void Alerta::Desactivar()
{
    digitalWrite(bozzer, LOW);
}

/* - Código para LCD sin I2C */
static LiquidCrystal_I2C lcd = getLcd();

//// Variables auxiliares ////
static short aux = 0;
static short reset = 0;
static short mostrando = 0;        /* Variable utilizada para determinar si esta siendo mostrada la opción seleccionada en el menú
                             Se utiliza en la función titilarSeleccion*/
static const long interval = 1500; // Intervalo de tiempo para el titilar de la selección.
static char auxCadena[16];

static int del = 200; // Delay para evitar el ruido.

//// Variables del sistema ////

static unsigned short velocidadDeLlenado = 5;
static unsigned short envasesLlenados = 0; // Esta variable cambiaría su valor con base al desarrollo que se presente en los demás bloques.
static unsigned short gramosLlenados = 0;  // Esta variable cambiaría su valor con base al desarrollo que se presente en los demás bloques.

static int PESOSMAXIMOS[3] = {179, 282, 330}; // Pesos de los tres tipos de recipientes cuando están llenos.
static int PESOSMINIMOS[3] = {31, 40, 50};    // Pesos de los tres tipos de recipientes cuando está con la cantidad mínima.

static int pesos[3] = {179, 282, 330}; // Pesos del envase 1, 2 y 3 respectivamente (incluyendo el peso de la lata vacía).
                                       // NOTA: el peso 3 no se ha definido aún, por lo que por ahora se define (arbitrariamente) como 330.

/* Variables de Menú */
static char MENU[3][17] = {"1-Vel.Llenado(5)", "2-CambiarPesos", "3-Estadísticas"}; // Menú inicial
static int seccionMenu = 0;                                                         /* Cual sección del menú está siendo mostrada (0=: sección 1 MENU[0:3]
                                                                                                                            1=: sección 2 MENU[3])*/
static int opcion = 0;                                                              // Opción del menú seleccionada al momento (esta titila).

/// Variables para el uso de millis()
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;

static void titilarSeleccion(int seccion, int opcion, int reset)
{
    // Se utiliza millis() para mostrar y ocultar la selección actual con el fin de que esta titile.

    if (reset == 0)
    {
        currentMillis = millis();
    }
    if (mostrando == 0 && currentMillis - previousMillis >= interval / 2)
    {
        switch (seccion)
        {
        case 0:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(MENU[0]);
            lcd.setCursor(0, 1);
            lcd.print(MENU[1]);
            mostrando = 1;
            break;
        case 1:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(MENU[2]);
            mostrando = 1;
            break;
        }
        previousMillis = currentMillis;
    }
    else if (mostrando == 1 && currentMillis - previousMillis >= interval)
    {
        switch (seccion)
        {
        case 0:
            lcd.clear();
            lcd.setCursor(0, !opcion);
            lcd.print(MENU[!opcion]);

            mostrando = 0;
            break;

        case 1:
            lcd.clear();
            mostrando = 0;
            break;
        }
        previousMillis = currentMillis;
    }
}

static void SeleccionDeVelocidad()
{
    // El usuario selecciona entre las 5 velocidades de llenado.
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Velocidad de");
    int vel_actual = velocidadDeLlenado;
    delay(del);

    while (1)
    {
        // Se cambia la velocidad.
        if (digitalRead(botonArriba) == LOW && velocidadDeLlenado < 5)
        {
            velocidadDeLlenado++;
        }
        else if (digitalRead(botonAbajo) == LOW && velocidadDeLlenado > 1)
        {
            velocidadDeLlenado--;
        }
        lcd.setCursor(0, 1);
        sprintf(auxCadena, "llenado: %d", velocidadDeLlenado);
        lcd.print(auxCadena);
        delay(del);

        // Se confirma ó cancela el cambio.
        if (digitalRead(botonCancelar) == LOW)
        {
            velocidadDeLlenado = vel_actual;
            delay(del);
            break;
        }
        else if (digitalRead(botonSeleccionar) == LOW)
        {
            sprintf(auxCadena, "1-Vel.Llenado(%d)", velocidadDeLlenado);
            // Se actualiza la primera sección del menú para que muestre la velocidad de llenado actual.
            strcpy(MENU[0], auxCadena);
            strcpy(MENU[1], "2-CambiarPesos");
            delay(del);
            break;
        }
    }
}

static void CambiarPeso(int tipoDeEnvase)
{
    // Se configura el peso del tipo de envase seleccionado.

    aux = pesos[tipoDeEnvase - 1];
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Seleccione el");
    lcd.setCursor(0, 1);
    lcd.print("nuevo peso: ");
    lcd.setCursor(12, 1);
    lcd.print(aux);

    while (1)
    {

        // El usuario puede aumentar/disminuir de 10 en 10 el peso siempre y cuando no sobrepase los límites establecidos.
        if (digitalRead(botonArriba) == LOW && aux < PESOSMAXIMOS[tipoDeEnvase - 1])
        {

            if (aux + 10 > PESOSMAXIMOS[tipoDeEnvase - 1])
            {
                aux = PESOSMAXIMOS[tipoDeEnvase] - 1;
            }
            else
            {
                aux = aux + 10;
            }
            lcd.setCursor(12, 1);
            lcd.print("    ");
            lcd.setCursor(12, 1);
            lcd.print(aux);
            lcd.setCursor(0, 0);
            delay(del);
        }
        else if (digitalRead(botonAbajo) == LOW && aux > PESOSMINIMOS[tipoDeEnvase - 1])
        {

            if (aux - 10 < PESOSMINIMOS[tipoDeEnvase - 1])
            {
                aux = PESOSMINIMOS[tipoDeEnvase - 1];
            }
            else
            {
                aux = aux - 10;
            }
            lcd.setCursor(12, 1);
            lcd.print("    ");
            lcd.setCursor(12, 1);
            lcd.print(aux);
            lcd.setCursor(0, 0);
            delay(del);
        }

        // Se confirma ó cancela la modificación.

        if (digitalRead(botonCancelar) == LOW)
        {
            aux = 0;
            delay(del);
            break;
        }
        else if (digitalRead(botonSeleccionar) == LOW)
        {
            pesos[tipoDeEnvase - 1] = aux;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Se ha cambiado");
            lcd.setCursor(0, 1);
            sprintf(auxCadena, "el peso %d (%d)", tipoDeEnvase, pesos[tipoDeEnvase - 1]);
            lcd.print(auxCadena);
            delay(3000);
            lcd.clear();
            aux = 0;
            lcd.setCursor(0, 0);
            break;
        }
    }
}

static void configPesos()
{
    // Se seleccionar el tipo de envase al cual cambiar su peso objetivo (aux = tipo de envase).

    aux = 1;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tipo de envase:");

    while (1)
    {
        delay(del);
        lcd.setCursor(0, 1);
        lcd.print(aux);

        if (digitalRead(botonArriba) == LOW && aux != 3)
        {
            aux++;
        }
        else if (digitalRead(botonAbajo) == LOW && aux != 1)
        {
            aux--;
        }

        if (digitalRead(botonSeleccionar) == LOW)
        {
            delay(del);
            CambiarPeso(aux);

            break;
        }
        else if (digitalRead(botonCancelar) == LOW)
        {
            break;
        }
    }
}

/////////////////////////////////////Estadistica//////////////////////////////////////
static void Estadisticas()
{
    // Muestra las estadisticas (cantidad de gramos envasados y cantidad de envases llenados).
    // Los muestra con un intervalo de 3 segundos entre sí.
    int aux = 0;
    delay(del);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Gramos");
    lcd.setCursor(0, 1);
    sprintf(auxCadena, "envasados: %d", gramosLlenados);
    lcd.print(auxCadena);

    while (1)
    {
        currentMillis = millis();
        if (currentMillis - previousMillis >= 3000)
        {
            if (aux == 0)
            {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Gramos");
                lcd.setCursor(0, 1);
                sprintf(auxCadena, "envasados: %d", gramosLlenados);
                lcd.print(auxCadena);

                aux = 1;
                previousMillis = currentMillis;
            }
            else
            {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Envases");
                lcd.setCursor(0, 1);
                sprintf(auxCadena, "llenados: %d", envasesLlenados);
                lcd.print(auxCadena);

                aux = 0;
                previousMillis = currentMillis;
            }
        }
        // Se vuelve al menú inicial con los botones cancelar ó seleccionar.
        if (digitalRead(botonCancelar) == LOW || digitalRead(botonSeleccionar) == LOW)
        {
            lcd.clear();
            delay(del);
            break;
        }
    }
}

//////////////////////////MENU PRINCIPAL DE FUNCIONAMIENTO DEL SISTEMA///////////////////////////

static void MenuPrincipal()
{

    titilarSeleccion(seccionMenu, opcion, reset); // Se titila la selección en el menú

    // Se verifican los botones para el cambio de selección/desplzamiento por el menú.
    if (digitalRead(botonArriba) == LOW && opcion != 0)
    {
        opcion--;

        //
        currentMillis = previousMillis + interval;
        reset = 1;
        titilarSeleccion(seccionMenu, opcion, reset);
        reset = 0;
    }
    else if (digitalRead(botonAbajo) == LOW && opcion != 2)
    {
        opcion++;

        currentMillis = previousMillis + interval;
        reset = 1;
        titilarSeleccion(seccionMenu, opcion, reset);
        reset = 0;
    }
    delay(del); // Delay para evitar relectura.

    // Se define que sección del menú (primera parte ó segunda parte) mostrar con base al desplazamiento en este.
    if (opcion <= 1)
    {
        seccionMenu = 0;
    }
    else if (opcion == 2)
    {
        seccionMenu = 1;
    }

    // Se verifica si se realizó selección de la opción seleccionada
    if (digitalRead(botonSeleccionar) == LOW)
    {
        switch (opcion)
        {
        case 0:
            SeleccionDeVelocidad();
            break;
        case 1:
            configPesos();
            break;
        case 2:
            Estadisticas();
            break;
        }
    }
}

void setupPantalla()
{
    // Setup pinMode

    // NOTA: Tener en cuenta que están configuradas en pull_up.
    pinMode(botonCancelar, INPUT_PULLUP);
    pinMode(botonArriba, INPUT_PULLUP);
    pinMode(botonSeleccionar, INPUT_PULLUP);
    pinMode(botonAbajo, INPUT_PULLUP);
    pinMode(interruptorMaestro, INPUT_PULLUP);
}

void MostrarPantalla(bool isEnvasadoEnable)
{
    while (isEnvasadoEnable)
    {
        MenuPrincipal();
    }
    // ... ejecución del ciclo de envasado...
}
