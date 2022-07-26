#include "classes.h"


void setup(){
    //Begins 
    Serial.begin(38400);
    rtc.begin();
    memoria.begin();
    adc.begin();
    gemini.begin();
    id = 111;
    //gemini.getId();

    Serial.println((String)"Versao do firmware: "+ versao);
    Serial.println((String)"id = "+ id);
    Serial.println("Iniciado com sucesso. Aguardando comando");
    com.comando(15);

}