#define IR_Sensor 14 
#define LED LED_BUILTIN       

int IR;      
bool oned = false;       

void setup()
{
  Serial.begin(9600);
  pinMode(IR_Sensor, INPUT);   
  pinMode(LED, OUTPUT);       
}
void loop(){
  IR=digitalRead(IR_Sensor);  
  if(IR==LOW){               
    call_fun();   
  }
}
void call_fun(){
  while (digitalRead(IR_Sensor) == LOW){
    Serial.println("Hand on sensor");
  }
  if (oned == false){
    oned = true;
    digitalWrite(LED,HIGH);
  }
  else if (oned == true){
    oned = false;
    digitalWrite(LED,LOW);
  }
  return;
}