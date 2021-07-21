
#include <WiFi.h> /* Header para uso das funcionalidades de wi-fi do ESP32 */
#include <PubSubClient.h>
#include <stdlib.h>


#define samplePin 35
#define amos_per_sec 100 //quantidade de amostras coletadas por segundo


//****************** WiFi ******************************
/* SSID / nome da rede WI-FI que deseja se conectar */
const char* SSID = "AP_405"; 
/*  Senha da rede WI-FI que deseja se conectar */
const char* PASSWORD = "mercuryHg100";
WiFiClient wifiClient;                

//*****************************************************

//*************** MQTT Server *************************
/* URL do broker MQTT que deseja utilizar */
const char* BROKER_MQTT = "broker.hivemq.com"; 
/* Porta do Broker MQTT */
int BROKER_PORT = 1883;

/* Tópico MQTT para envio de informações do ESP32 para broker MQTT */
#define TOPIC_PUBLISH   "amostrar/heart_beat"  
#define TOPIC_SUBSCRIBE "amostrar/ans"
/* id mqtt (para identificação de sessão) */
/* IMPORTANTE: este deve ser único no broker (ou seja, 
               se um client MQTT tentar entrar com o mesmo 
               id de outro já conectado ao broker, o broker 
               irá fechar a conexão de um deles).
*/
#define ID_MQTT  "SENDER"            //Informe um ID unico e seu. Caso sejam usados IDs repetidos a ultima conexão irá sobrepor a anterior. 
PubSubClient MQTT(wifiClient);        // Instancia o Cliente MQTT passando o objeto espClient
//**********************************************************



int input;//variavel que armazenara a entrada de audio
volatile int interruptCounter;
int totalInterruptCounter;
 
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
 
void IRAM_ATTR onTimer() { //funcao de interrupt
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
 
}

//Declaração das Funções
void mantemConexoes();  //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
void conectaWiFi();     //Faz conexão com WiFi
void conectaMQTT();     //Faz conexão com Broker MQTT
void enviaValores();     //
//void recebePacote(char* topic, byte* payload, unsigned int length);



void setup() { //funcao de configuracao
  pinMode(samplePin, input);//configurando pino de amotragem para o pino samplePin
  Serial.begin(115200);//iniciando comunicao serial
  
  conectaWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);   
  
  
  //inicializando timer para interrupt
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000/amos_per_sec, true);
  timerAlarmEnable(timer);
}

void loop() {//funcao loop
   mantemConexoes();
   //enviaValores();
   MQTT.loop();
  
   if (interruptCounter > 0){//caso o contador de interrupt seja maior que 0
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);
    totalInterruptCounter++;
    
   input = analogRead(samplePin);
   float s = input*3.3/4096;
   char tempstring[6];
   dtostrf(s,3,1,tempstring);
   Serial.println(s);//Serial.print("An interrupt as occurred. Total number: ");
   MQTT.publish(TOPIC_PUBLISH, tempstring) ;
    //Serial.println(totalInterruptCounter);
  }

}

void mantemConexoes() {
    if (!MQTT.connected()) {
       conectaMQTT(); 
    }
    
    conectaWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

void conectaWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
     return;
  }
        
  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI  
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(SSID);  
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP()); 
}
void conectaMQTT() 
{ 
  while (!MQTT.connected()) 
  {
    //Serial.print("Conectando ao Broker MQTT: ");
    //Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT)) {
        //Serial.println("Conectado ao Broker com sucesso!");
        //MQTT.subscribe(TOPIC_SUBSCRIBE);
    } 
    else 
    {
      //Serial.println("Nao foi possivel se conectar ao broker.");
      //Serial.println("Nova tentatica de conexao em 10s");
      delay(10000);
    }
  }
}
