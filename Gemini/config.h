#include <SPIMemory.h>
#include <Nanoshield_ADC.h>
#include <EEPROMex.h>
#include <LowPower.h>

#include "Gemini_hardware.h"

//Versão do firmware
#define versao "4.0.0"



//Debug - Mude a variavel DEBUG_ON para True pra fazer com que o os prints sejam ativados.
#define DEBUG 1

#if DEBUG == 1
#define debug(texto_debug) Serial.print(texto_debug)
#define debugln(texto_debug) Serial.println(texto_debug)
#define debugDelay(tempo_delay) delay(tempo_delay)
#else
#define debug(texto_debug)
#define debugln(texto_debug) 
#define debugDelay(tempo_delay)
#endif




// Configurações 
#define tempoLimite 300000
#define bitsPorLeitura 64
#define intervaloDeLeiturasPadrao 5


//Endereço do relógio
#define DS1307_ADDRESS 0x68

//Endereços da EEPROM: Bytes
#define EEPROM_intervalo 102

//#define EEPROM_s1 103
//#define EEPROM_s2 104
//#define EEPROM_s3 105

//Endereços da EEPROM: Longs
#define EEPROM_id 200
#define EEPROM_interrupts 210
#define EEPROM_rangeS0 220
#define EEPROM_rangeS1 230
#define EEPROM_rangeS2 240
#define EEPROM_rangeS3 250
#define EEPROM_ultima_leitura 300
#define EEPROM_enderecoAtual 350



// Modos
#define normal 0
#define configuracao 1
#define testa 2


// Variaveis globais
int id;

// Objetos usados nas minhas classes 
SPIFlash flash(45);
Nanoshield_ADC adcOriginal;


