#include "Funcoes.h"

#define SerialTelemetria Serial2

class Relogio
{
private:
  int segundo = 0, minuto = 0, hora = 0, diadasemana = 0, dia = 0, mes = 0, ano = 0;
  byte zero = 0x00;
  byte h = 255;

  byte ConverteParaBCD(byte val)
  {
    // Converte o número de decimal para BCD
    return ((val / 10 * 16) + (val % 10));
  }

  byte ConverteparaDecimal(byte val)
  {
    // Converte de BCD para decimal
    return ((val / 16 * 10) + (val % 16));
  }

public:
  void begin()
  {
    pinMode(pwr_RTC, OUTPUT);
  }

  void set(byte D, byte M, byte A, byte h, byte m)
  {

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

  void read()
  {
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

  byte getDia() { return dia; }
  byte getMes() { return mes; }
  byte getAno() { return ano; }
  byte getHora() { return hora; }
  byte getMinuto() { return minuto; }
  byte getSegundo() { return segundo; }
};

Relogio rtc;

class MemoriaFlash
{
private:
  long enderecoAtual;

public:
  long getEnderecoAtual() { return enderecoAtual; }

  void begin(bool printa = 0)
  {
    digitalWrite(pwr_memoria, HIGH);
    delay(200);

    flash.begin();
    debugln((String) "eeprom: " + EEPROM.readLong(EEPROM_enderecoAtual));
    enderecoAtual = EEPROM.readLong(EEPROM_enderecoAtual);
    enderecoAtual = enderecoAtual * bitsPorLeitura;

    if (printa)
    {
      Serial.println((String) "Leituras: " + (enderecoAtual / bitsPorLeitura));
      Serial.println((String) "Leituras já lidas: " + (EEPROM.readLong(EEPROM_ultima_leitura) / bitsPorLeitura));
      Serial.print((String) "Leituras novas: " + ((enderecoAtual - EEPROM.readLong(EEPROM_ultima_leitura)) / bitsPorLeitura));
    }

    digitalWrite(pwr_memoria, LOW);
  }

  void write(String info)
  {
    digitalWrite(pwr_memoria, HIGH);
    delay(10);

    flash.writeStr(enderecoAtual, info);

    if (DEBUG)
    {
      String lido_da_flash = "";
      flash.readStr(enderecoAtual, lido_da_flash);
      Serial.println((String)lido_da_flash + " salvo no endereco: " + enderecoAtual);
    }

    enderecoAtual += bitsPorLeitura;
    long enderecoConvertido = enderecoAtual / bitsPorLeitura;
    EEPROM.writeLong(EEPROM_enderecoAtual, enderecoConvertido);
    EEPROM.writeLong(EEPROM_interrupts, 0);

    digitalWrite(pwr_memoria, LOW);
  }

  void encheMemoria()
  {
    digitalWrite(pwr_memoria, HIGH);
    delay(10);

    String fill = "";
    Serial.println("Criando dados na memoria...");
    for (int i = 0; i < 10000; i++)
    {
      debugln(".");
      fill = "";
      fill.concat("0000000000000000000000000");
      if (i < 10000)
        fill.concat(0);
      if (i < 1000)
        fill.concat(0);
      if (i < 100)
        fill.concat(0);
      if (i < 10)
        fill.concat(0);
      fill.concat(i);

      flash.writeStr(enderecoAtual, fill);
      enderecoAtual += bitsPorLeitura;
      long int enderecoConvertido = enderecoAtual / bitsPorLeitura;
      EEPROM.writeLong(EEPROM_enderecoAtual, enderecoConvertido);
      if (enderecoAtual >= 99840)
        break;
    }

    Serial.println("Memorias Criadas");

    digitalWrite(pwr_memoria, LOW);
  }

  void clean()
  {
    digitalWrite(pwr_memoria, HIGH);
    delay(10);

    Serial.println("Limpando memoria...");
    flash.eraseChip();
    EEPROM.writeLong(EEPROM_enderecoAtual, 0);
    EEPROM.writeLong(EEPROM_interrupts, 0);
    Serial.println("Memoria limpa!");
    debugln((String) "EEPROM: " + EEPROM.readLong(EEPROM_enderecoAtual));
    debugln((String) "Interrupts: " + EEPROM.readLong(EEPROM_interrupts));
    enderecoAtual = 0;
    EEPROM.writeLong(EEPROM_ultima_leitura, 0);

    digitalWrite(pwr_memoria, LOW);
  }

  String read(long quantidadeDeLeituras = 0)
  { // Lê todos os dados se nenhum parâmetro for informado. Se uma quantidade foi informada, lê aquela quantidade de leituras
    digitalWrite(pwr_memoria, HIGH);
    debugln("Memória ligada");
    delay(5000);
    quantidadeDeLeituras = quantidadeDeLeituras * bitsPorLeitura;
    long inicioLeituras = enderecoAtual - quantidadeDeLeituras;
    if (inicioLeituras < 0 || inicioLeituras > enderecoAtual)
      Serial.println("Quantidade invalida");

    else if (quantidadeDeLeituras == 0)
    { // Acontece se nenhum parâmetro foi passado. Lê a memória inteira
      Serial.println("#start");
      // Serial.println((String) "config" + );

      delay(100);
      String outputString = "";

      for (long i = 0; i < enderecoAtual; i += bitsPorLeitura)
      {
        flash.readStr(i, outputString);
        Serial.println((String) "==> " + i + ": " + id + outputString);
      }

      Serial.println("#end");
    }

    else
    {
      Serial.println((String) "Lendo as ultimas " + quantidadeDeLeituras + " leituras...");
      Serial.println("#start");
      delay(100);
      String outputString = "";
      String dados = "";
      rtc.read();
      dados.concat((String) "leituras=configId" + id + ";");
      dados.concat((String) "Data:" + rtc.getDia() + "/" + rtc.getMes() + " Hora:" + rtc.getHora() + ":" + rtc.getMinuto() + ";");

      for (long i = inicioLeituras; i < enderecoAtual; i += bitsPorLeitura)
      {
        flash.readStr(i, outputString);
        Serial.println((String) "==> " + i + ": " + id + outputString);
        dados.concat(outputString);
        dados.concat(";");
      }

      Serial.println("#end");
      delay(1000);
      return dados;
    }

    digitalWrite(pwr_memoria, LOW);
    delay(1000);
  }

  void readNew()
  {
    digitalWrite(pwr_memoria, HIGH);
    delay(100);

    Serial.println("#start");
    String outputString = "";
    long inicio = EEPROM.readLong(EEPROM_ultima_leitura);

    for (long i = inicio; i < enderecoAtual; i += bitsPorLeitura)
    {
      flash.readStr(i, outputString);
      Serial.println((String) "==>" + id + outputString);
    }
    Serial.println("#end");
    EEPROM.writeLong(EEPROM_ultima_leitura, enderecoAtual);

    digitalWrite(pwr_memoria, LOW);
  }
};

MemoriaFlash memoria;

class ADC4a20
{
private:
  long sensor0, sensor1, sensor2, sensor3;
  long range0, range1, range2, range3;
  long rangeDecimal0, rangeDecimal1, rangeDecimal2, rangeDecimal3;
  int mili0, mili1, mili2, mili3;

public:
  void begin()
  {
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

  void read(bool delay_leitura = 1)
  {
    digitalWrite(pwr_stepUp, HIGH);
    digitalWrite(pwr_ADC, HIGH);
    if (delay_leitura)
      delay(1500);
    else
      delay(100);

    mili0 = adcOriginal.read4to20mA(0) * 1000;
    mili1 = adcOriginal.read4to20mA(1) * 1000;
    mili2 = adcOriginal.read4to20mA(2) * 1000;
    mili3 = adcOriginal.read4to20mA(3) * 1000;

    debugln((String) "mAs: " + mili0 + " " + mili1 + " " + mili2 + " " + mili3);

    sensor0 = map(mili0, 3700, 20000, 0, (range0 * 100));
    sensor1 = map(mili1, 3700, 20000, 0, (range1 * 100));
    sensor2 = map(mili2, 3700, 20000, 0, (range2 * 100));
    sensor3 = map(mili3, 3700, 20000, 0, (range3 * 100));
    if (sensor0 < 0)
      sensor0 = 98765;
    if (sensor1 < 0)
      sensor1 = 98765;
    if (sensor2 < 0)
      sensor2 = 98765;
    if (sensor3 < 0)
      sensor3 = 98765;

    debugln((String) "Sensores: " + sensor0 + " " + sensor1 + " " + sensor2 + " " + sensor3);

    digitalWrite(pwr_stepUp, LOW);
    digitalWrite(pwr_ADC, LOW);
  }

  long getSensor0() { return sensor0; }
  long getSensor1() { return sensor1; }
  long getSensor2() { return sensor2; }
  long getSensor3() { return sensor3; }

  long getMili0() { return mili0; }
  long getMili1() { return mili1; }
  long getMili2() { return mili2; }
  long getMili3() { return mili3; }

  long getRange0() { return range0; }
  long getRange1() { return range1; }
  long getRange2() { return range2; }
  long getRange3() { return range3; }

  bool setRange0(int valor_range)
  {
    if (valor_range < 1)
      return false;

    else
    {
      range0 = valor_range;
      EEPROM.writeLong(EEPROM_rangeS0, range0);
      return true;
    }
  }

  bool setRange1(int valor_range)
  {
    if (valor_range < 1)
      return false;

    else
    {
      range1 = valor_range;
      EEPROM.writeLong(EEPROM_rangeS1, range1);
      return true;
    }
  }

  bool setRange2(int valor_range)
  {
    if (valor_range < 1)
      return false;

    else
    {
      range2 = valor_range;
      EEPROM.writeLong(EEPROM_rangeS2, range2);
      return true;
    }
  }

  bool setRange3(int valor_range)
  {
    if (valor_range < 1)
      return false;

    else
    {
      range3 = valor_range;
      EEPROM.writeLong(EEPROM_rangeS3, range3);
      return true;
    }
  }
};

ADC4a20 adc;

class Interrupt
{
public:
  void begin()
  {
    pinMode(interruptPin, INPUT_PULLUP);
    pinMode(interruptPin2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruptPin), interrupt1, LOW);
    attachInterrupt(digitalPinToInterrupt(interruptPin2), interrupt2, FALLING);
  }

  void ativa()
  {
    attachInterrupt(digitalPinToInterrupt(interruptPin), interrupt1, LOW);
    attachInterrupt(digitalPinToInterrupt(interruptPin2), interrupt2, FALLING);
  }

  void desativa()
  {
    detachInterrupt(digitalPinToInterrupt(interruptPin));
    detachInterrupt(digitalPinToInterrupt(interruptPin2));
  }

  void botao() // Acende os leds quando o botão é pressionado
  {
    debugln("Botão");
    int1Acionado = false;
    adc.read(0);
    digitalWrite(ledPower, HIGH);
    if (adc.getMili0() > 300)
      digitalWrite(ledSensor1, HIGH);
    if (adc.getMili1() > 300)
      digitalWrite(ledSensor2, HIGH);
    if (adc.getMili2() > 300)
      digitalWrite(ledSensor3, HIGH);
    if (rtc.getAno() == 0 || rtc.getAno() > 80)
    {
      for (int i = 0; i < 4; i++)
      {
        digitalWrite(ledStatus, HIGH);
        delay(250);
        digitalWrite(ledStatus, LOW);
        delay(250);
      }
      digitalWrite(ledStatus, HIGH);
      delay(500);
    }
    else
    {
      digitalWrite(ledStatus, HIGH);
      delay(3000);
    }

    digitalWrite(ledSensor1, LOW);
    digitalWrite(ledSensor2, LOW);
    digitalWrite(ledSensor3, LOW);
    digitalWrite(ledStatus, LOW);
    digitalWrite(ledPower, LOW);

    attachInterrupt(digitalPinToInterrupt(interruptPin), interrupt1, LOW);
    attachInterrupt(digitalPinToInterrupt(interruptPin2), interrupt2, FALLING);
  }

  void pulso()
  {
    delay(100);
    debugln("Pulso");
    int2Acionado = false;
    int contagem_pulsos = EEPROM.readLong(EEPROM_interrupts);
    contagem_pulsos++;
    debug("Contagem de pulsso atual = ");
    debugln(contagem_pulsos);
    EEPROM.writeLong(EEPROM_interrupts, contagem_pulsos);
    delay(250);
    attachInterrupt(digitalPinToInterrupt(interruptPin), interrupt1, LOW);
    attachInterrupt(digitalPinToInterrupt(interruptPin2), interrupt2, FALLING);
  }

  void verifica()
  {
    if (int1Acionado == true)
      botao();
    else if (int2Acionado == true)
      pulso();
  }
};

Interrupt interrupt;

class Gemini
{
private:
  int n_serie, intervaloDeLeituras, ultimo_minuto;

public:
  void begin()
  {
    n_serie = EEPROM.readLong(EEPROM_id);
    intervaloDeLeituras = EEPROM.read(EEPROM_intervalo);
    if (intervaloDeLeituras != 5 && intervaloDeLeituras != 10 && intervaloDeLeituras != 15 && intervaloDeLeituras != 20 && intervaloDeLeituras != 30 && intervaloDeLeituras != 60)
    {
      debugln("Intervalo definido para o padrao");
      intervaloDeLeituras = intervaloDeLeiturasPadrao;
    }

    interrupt.begin();
  }

  void dormir(int t)
  {
    debugln("dormir");
    delay(5);
    t = t / 2;
    for (int i = 0; i <= t; i++)
    {
      LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
      interrupt.verifica();
    }
  }

  bool tempoAcabou()
  { // Retorna true se o tempo limite já tiver acabado
    long tempoAtual = millis();
    if (tempoAtual > tempoLimite)
      return true;
    else
      return false;
  }

  void horaDeAcordar()
  { // Retorna true se for hora de fazer uma leitura
    delay(50);
    rtc.read();
    if (rtc.getMinuto() != ultimo_minuto)
    {
      debug(".");
      ultimo_minuto = rtc.getMinuto();
      debugln(ultimo_minuto);
      debugln((String) "Intervalo: " + intervaloDeLeituras);

      if (ultimo_minuto == intervaloDeLeituras || ultimo_minuto == (intervaloDeLeituras * 2) || ultimo_minuto == (intervaloDeLeituras * 3) || ultimo_minuto == (intervaloDeLeituras * 4) || ultimo_minuto == (intervaloDeLeituras * 5) || ultimo_minuto == (intervaloDeLeituras * 6) || ultimo_minuto == (intervaloDeLeituras * 7) || ultimo_minuto == (intervaloDeLeituras * 8) || ultimo_minuto == (intervaloDeLeituras * 9) || ultimo_minuto == (intervaloDeLeituras * 10) || ultimo_minuto == (intervaloDeLeituras * 11) || ultimo_minuto == 0)
      {
        deveAcordar = true;
      }
      else
      {
        deveAcordar = false;
      }
    }
  }

  void acordar()
  {

    debug("Acordei: ");
    debugln(rtc.getMinuto());
    adc.read();
    memoria.write(criaString());
  }

  String criaString()
  {
    String dados = "";
    if (rtc.getDia() < 10)
      dados.concat(0);
    dados.concat(rtc.getDia());

    if (rtc.getMes() < 10)
      dados.concat(0);
    dados.concat(rtc.getMes());

    if (rtc.getAno() < 10)
      dados.concat(0);
    dados.concat(rtc.getAno());

    if (rtc.getHora() < 10)
      dados.concat(0);
    dados.concat(rtc.getHora());

    if (rtc.getMinuto() < 10)
      dados.concat(0);
    dados.concat(rtc.getMinuto());

    if (adc.getSensor0() < 10000)
      dados.concat(0);
    if (adc.getSensor0() < 1000)
      dados.concat(0);
    if (adc.getSensor0() < 100)
      dados.concat(0);
    if (adc.getSensor0() < 10)
      dados.concat(0);
    dados.concat(adc.getSensor0());

    if (adc.getSensor1() < 10000)
      dados.concat(0);
    if (adc.getSensor1() < 1000)
      dados.concat(0);
    if (adc.getSensor1() < 100)
      dados.concat(0);
    if (adc.getSensor1() < 10)
      dados.concat(0);
    dados.concat(adc.getSensor1());

    if (adc.getSensor2() < 10000)
      dados.concat(0);
    if (adc.getSensor2() < 1000)
      dados.concat(0);
    if (adc.getSensor2() < 100)
      dados.concat(0);
    if (adc.getSensor2() < 10)
      dados.concat(0);
    dados.concat(adc.getSensor2());

    int pulsos = EEPROM.readLong(EEPROM_interrupts);
    if (pulsos < 100)
      dados.concat(0);
    if (pulsos < 10)
      dados.concat(0);
    dados.concat(pulsos);

    return dados;
  }

  int getIntervalo()
  {
    intervaloDeLeituras = EEPROM.read(EEPROM_intervalo);
    return intervaloDeLeituras;
  }

  int getId() { return n_serie; }

  bool setId(int valor_id)
  {

    if (valor_id > 999 || valor_id < 0)
      return false;
    else
    {
      n_serie = valor_id;
      EEPROM.writeLong(EEPROM_id, n_serie);
      return true;
    }
  }

  bool setIntervalo(int intervalo)
  {
    if (intervalo != 5 && intervalo != 10 && intervalo != 15 && intervalo != 20 && intervalo != 30 && intervalo != 60)
      intervalo = intervaloDeLeiturasPadrao;
    if (intervalo < 5)
      intervalo = intervaloDeLeiturasPadrao;
    EEPROM.write(EEPROM_intervalo, intervalo);
    return true;
  }
};

Gemini gemini;

class Telemetria
{

private:
  bool envioSucesso = false, erro = false;

  String TeleRead(int tempo = 85)
  {
    String incoming = "";
    for (int i = 0; i < tempo * 100; i++)
    {
      delay(10);
      if (SerialTelemetria.available())
      {
        incoming = SerialTelemetria.readString();
        return incoming;
        break;
      }
    }
  }

  void VerificaInicio()

  {
    String incoming = "";
    for (int i = 0; i < 1500; i++)
    {
      delay(10);
      if (SerialTelemetria.available())
      {
        incoming = SerialTelemetria.readString();
        if (incoming == "\r\nAPP RDY\r\n")
        {

          debug("Entrada correta:");
          debugln(incoming);
          break;
        }
      }
    }
    debugln("Saiu do inicio");
  }

  bool EnvioDeuCerto()
  {

    String retorno = TeleRead();
    String trecho = "+QHTTPPOST: 0,200";
    String trechoErro = "+QHTTPPOST: 0,500";
    if (retorno.indexOf(trecho) != -1)
    {
      debugln("Deu certo o do ChatGPT!!");
      debug("O retorno é: ");
      debugln(retorno);
      Serial.println("Dados enviados com sucesso!");
      return true;
    }
    else if (retorno.indexOf("OK") != -1)
    {
      debug("Deu um ok simples:");
      debugln(retorno);
      retorno = TeleRead();
      if (retorno.indexOf(trecho) != -1)
      {
        debugln("Deu certo o do ChatGPT com ok separado!!");
        debug("O retorno é: ");
        debugln(retorno);
        Serial.println("Dados enviados com sucesso!");
        return true;
      }
      else if (retorno.indexOf(trechoErro) != -1)
      {
        debugln("Deu 500. Erro no servidor");
        debug("O retorno é: ");
        debugln(retorno);
        return false;
      }
      else
      {
        debug("Deu errado o ok separado: ");
        debugln(retorno);
        return false;
      }
    }
    else
    {
      debug("Deu errado chatGPT: ");
      debugln(retorno);
      return false;
    }
  }

  bool enviaDadosContinua(String incoming = "", String esperado = " ")
  {
    if (esperado == incoming)
    {

      debugln();
      debugln("Entrada ok");
      debug("Entrada:");
      debugln(incoming);
      return true;
    }
    else
    {
      debug("Erro: ");
      debugln(incoming);

      incoming = TeleRead(2);
      debug("Tentando de novo: ");
      debugln(incoming);

      if (esperado == incoming)
      {

        debugln();
        debugln("Entrada ok");
        debug("Entrada:");
        debugln(incoming);
        return true;
      }
      else
      {
        incoming = TeleRead(2);
        debug("Tentando de novo 2: ");
        debugln(incoming);

        if (esperado == incoming)
        {

          debugln();
          debugln("Entrada ok");
          debug("Entrada:");
          debugln(incoming);
          return true;
        }
        else
        {
          debugln();
          debugln("Entrada não ok XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
          debug("Entrada:");
          debugln(incoming);
          erro = true;
          return false;
        }
      }
    }
  }

public:
  void
  begin()
  {
    SerialTelemetria.begin(9600);
    pinMode(rst_TLM, OUTPUT);
    pinMode(pwr_TLM, OUTPUT);
    digitalWrite(rst_TLM, HIGH);
  }

  void comunicaTelemetria()
  {

    digitalWrite(rst_TLM, HIGH);
    digitalWrite(pwr_TLM, LOW);
    delay(2000);
    digitalWrite(pwr_TLM, HIGH);
    Serial.println("Telemetria Ligada");
    delay(2000);
    digitalWrite(rst_TLM, LOW);
    delay(250);
    digitalWrite(rst_TLM, HIGH);
    Serial.println("Telemetria iniciada");

    while (true)
    {

      if (Serial.available())
      {
        Serial2.write(Serial.read());
      }

      if (Serial2.available())
      {
        Serial.write(Serial2.read());
      }
    }
  }

  bool enviaDados(String dados)
  {
    debugln("Enviando dados...");
    debugln(dados);
    debugln(dados.length());
    digitalWrite(rst_TLM, HIGH);
    delay(200);
    envioSucesso = false;
    byte tentativas = 0;
    while (!envioSucesso)
    {
      tentativas++;
      if (tentativas > 3)
      {
        debugln("Erro no envio.");
        break;
      }

      digitalWrite(pwr_TLM, LOW);
      debugln("Telemetria desligada");
      delay(1000);
      digitalWrite(pwr_TLM, HIGH);
      debugln("Telemetria ligada");
      delay(1000);
      digitalWrite(rst_TLM, LOW);
      delay(250);
      digitalWrite(rst_TLM, HIGH);
      delay(2000);
      debugln("Telemetria Reiniciada");
      VerificaInicio();
      delay(2000);
      SerialTelemetria.println("AT+QHTTPURL=54,80");
      debugln("**AT+QHTTPURL=54,80");
      if (!enviaDadosContinua(TeleRead(), "AT+QHTTPURL=54,80\r\r\nCONNECT\r\n"))
        continue;
      SerialTelemetria.println(url);
      debug("**");
      debugln(url);
      if (!enviaDadosContinua(TeleRead(), "\r\nOK\r\n"))
        continue;
      SerialTelemetria.println((String) "AT+QHTTPPOST=" + dados.length() + ",80,80");
      debugln((String) "**AT+QHTTPPOST=" + dados.length() + ",80,80");
      if (!enviaDadosContinua(TeleRead(), (String) "AT+QHTTPPOST=" + dados.length() + ",80,80\r"))
        continue;
      delay(2000);
      if (!enviaDadosContinua(TeleRead(), "\r\nCONNECT\r\n"))
        continue;
      debugln("Mandando dados para telemetria...");
      debugln(dados);
      SerialTelemetria.println(dados);
      debugln("Dados enviados para a telemetria!");
      delay(1000);
      envioSucesso = EnvioDeuCerto();
    }

    digitalWrite(pwr_TLM, LOW);
  }

  bool enviaTeste()
  {

    String dados = "leituras=configId421;Data:70/70 Hora:70:70;000000000000000000000000001512;000000000000000000000000001513;000000000000000000000000001514;000000000000000000000000001515;000000000000000000000000001516;000000000000000000000000001517;000000000000000000000000001518;000000000000000000000000001519;000000000000000000000000001520;000000000000000000000000001521;000000000000000000000000001522;000000000000000000000000001523;000000000000000000000000001524;000000000000000000000000001525;000000000000000000000000001526;000000000000000000000000001527;000000000000000000000000001528;000000000000000000000000001529;000000000000000000000000001530;000000000000000000000000001531;000000000000000000000000001532;000000000000000000000000001533;000000000000000000000000001534;000000000000000000000000001535;000000000000000000000000001536;000000000000000000000000001537;000000000000000000000000001538;000000000000000000000000001539;000000000000000000000000001540;000000000000000000000000001541;000000000000000000000000001542;000000000000000000000000001543;000000000000000000000000001544;000000000000000000000000001545;000000000000000000000000001546;000000000000000000000000001547;000000000000000000000000001548;000000000000000000000000001549;000000000000000000000000001550;000000000000000000000000001551;000000000000000000000000001552;000000000000000000000000001553;000000000000000000000000001554;000000000000000000000000001555;000000000000000000000000001556;000000000000000000000000001557;000000000000000000000000001558;000000000000000000000000001559;";

    debugln("Enviando dados...");
    debugln(dados);
    debugln(dados.length());
    digitalWrite(rst_TLM, HIGH);
    delay(200);
    envioSucesso = false;
    byte tentativas = 0;
    while (!envioSucesso)
    {
      tentativas++;
      if (tentativas > 3)
      {
        debugln("Erro no envio.");
        break;
      }

      digitalWrite(pwr_TLM, LOW);
      debugln("Telemetria desligada");
      delay(1000);
      digitalWrite(pwr_TLM, HIGH);
      debugln("Telemetria ligada");
      delay(1000);
      digitalWrite(rst_TLM, LOW);
      delay(250);
      digitalWrite(rst_TLM, HIGH);
      delay(2000);
      debugln("Telemetria Reiniciada");
      VerificaInicio();
      delay(2000);
      SerialTelemetria.println("AT+QHTTPURL=54,80");
      debugln("**AT+QHTTPURL=54,80");
      if (!enviaDadosContinua(TeleRead(), "AT+QHTTPURL=54,80\r\r\nCONNECT\r\n"))
        continue;
      SerialTelemetria.println(url);
      debug("**");
      debugln(url);
      if (!enviaDadosContinua(TeleRead(), "\r\nOK\r\n"))
        continue;
      SerialTelemetria.println((String) "AT+QHTTPPOST=" + dados.length() + ",80,80");
      debugln((String) "**AT+QHTTPPOST=" + dados.length() + ",80,80");
      if (!enviaDadosContinua(TeleRead(), (String) "AT+QHTTPPOST=" + dados.length() + ",80,80\r"))
        continue;
      delay(2000);
      if (!enviaDadosContinua(TeleRead(), "\r\nCONNECT\r\n"))
        continue;
      debugln("Mandando dados para telemetria...");
      debugln(dados);
      SerialTelemetria.println(dados);
      debugln("Dados enviados para a telemetria!");
      delay(1000);
      envioSucesso = EnvioDeuCerto();
    }

    digitalWrite(pwr_TLM, LOW);
  }
};

Telemetria telemetria;

class COM
{

public:
  String serial(int espera = 0)
  { // Espera um comando na serial pelo tempo informado em segundos no parametro e o retorna. Se o tempo não for informado, espera até que receba algo pela Serial.
    espera = espera * 100;
    String incoming = "vazio";
    long idle = millis();
    if (espera == 0)
      while (!Serial.available() && !gemini.tempoAcabou())
      {
        delay(1);
        long tempo_esperando = millis() - idle;
        if (tempo_esperando > 5000)
        {
          Serial.println("idle");
          idle = millis();
        }
      }

    else
      for (int i = 0; i < espera; i++)
      {
        delay(10);
        if (Serial.available())
          break;
      }

    if (Serial.available())
      incoming = Serial.readString();
    else
      Serial.println("Nenhum comando recebido");
    debugln(incoming);
    return incoming;
  }

  bool comando(int t = 0)
  {
    String entrada = serial(t);

    if (entrada.substring(0, 4) == "CFre")
    {
      String recebe = entrada.substring(4, 14);
      int regulaDia = recebe.substring(0, 2).toInt();
      int regulaMes = recebe.substring(2, 4).toInt();
      int regulaAno = recebe.substring(4, 6).toInt();
      int regulaHora = recebe.substring(6, 8).toInt();
      int regulaMinuto = recebe.substring(8, 10).toInt();
      rtc.set(regulaDia, regulaMes, regulaAno, regulaHora, regulaMinuto);
      Serial.println("CF:ok");
    }

    if (entrada.substring(0, 4) == "CFin")
    {
      String sub = entrada.substring(4, 6);
      int intervalo = sub.toInt();
      debugln(intervalo);
      if (gemini.setIntervalo(intervalo))
        Serial.println("CF:ok");
    }

    else if (entrada == "limpar" || entrada == "limpar\r\n")
      memoria.clean();
    else if (entrada == "ler" || entrada == "ler\r\n" || entrada == "lertudo" || entrada == "lertudo\r\n")
      memoria.read(100);
    else if (entrada == "testaMemoria" || entrada == "testaMemoria\r\n")
      memoria.encheMemoria();
    else if (entrada == "relogio" || entrada == "relogio\r\n")
      regulaRelogio();
    else if (entrada == "range" || entrada == "range\r\n")
      defineRange();
    else if (entrada == "id" || entrada == "id\r\n")
      defineId();
    else if (entrada == "intervalo" || entrada == "intervalo\r\n")
      defineIntervalo();
    else if (entrada == "STATUS" || entrada == "STATUS\r\n" || entrada == "status" || entrada == "status\r\n")
      Status();
    else if (entrada == "telemetria" || entrada == "telemetria\r\n")
      telemetria.comunicaTelemetria();
    else if (entrada == "enviaTeste" || entrada == "enviaTeste\r\n")
    {
      telemetria.enviaTeste();
    }
    else if (entrada == "enviaDados" || entrada == "enviaDados\r\n")
    {
      debugln("Lendo memoria...");
      delay(2000);
      String dadosTelemetria = memoria.read(48);
      debugln("Ativando Telemetria...");
      delay(2000);
      telemetria.enviaDados(dadosTelemetria);
    }
    else if (entrada == "config" || entrada == "config\r\n")
      config();
    else if (entrada == "testa" || entrada == "testa\r\n")
      testa();
    else
      Serial.println("Comando invalido");

    if (entrada == "fim" || entrada == "fim\r\n" || entrada == "FIM" || entrada == "FIM\r\n" || entrada == "CFfi" || entrada == "CFfi\r\n")
    {
      Serial.println("FIM:ok");
      return false;
    }

    else
      return true;
  }

  void regulaRelogio()
  {
    Serial.println("Regulando relogio. Digite a data e a hora(DDMMAAHHMM):");
    String recebe = serial();
    int regulaDia = recebe.substring(0, 2).toInt();
    int regulaMes = recebe.substring(2, 4).toInt();
    int regulaAno = recebe.substring(4, 6).toInt();
    int regulaHora = recebe.substring(6, 8).toInt();
    int regulaMinuto = recebe.substring(8, 10).toInt();

    rtc.set(regulaDia, regulaMes, regulaAno, regulaHora, regulaMinuto);

    Serial.println("Relogio configuurado");
  }

  void defineId()
  {
    Serial.println("Informe o ID...");
    String id_recebido = serial();
    id = id_recebido.toInt();
    gemini.setId(id);
    Serial.println("Id configurado");
  }

  void defineRange()
  {
    Serial.println("Qual o range do sensor 0?");
    String entrada = serial();
    if (adc.setRange0(entrada.toInt()))
      Serial.println("Range do sensor0 configurado.");
    Serial.println("Qual o range do sensor 1?");
    entrada = serial();
    if (adc.setRange1(entrada.toInt()))
      Serial.println("Range do sensor1 configurado.");
    Serial.println("Qual o range do sensor 2?");
    entrada = serial();
    if (adc.setRange2(entrada.toInt()))
      Serial.println("Range do sensor2 configurado.");
    Serial.println("Qual o range do sensor 3?");
    entrada = serial();
    if (adc.setRange3(entrada.toInt()))
      Serial.println("Range do sensor3 configurado.");
  }

  void defineIntervalo()
  {
    Serial.println("Informe o intervalo...");
    String int_recebido = serial();
    int intervalo = int_recebido.toInt();
    debugln(intervalo);
    gemini.setIntervalo(intervalo);
    Serial.print("Intervalo configurado para: ");
    Serial.println(gemini.getIntervalo());
  }

  void Status()
  {
    Serial.println("#intStatus");
    rtc.read();
    adc.read();

    // DATA
    Serial.print("DATA=");
    if (rtc.getDia() < 10)
      Serial.print("0");
    Serial.print(rtc.getDia());
    Serial.print("/");
    if (rtc.getMes() < 10)
      Serial.print("0");
    Serial.print(rtc.getMes());
    Serial.print("/");
    if (rtc.getAno() < 10)
      Serial.print("0");
    Serial.println(rtc.getAno());

    // HORA
    Serial.print("HORA=");
    if (rtc.getHora() < 10)
      Serial.print("0");
    Serial.print(rtc.getHora());
    Serial.print(":");
    if (rtc.getMinuto() < 10)
      Serial.print("0");
    Serial.print(rtc.getMinuto());
    Serial.println("");

    // SENSORES
    Serial.print("Sensor1:");
    Serial.print("Range=");
    Serial.print(adc.getRange0());
    Serial.print(";Pressao atual=");
    Serial.print(adc.getSensor0() / 100.0);
    Serial.println(";");

    Serial.print("Sensor2:");
    Serial.print("Range=");
    Serial.print(adc.getRange1());
    Serial.print(";Pressao atual=");
    Serial.print(adc.getSensor1() / 100.0);
    Serial.println(";");

    // INTERVALO
    Serial.print("Intervalo:");
    Serial.println(gemini.getIntervalo());
    Serial.println("#endStatus");
  }

  void config()
  {
    long Idle = millis();
    Serial.println("Modo de configuracao. Aguardando comando...");
    while (!gemini.tempoAcabou())
    {
      if (!comando())
        break;
      long tempo_esperando = millis() - Idle;
      if (tempo_esperando > 10000)
      {
        Serial.println("idle");
        Idle = millis();
      }
    }
  }

  void testa()
  {
    while (!gemini.tempoAcabou())
    {
      comando(1);
      Serial.println("Modo de testes");
      rtc.read();
      adc.read();

      // DATA
      Serial.print("DATA=");
      if (rtc.getDia() < 10)
        Serial.print("0");
      Serial.print(rtc.getDia());
      Serial.print("/");
      if (rtc.getMes() < 10)
        Serial.print("0");
      Serial.print(rtc.getMes());
      Serial.print("/");
      if (rtc.getAno() < 10)
        Serial.print("0");
      Serial.println(rtc.getAno());

      // HORA
      Serial.print("HORA=");
      if (rtc.getHora() < 10)
        Serial.print("0");
      Serial.print(rtc.getHora());
      Serial.print(":");
      if (rtc.getMinuto() < 10)
        Serial.print("0");
      Serial.print(rtc.getMinuto());
      Serial.println("");

      // SENSORES
      Serial.print("Sensor0: ");
      Serial.print(adc.getSensor0() / 100.0);
      Serial.print(";  Milis:");
      Serial.println(adc.getMili0());

      Serial.print("Senso1: ");
      Serial.print(adc.getSensor1() / 100.0);
      Serial.print(";  Milis:");
      Serial.println(adc.getMili1());

      Serial.print("Sensor2: ");
      Serial.print(adc.getSensor2() / 100.0);
      Serial.print(";  Milis:");
      Serial.println(adc.getMili2());

      Serial.print("Sensor3: ");
      Serial.print(adc.getSensor3() / 100.0);
      Serial.print(";  Milis:");
      Serial.println(adc.getMili3());

      memoria.write(gemini.criaString());
      Serial.println();
      Serial.println();
    }
  }
};

COM com;
