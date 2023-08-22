#include "Gemini_begin.h"

void loop()
{

    debugln("acorda");
    // interrupt.verifica();
    if(modoDeTestesAcionado){
    debugln("Modo de testes detectado!");
    modoDeTestes();
    modoDeTestesAcionado = false;
    } 
    gemini.horaDeAcordar();
    if (deveAcordar)
    // if(true)
    {
        debugln("Hora de acordar");
        interrupt.desativa();
        gemini.acordar();
        if (rtc.getMinuto() == 0 && rtc.getHora() == 10)
        // if(true)
        {
            delay(2000);
            telemetria.enviaDados(memoria.read(48));
        }
        else if (rtc.getMinuto() == 0 && rtc.getHora() == 22)
        {
            delay(2000);
            telemetria.enviaDados(memoria.read(48));
        }

        interrupt.ativa();
        deveAcordar = false;
        delay(10);
    }
    gemini.dormir(30);
}


