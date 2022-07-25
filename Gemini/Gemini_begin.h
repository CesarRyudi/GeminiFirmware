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

    delay(2000);

    Serial.println((String)"Versao do firmware: "+ versao);
    Serial.println((String)"id = "+ id);
    Serial.println("Iniciado com sucesso. Aguardando comando");
    String texto = com.serial(10);

}