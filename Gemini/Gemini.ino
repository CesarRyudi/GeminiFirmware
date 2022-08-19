#include "Gemini_begin.h""

void loop(){
    interrupt.verifica();
    if(gemini.horaDeAcordar()) {
        debugln("Hora de acordar");
        gemini.acordar();
    }
    gemini.dormir(30);
}