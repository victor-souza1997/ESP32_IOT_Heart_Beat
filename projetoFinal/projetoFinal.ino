

#define Pin35 35

int input;//variavel que armazenara a entrada de audio

void setup() {
  pinMode(Pin35, input);
  Serial.begin(115200);
}

void loop() {
  input = analogRead(Pin35);
  float s = input*3.3/4096;
  Serial.println(s);
  // put your main code here, to run repeatedly:

}
