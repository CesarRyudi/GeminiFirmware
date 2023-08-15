#include "config.h"

bool deveAcordar = false;

bool int1Acionado = false, int2Acionado = false;

void interrupt1()
{
    detachInterrupt(digitalPinToInterrupt(interruptPin));
    detachInterrupt(digitalPinToInterrupt(interruptPin2));
    int1Acionado = true;
}

void interrupt2()
{
    detachInterrupt(digitalPinToInterrupt(interruptPin));
    detachInterrupt(digitalPinToInterrupt(interruptPin2));
    int2Acionado = true;
}