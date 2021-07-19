

#define Pin35 35

int input;//variavel que armazenara a entrada de audio

void setup() {
  pinMode(Pin35, input);
  Serial.begin(19200);
}

void loop() {
  input = analogRead(Pin35);
  Serial.println(input);
  // put your main code here, to run repeatedly:

}
