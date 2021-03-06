
#include <WiFi.h> /* Header para uso das funcionalidades de wi-fi do ESP32 */
#include <PubSubClient.h>
#include <stdlib.h>
#include <arduinoFFT.h>

#define SAMPLES 1024              //Must be a power of 2
#define SAMPLING_FREQUENCY amos_per_sec   //Hz. Determines maximum frequency that can be analysed by the FFT.
#define samplePin 35//pino de amostragem do sinal
#define ansPIN 13//pino que acende quando paciente estiver ruim
#define amos_per_sec 512 //quantidade de amostras coletadas por segundo
#define user "p1"
// configurando FFT
arduinoFFT FFT = arduinoFFT();      
                         
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
#define TOPIC_SUBSCRIBE "amostrar/ans/p2"
/* id mqtt (para identificação de sessão) */
/* IMPORTANTE: este deve ser único no broker (ou seja, 
               se um client MQTT tentar entrar com o mesmo 
               id de outro já conectado ao broker, o broker 
               irá fechar a conexão de um deles).
*/
#define ID_MQTT  "SENDER1"            //Informe um ID unico e seu. Caso sejam usados IDs repetidos a ultima conexão irá sobrepor a anterior. 
PubSubClient MQTT(wifiClient);        // Instancia o Cliente MQTT passando o objeto espClient
//**********************************************************

//inicializando variaveis
int input;//variavel que armazenara a entrada de audio
// variaveis responsaveis pela FFT
int j = 0;
double vRealADC[SAMPLES];//armazena valor amostrado
double vReal[SAMPLES];//armazena valor amostrado e posteriormente a resposta da DFT
double vImag[SAMPLES];//armazena valor complexo da fft

//variaveis do contador de interrupcao
volatile int interruptCounter;//variavel onde é contado 
int totalInterruptCounter;//variavel que armazena o numero de interrupcoes
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;



//Declaração das Funções
void mantemConexoes();  //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
void conectaWiFi();     //Faz conexão com WiFi
void conectaMQTT();     //Faz conexão com Broker MQTT
void enviaValores();     //
void IRAM_ATTR onTimer(); //construtor da funcao interrupt 
void recebePacote(char* topic, byte* payload, unsigned int length);//construtor da funcao que recebe pacote

void setup() 
{ //funcao de configuracao
  pinMode(samplePin, input);//configurando pino de amotragem para o pino samplePin
  pinMode(ansPIN, OUTPUT);
  Serial.begin(115200);//iniciando comunicao serial
  conectaWiFi();//conectando ao WIFI
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);//inicializando server MQQT   
  MQTT.setCallback(recebePacote);
  //inicializando timer para interrupt
  timer = timerBegin(0, 80, true); //inicializar timer
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000/amos_per_sec, true);
  timerAlarmEnable(timer);
}

void loop() 
{   
   //enviaValores();
   mantemConexoes();
   MQTT.loop();
   if(j == SAMPLES ){//ultima amostragem 
    //etapa para calcular a DFT
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
  
    float temp = 0;//variavel auxiliar para encontrar maior componente de frequencia
    int pos; //armazenar posicao da frequencia fundamental
    for(int i=0; i<(SAMPLES/2); i++)//encontrar maior componente harmonica  
    {
      //Serial.print((i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES, 1);//Serial.print("\t");//Serial.println(vReal[i]);    
      if(vReal[i]>temp) {temp = vReal[i]; pos = (i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES;}
    }
    //Serial.println(temp);
    //Serial.println(pos);
    j = 0;
    char tempstring[10];
    dtostrf(pos,1,1,tempstring);
    //Serial.println(tempstring);//Serial.print("An interrupt as occurred. Total number: ");
    MQTT.publish(TOPIC_PUBLISH, user); //mandar usuario o qual o dado medido pertence
    MQTT.publish(TOPIC_PUBLISH, tempstring);//mandar dado medido para servidor central
  }
  if (interruptCounter > 0)//caso o contador de interrupt seja maior que 0
  { 
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);
    //totalInterruptCounter++;//contando numero de interrupcoes 
    input = analogRead(samplePin);//amostrando valor da porta smamplePin
    float s = input*3.3/4096 - 1.57;//convertendo valor binario num valor array e removendo nivel Dc
    vRealADC[j] = s;//""""
    vReal[j] = s;//vetor que armazena valores reais para o calculo da DFT
    vImag[j] = 0;//vetor que armazena valores complexos
    j++;//incrementando posicao do vetor de amostragem
    Serial.println(s);
  }

}
void mantemConexoes() 
{
    if (!MQTT.connected()){conectaMQTT();}   
    conectaWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}
void conectaWiFi() 
{
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("Aguarde!");
  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI  
  while (WiFi.status() != WL_CONNECTED) 
  {
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
    if (MQTT.connect(ID_MQTT)) 
    {
      //Serial.println("Conectado ao Broker com sucesso!");
      MQTT.subscribe(TOPIC_SUBSCRIBE);
    } 
    else 
    {
      //Serial.println("Nao foi possivel se conectar ao broker.");
      //Serial.println("Nova tentatica de conexao em 10s");
      delay(10000);
    }
  }
}
void IRAM_ATTR onTimer()//funcao de interrupt  
{
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
}
void recebePacote(char* topic, byte* payload, unsigned int length) //funcao responsavel por alterar valor do setTemp
{
  String msg;
  //obtem a string do payload recebido
  for(int i = 0; i < length; i++) 
  {
     char c = (char)payload[i];
     msg += c;
  }
  /*receber mensagem do estado do LED*/
  if(msg = 'L') digitalWrite(ansPIN, HIGH);
  Serial.println(msg);  
}
