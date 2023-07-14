#include "Gemini_begin.h"

void loop()
{
    bool acorda = false;
    interrupt.verifica();
    gemini.horaDeAcordar();
    if (deveAcordar)
    // if(true)
    {
        debugln("Hora de acordar");
        gemini.acordar();
        // if (rtc.getMinuto() == 0 && rtc.getHora() == 10)
        if(true)
        {
            delay(2000);
            telemetria.enviaDados(memoria.read(48));
        }
        deveAcordar = false;
        delay(10);
    }
    gemini.dormir(30);
}
