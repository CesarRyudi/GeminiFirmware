#include "Gemini_begin.h""

void loop(){
    gemini.dormir(30);
    if(gemini.horaDeAcordar()) {
        debugln("Hora de acordar");
        gemini.acordar();
    }

}

