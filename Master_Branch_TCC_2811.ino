//BIBLIOTECAS

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <WiFi.h>
#include <Servo.h>
#include "time.h"

//CREDENCIAIS DA INTERFACE

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME  "diegosantos"
#define AIO_KEY  "KEYNÃOREVELADA"

//CONEXÃO AO MQTT

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe PrimeiroRemedio = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/PrimeiroRemedio");
Adafruit_MQTT_Subscribe SegundoRemedio = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/SegundoRemedio");
Adafruit_MQTT_Subscribe QuantidadePrimeiro = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/QuantidadePrimeiro");
Adafruit_MQTT_Subscribe QuantidadeSegundo = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/QuantidadeSegundo");
Adafruit_MQTT_Subscribe IntervaloPrimeiro = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/IntervaloPrimeiro");
Adafruit_MQTT_Subscribe IntervaloSegundo = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/IntervaloSegundo");
Adafruit_MQTT_Subscribe Salvar = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/a");
Adafruit_MQTT_Subscribe ArmazenamentoPrimeiro = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ArmazenamentoPrimeiro");
Adafruit_MQTT_Subscribe ArmazenamentoSegundo = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ArmazenamentoSegundo");
boolean MQTT_connect();

boolean MQTT_connect() {  int8_t ret; if (mqtt.connected()) {    return true; }  uint8_t retries = 3;  while ((ret = mqtt.connect()) != 0) { mqtt.disconnect(); delay(2000);  retries--;if (retries == 0) { return false; }} return true;}

//CRIAÇÃO DOS SERVO MOTORES

Servo servo1;
Servo servo2;

//VARIÁVEIS PARA RECEBER OS VALORES DA INTERFACE

char * horarioPrimeiro;
char * quant_primeiro;
char * interv_primeiro;
char * horarioSegundo;
char * quant_segundo;
char * interv_segundo;
char * salvar_itens;
int horarioPrimeiroInt;
int quant_primeiroInt;
int interv_primeiroInt;
int horarioSegundoInt;
int quant_segundoInt;
int interv_segundoInt;

char * armazenamentoPrimeiro;
char * armazenamentoSegundo;
int armazenamentoPrimeiroInt;
int armazenamentoSegundoInt;

//OUTRAS VARIÁVEIS

int horaExata;

int calculoPrimeiro;
int calculoSegundo;

int etapa1 = 0;
int etapa2 = 0;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -10800;
const int   daylightOffset_sec = 0;

void setup(){
  Serial.begin(9600);

  //CONEXÃO WIFI

  WiFi.disconnect();
  delay(3000);
  Serial.println("Começando a conexão");
  WiFi.begin("NOMEDAREDE","SENHADAREDE");
  while ((!(WiFi.status() == WL_CONNECTED))){
    delay(500);
    Serial.println("...conectando");

  }
  Serial.println("Conectado!");

  //DECLARAÇÃO DOS FEEDS

  mqtt.subscribe(&PrimeiroRemedio);
  mqtt.subscribe(&SegundoRemedio);
  mqtt.subscribe(&QuantidadePrimeiro);
  mqtt.subscribe(&QuantidadeSegundo);
  mqtt.subscribe(&IntervaloPrimeiro);
  mqtt.subscribe(&IntervaloSegundo);
  mqtt.subscribe(&Salvar);
  mqtt.subscribe(&ArmazenamentoPrimeiro);
  mqtt.subscribe(&ArmazenamentoSegundo);

  //DECLARAÇÃO DOS SERVOS

  servo1.attach(13);
  servo2.attach(12);

  //CONFIGURAÇÃO DE TEMPO NTP

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);  
}


