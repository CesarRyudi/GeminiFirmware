 #include "config.h"

class Relogio {
  private:
    int segundo = 0, minuto = 0, hora = 0, diadasemana = 0, dia = 0, mes = 0, ano = 0;
    byte zero = 0x00;
    byte h = 255;

    byte ConverteParaBCD(byte val)
    {
      //Converte o número de decimal para BCD
      return ( (val / 10 * 16) + (val % 10) );
    }

    byte ConverteparaDecimal(byte val)
    {
      //Converte de BCD para decimal
      return ( (val / 16 * 10) + (val % 16) );
    }
  
  public: 

    void begin(){
      pinMode(pwr_RTC, OUTPUT);
    }


    void set(byte D, byte M, byte A, byte h, byte m) {

      digitalWrite(pwr_RTC, HIGH);
      Wire.beginTransmission(0x68);
      Wire.write(zero);
      Wire.write(ConverteParaBCD(0));
      Wire.write(ConverteParaBCD(m));
      Wire.write(ConverteParaBCD(h));
      Wire.write(ConverteParaBCD(1));
      Wire.write(ConverteParaBCD(D));
      Wire.write(ConverteParaBCD(M));
      Wire.write(ConverteParaBCD(A));
      Wire.write(zero);
      Wire.endTransmission();
      digitalWrite(pwr_RTC, LOW);

    }

    void read(){
        digitalWrite(pwr_RTC, HIGH);

        Wire.beginTransmission(0x68);
        Wire.write(zero);
        Wire.endTransmission();
        Wire.requestFrom(0x68, 7);
        segundo = ConverteparaDecimal(Wire.read());
        minuto = ConverteparaDecimal(Wire.read());
        hora = ConverteparaDecimal(Wire.read() & 0b111111);
        diadasemana = ConverteparaDecimal(Wire.read()) + 1;
        dia = ConverteparaDecimal(Wire.read());
        mes = ConverteparaDecimal(Wire.read());
        ano = ConverteparaDecimal(Wire.read());

        digitalWrite(pwr_RTC, LOW);
    }

    byte getDia(){return dia;}
    byte getMes(){return mes;}
    byte getAno(){return ano;}
    byte getHora(){return hora;}
    byte getMinuto(){return minuto;}
    byte getSegundo(){return segundo;}
  };

  Relogio rtc;

/************************************************************************************************************************************************************************************************/

class MemoriaFlash {
  private: 
  long enderecoAtual; 

  public:
    long getEnderecoAtual(){return enderecoAtual;}

    void begin(bool printa = 0){
      digitalWrite(pwr_memoria, HIGH);
      delay(200);

      flash.begin();
      debugln((String)"eeprom: "+EEPROM.readLong(EEPROM_enderecoAtual));
      enderecoAtual = EEPROM.readLong(EEPROM_enderecoAtual);
      enderecoAtual = enderecoAtual * bitsPorLeitura;

      if(printa){
        Serial.println((String)"Leituras: "+ (enderecoAtual / bitsPorLeitura));
        Serial.println((String)"Leituras já lidas: "+(EEPROM.readLong(EEPROM_ultima_leitura) / bitsPorLeitura));
        Serial.print((String)"Leituras novas: "+((enderecoAtual - EEPROM.readLong(EEPROM_ultima_leitura)) / bitsPorLeitura));
      }

      digitalWrite(pwr_memoria, LOW);
    }


    void write(String info) {
          digitalWrite(pwr_memoria, HIGH);
          delay(10);

          flash.writeStr(enderecoAtual, info);

          if(DEBUG){
            String lido_da_flash  = "";
            flash.readStr(enderecoAtual, lido_da_flash);
            Serial.println((String)lido_da_flash+"salvo no endereco: "+enderecoAtual);
          }

          enderecoAtual += bitsPorLeitura;
          long int enderecoConvertido = enderecoAtual / bitsPorLeitura;
          EEPROM.writeLong(EEPROM_enderecoAtual, enderecoConvertido);
          EEPROM.writeLong(EEPROM_interrupts, 0);

          digitalWrite(pwr_memoria, LOW);
        }
        

