#include "Gemini_begin.h""

void loop(){
    //gemini.dormir(3);
    debugDelay(1000);
    if(gemini.horaDeAcordar()) {
        debugln("Hora de acordar");
        gemini.acordar();
    }

    if(Serial.available()){
        memoria.read();
    }
}
