#include "Gemini_begin.h"

    void loop()
{
    bool acorda = false;
    interrupt.verifica();
    gemini.horaDeAcordar();
    if (deveAcordar)
    {
        debugln("Hora de acordar");
        gemini.acordar();
        if (rtc.getMinuto() == 0 || rtc.getMinuto() == 30) telemetria.enviaDados(memoria.read(100));
        deveAcordar = false;
        delay(10);
    }
    gemini.dormir(30);
}