void loop()
{
  //RECEPÇÃO NTP
  
  struct tm timeinfo;
  
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  int horarioExato = atoi(timeHour);
  Serial.print("Hora: ");
  Serial.println(horarioExato);
  
  //CONDIÇÃO DE RECEPÇÃO DE DADOS DO PROTOCOLO MQTT
  
    if (MQTT_connect()) {
      Adafruit_MQTT_Subscribe *subscription_name;
      while ((subscription_name = mqtt.readSubscription(5000))) {
        if (subscription_name == &Salvar) {
          
          Serial.println("Valores Salvos:");

          //RECEPÇÃO DO VALOR LIDO E APRESENTAÇÃO NO MONITOR SERIAL
          
          horarioPrimeiro = ((char *)PrimeiroRemedio.lastread);
          Serial.println(horarioPrimeiro);
          quant_primeiro = ((char *)QuantidadePrimeiro.lastread); 
          Serial.println(quant_primeiro);
          interv_primeiro = ((char *)IntervaloPrimeiro.lastread); 
          Serial.println(interv_primeiro);
          armazenamentoPrimeiro = ((char *)ArmazenamentoPrimeiro.lastread); 
          Serial.println(armazenamentoPrimeiro);

          horarioSegundo = ((char *)SegundoRemedio.lastread);
          Serial.println(horarioSegundo);
          quant_segundo = ((char *)QuantidadeSegundo.lastread); 
          Serial.println(quant_segundo);
          interv_segundo = ((char *)IntervaloSegundo.lastread); 
          Serial.println(interv_segundo);
          armazenamentoSegundo = ((char *)ArmazenamentoSegundo.lastread); 
          Serial.println(armazenamentoSegundo);

          //TRASFORMAÇÃO EM VALORES 'INT' PARA COMPARAÇÃO EM 'IF'
          
          horarioPrimeiroInt = atoi(horarioPrimeiro);
          quant_primeiroInt = atoi(quant_primeiro);
          interv_primeiroInt = atoi(interv_primeiro);
          armazenamentoPrimeiroInt = atoi(armazenamentoPrimeiro);
          
          horarioSegundoInt = atoi(horarioSegundo);
          quant_segundoInt = atoi(quant_segundo);
          interv_segundoInt = atoi(interv_segundo);
          armazenamentoSegundoInt = atoi(armazenamentoSegundo);

          //CALCULO DO HORARIOS DOS REMÉDIOS QUE DEVEM SER TOMADOS

          int calculaHorarios1[armazenamentoPrimeiroInt];
          int calculaHorarios2[armazenamentoSegundoInt];
          etapa1 = 0;
          etapa2 = 0;

          calculaHorarios1[0] = horarioPrimeiroInt;
          calculoPrimeiro = horarioPrimeiroInt;
          calculaHorarios2[0] = horarioSegundoInt;
          calculoSegundo = horarioSegundoInt;
          
          Serial.println("Point(A):");
          Serial.println(armazenamentoPrimeiroInt);
          Serial.println(armazenamentoSegundoInt);
          
          for(int i=1; i<armazenamentoPrimeiroInt;i++){
            Serial.print("Remédio(1) ");
            Serial.print(i+1);
            Serial.print(": ");
            Serial.println(calculaHorarios1[i-1]);
            calculoPrimeiro = calculoPrimeiro + interv_primeiroInt;
          if(calculoPrimeiro > 23){
            calculoPrimeiro = calculoPrimeiro - 24;
            calculaHorarios1[i] = calculoPrimeiro;
          }else{
            calculaHorarios1[i] = calculoPrimeiro;
          }
          }
          
          for(int x=1; x<armazenamentoSegundoInt; x++){
            Serial.print("Remédio(2) ");
            Serial.print(x+1);
            Serial.print(": ");
            Serial.println(calculaHorarios2[x-1]);
            calculoSegundo = calculoSegundo + interv_segundoInt;
          if(calculoSegundo > 23){
            calculoSegundo = calculoSegundo - 24;
            calculaHorarios2[x] = calculoSegundo;
          }else{
            calculaHorarios2[x] = calculoSegundo;
          }
          }

          //DEFINIÇÃO DO HORÁRIO DE LIBERAÇÃO
  
          if(calculaHorarios1[etapa1] == horarioExato){
            etapa1++;
            
            Serial.println("Remédio 1 liberado");

            int while1 = quant_primeiroInt;

            while(while1 > 0){
              while1--;
              
              for(int ser1 = 0; ser1 <90; ser1++){
                 servo1.write(ser1);
                 delay(20); 
              } 
              for(int ser1 = 90; ser1 >0; ser1--){
                 servo1.write(ser1);
                 delay(20); 
              }
              servo1.write(0);      
            }
            delay(1000);
          }
          
          if(calculaHorarios2[etapa2] == horarioExato){
            etapa2++;
            
            Serial.println("Remédio 2 liberado");

            int while2 = quant_segundoInt;

            while(while2 > 0){
              while2--;  
              
              for(int ser2 = 0; ser2 <90; ser2++){
                 servo2.write(ser2);
                 delay(20); 
              } 
              for(int ser2 = 90; ser2 >0; ser2--){
                 servo2.write(ser2);
                 delay(20); 
              }
              servo2.write(0);
            }
            delay(1000);    
          }
          
          
          }       

    }
  }    
}
