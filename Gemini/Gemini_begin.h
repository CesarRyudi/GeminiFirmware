#include "classes.h"



void setup(){
    //Begins 
    Serial.begin(38400);
    telemetria.begin();
    rtc.begin();
    memoria.begin();
    adc.begin();
    gemini.begin();
    id = gemini.getId();

    Serial.println("Inicio");
    Serial.println("");
    Serial.print("Leituras: ");
    Serial.println(memoria.getEnderecoAtual() / bitsPorLeitura);
    Serial.print("Leituras já lidas: ");
    Serial.println(EEPROM.readLong(EEPROM_ultima_leitura) / bitsPorLeitura);
    Serial.print("Leituras novas: ");
    Serial.println((memoria.getEnderecoAtual() - EEPROM.readLong(EEPROM_ultima_leitura)) / bitsPorLeitura);
    Serial.println((String)"Versao do firmware: "+ versao);
    Serial.println((String)"id = "+ id);
    Serial.println("Iniciado com sucesso. Aguardando comando");
    com.comando(10);
    debugln("Modo de testes Iniciado!");
}