    void encheMemoria() {
      digitalWrite(pwr_memoria, HIGH);
      delay(10);

      String fill = "";
      Serial.println("Criando dados na memoria...");
      for (int i = 0; i < 10000; i++) {
        fill = "";
        fill.concat("0000000000000000000000000");
        if (i < 10000) fill.concat(0);
        if (i < 1000) fill.concat(0);
        if (i < 100) fill.concat(0);
        if (i < 10) fill.concat(0);
        fill.concat(i);

        flash.writeStr(enderecoAtual, fill);
        enderecoAtual += bitsPorLeitura;
        long int enderecoConvertido = enderecoAtual / bitsPorLeitura;
        EEPROM.writeLong(EEPROM_enderecoAtual, enderecoConvertido);
      }

      digitalWrite(pwr_memoria, LOW);
    }


    void clean() {
      digitalWrite(pwr_memoria, HIGH);
      delay(10);

      Serial.println("Limpando memoria...");
      flash.eraseChip();
      EEPROM.writeLong(EEPROM_enderecoAtual, 0);
      EEPROM.writeLong(EEPROM_interrupts, 0);
      Serial.println("Memoria limpa!");
      debugln((String)"EEPROM: "+EEPROM.readLong(EEPROM_enderecoAtual));
      debugln((String)"Interrupts: "+EEPROM.readLong(EEPROM_interrupts));
      enderecoAtual = 0;
      EEPROM.writeLong(EEPROM_ultima_leitura, 0);

      digitalWrite(pwr_memoria, LOW);
    }

    void read(long quantidadeDeLeituras = 0) { // Lê todos os dados se nenhum parâmetro for informado. Se uma quantidade foi informada, lê aquela quantidade de leituras
      digitalWrite(pwr_memoria, HIGH);
      quantidadeDeLeituras = quantidadeDeLeituras * bitsPorLeitura;
      long inicioLeituras = enderecoAtual - quantidadeDeLeituras;
      if (inicioLeituras < 0 || inicioLeituras >= enderecoAtual) Serial.println("Quantidade invalida");
      
      
      else if(quantidadeDeLeituras == 0 ){// Acontece se nenhum parâmetro foi passado. Lê a memória inteira 
        Serial.println("#start");
        delay(100);
        String outputString = "";

        for (long i = 0; i < enderecoAtual; i += bitsPorLeitura) {
          flash.readStr(i, outputString);
          Serial.print((String)"==>"+id+outputString);
        }

        Serial.println("#end");
      }


      else {
        Serial.println((String)"Lendo as ultimas "+quantidadeDeLeituras+" leituras...");
        Serial.println("#start");
        delay(100);
        String outputString = "";

        for (long i = inicioLeituras; i < enderecoAtual; i += bitsPorLeitura) {
          flash.readStr(i, outputString);
          Serial.print((String)"==>"+id+outputString);
        }

        Serial.println("#end");
        }

      digitalWrite(pwr_memoria, LOW);
    }



    void readNew(){
      digitalWrite(pwr_memoria, HIGH);
      delay(100);

      Serial.println("#start");
      String outputString = "";
      long inicio = EEPROM.readLong(EEPROM_ultima_leitura);
      
      for (long i = inicio; i < enderecoAtual; i += bitsPorLeitura) {
        flash.readStr(i, outputString);
        Serial.print((String)"==>"+id+outputString);

      }
      Serial.println("#end");
      EEPROM.writeLong(EEPROM_ultima_leitura, enderecoAtual);

      digitalWrite(pwr_memoria, LOW);
    }

};

  MemoriaFlash memoria;

/************************************************************************************************************************************************************************************************/
class ADC4a20 {
  private: 
  long sensor0, sensor1, sensor2, sensor3;  
  long range0, range1, range2, range3;
  long rangeDecimal0, rangeDecimal1, rangeDecimal2, rangeDecimal3;
  int  mili0, mili1, mili2, mili3;

  public:

    void begin(){
      pinMode(pwr_stepUp, OUTPUT);
      pinMode(pwr_ADC, OUTPUT);
      digitalWrite(pwr_stepUp, LOW);
      digitalWrite(pwr_ADC, LOW);

      adcOriginal.begin();

      range0 = EEPROM.readLong(EEPROM_rangeS0);
      range1 = EEPROM.readLong(EEPROM_rangeS1);
      range2 = EEPROM.readLong(EEPROM_rangeS2);
      range3 = EEPROM.readLong(EEPROM_rangeS3);

    }

