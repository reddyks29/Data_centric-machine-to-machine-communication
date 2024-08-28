const int sensorOut = A0;
int pressure; 

void setup(){

  Serial.begin(9600);       // Begin serial communication
}

void loop(){
  pressure = analogRead(sensorOut);         //Read and store analog value from Force Sensitive Resistance
  Serial.println(pressure);                 //Send PWM value to led connected at pin D0
  delay(1000);                               //Small delay
}