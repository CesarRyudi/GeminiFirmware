#include "classes.h"


void setup(){


    //Begins 
    Serial.begin(38400);
    rtc.begin();
    memoria.begin();
    gemini.begin();
    id = gemini.getId();

    Serial.println((String)"Versao do firmware: "+versao);
    Serial.println((String)"id = "+id);
    Serial.println("Iniciado com sucesso. Aguardando comando");


}