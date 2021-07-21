

#define Pin35 35

#define amos_per_sec 1000 //quantidade de amostras coletadas por segundo

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

void setup() { //funcao de configuracao
  pinMode(Pin35, input);//configurando pino de amotragem para o pino Pin35
  Serial.begin(115200);//iniciando comunicao serial
  
  //inicializando timer para interrupt
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000/amos_per_sec, true);
  timerAlarmEnable(timer);
}

void loop() {//funcao loop
   
   if (interruptCounter > 0){//caso o contador de interrupt seja maior que 0
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);
    totalInterruptCounter++;
    
   input = analogRead(Pin35);
   float s = input*3.3/4096;
   Serial.println(s);//Serial.print("An interrupt as occurred. Total number: ");
    //Serial.println(totalInterruptCounter);
  }
  
  
}
