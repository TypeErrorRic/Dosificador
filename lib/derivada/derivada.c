#include <derivada.h>
#include <Arduino.h>

static unsigned long timer;
static unsigned long past_timer;

float Derivada(float x, float y, unsigned int time)
{
    past_timer = millis();
    float derivada = 0;
    while(timer <= time){timer = millis();}
    derivada = x - y;
    derivada /= (past_timer - timer);

    return derivada;
}