    void read(bool delay_leitura = 1){
      digitalWrite(pwr_stepUp, HIGH);
      digitalWrite(pwr_ADC, HIGH);
      if(delay_leitura) delay(1500);
      else delay(100);

      mili0 = adcOriginal.read4to20mA(0) * 1000;
      mili1 = adcOriginal.read4to20mA(1) * 1000;
      mili2 = adcOriginal.read4to20mA(2) * 1000;
      mili3 = adcOriginal.read4to20mA(3) * 1000;

      sensor0 = map(mili0, 3700, 20000,0,(range0*100));
      sensor1 = map(mili1, 3700, 20000,0,(range1*100));
      sensor2 = map(mili2, 3700, 20000,0,(range2*100));
      sensor3 = map(mili3, 3700, 20000,0,(range3*100));
      if (sensor0 < 0) sensor0 = 98765;
      if (sensor1 < 0) sensor1 = 98765;
      if (sensor2 < 0) sensor2 = 98765;
      if (sensor3 < 0) sensor3 = 98765;

      digitalWrite(pwr_stepUp, LOW);
      digitalWrite(pwr_ADC, LOW);
    }


    long getSensor0(){return sensor0;}
    long getSensor1(){return sensor1;}
    long getSensor2(){return sensor2;}
    long getSensor3(){return sensor3;}

    long getRange0(){return range0;}
    long getRange1(){return range1;}
    long getRange2(){return range2;}
    long getRange3(){return range3;}


    bool setRange0(int valor_range){
      if(valor_range < 1) return false;

      else{
      range0 = valor_range;
      EEPROM.writeLong(EEPROM_rangeS0, range0);
      return true;
      }
    }

    
    bool setRange1(int valor_range){
      if(valor_range < 1) return false;

      else{
      range1 = valor_range;
      EEPROM.writeLong(EEPROM_rangeS1, range1);
      return true;
      }
    }
    
    bool setRange2(int valor_range){
      if(valor_range < 1) return false;

      else{
      range2 = valor_range;
      EEPROM.writeLong(EEPROM_rangeS2, range2);
      return true;
      }
    }
    
    bool setRange3(int valor_range){
      if(valor_range < 1) return false;

      else{
      range3 = valor_range;
      EEPROM.writeLong(EEPROM_rangeS3, range3);
      return true;
      }
    }

};

  ADC4a20 adc;

/*****************************************************************************************************************************/
class Gemini {
  private:
  int n_serie, intervaloDeLeituras, ultimo_minuto;

  public:
    
    void begin(){
      n_serie = EEPROM.readLong(EEPROM_id);
      intervaloDeLeituras = EEPROM.read(EEPROM_intervalo);
    } 
    
    bool tempoAcabou(){
        long tempoAtual = millis();
        if(tempoAtual > tempoLimite) return true;
        else return false;  
    }

    
    bool horaDeAcrodar(){
      rtc.read();
      if (rtc.getMinuto() != ultimo_minuto){
        Serial.println(".");
        ultimo_minuto = rtc.getMinuto();

        if( rtc.getMinuto() == intervaloDeLeituras   ||rtc.getMinuto() == intervaloDeLeituras*2 
          ||rtc.getMinuto() == intervaloDeLeituras*3 ||rtc.getMinuto() == intervaloDeLeituras*4 
          ||rtc.getMinuto() == intervaloDeLeituras*5 ||rtc.getMinuto() == intervaloDeLeituras*6
          ||rtc.getMinuto() == intervaloDeLeituras*7 ||rtc.getMinuto() == intervaloDeLeituras*8 
          ||rtc.getMinuto() == intervaloDeLeituras*9 ||rtc.getMinuto() == intervaloDeLeituras*10
          ||rtc.getMinuto() == intervaloDeLeituras*11||rtc.getMinuto() == intervaloDeLeituras*12)return true;
          
        else return false;
      }
    }




    int getId(){return n_serie;}

    bool setId(int valor_id){
      if(valor_id > 999 || valor_id < 0 ) return false;
      else{
        n_serie = valor_id;
        EEPROM.writeLong(EEPROM_id, n_serie);
        return true;
      }
    }


};

  Gemini gemini;




