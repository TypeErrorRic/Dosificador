#include <derivada.h>
#include <Arduino.h>

static unsigned long timer;
static unsigned long past_timer;

float Derivada(float x, float (*y)(void), unsigned int time)
{
    past_timer = millis();
    timer = millis();
    while(timer <= (time + past_timer)){timer = millis();}
    return (y() - x)/(timer - past_timer);
}