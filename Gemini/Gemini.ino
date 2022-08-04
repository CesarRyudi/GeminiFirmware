#include "Gemini_begin.h""

void loop(){
    gemini.dormir(3);
    if(gemini.horaDeAcordar()) {
        debugln("Hora de acordar");
        gemini.acordar();
    }

}

