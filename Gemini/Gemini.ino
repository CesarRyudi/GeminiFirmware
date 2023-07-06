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
        if (rtc.getMinuto() == 0 || rtc.getMinuto() == 15 || rtc.getMinuto() == 30 || rtc.getMinuto() == 45)
            delay(2000);
            telemetria.enviaDados(memoria.read(48));
        deveAcordar = false;
        delay(10);
    }
    gemini.dormir(30);
}
