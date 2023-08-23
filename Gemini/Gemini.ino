#include "Gemini_begin.h"

void loop()
{
    debugln("Enviando dados!");
    telemetria.enviaTeste();
    debugln("Aguardando...");
    delay(2000);
    gemini.dormir(30);
} 
