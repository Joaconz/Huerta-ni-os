#include <Arduino.h>
#include <time.h>

struct riego{           
  bool estado=1;  
  int intervalo=2;         
  int tiempo=20;   
};
int n=1;
int IByte;
struct riego lista[5];


void valvulasEstado(int i, bool b){
  digitalWrite(i+n, b);

  Serial.print("valvula: ");
  Serial.print(i);
  Serial.print("  estado: ");
  Serial.println(b);
}


int minute(){
  time_t now;
  time(&now);
  return (localtime(&now)->tm_min);
}


int second(){
  time_t now;
  time(&now);
  return (localtime(&now)->tm_sec);
}


bool sameTime(int i){
  if((!(minute()%lista[i].intervalo)) && (second()<=lista[i].tiempo)) return 1;
  else return 0;
}


void printDigits(int digits){
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


void changeBool(bool &a){
  if (a) a=0;
  else a=1;  
}


void modificarCanal(){
  while (!(Serial.available()>0)){
    delay(10);}
  int incomingByte= Serial.read()-'1' ;
  changeBool(lista[incomingByte].estado);

  Serial.print("estado cambiado. Valvula: ");
  Serial.print(incomingByte);
  Serial.print("  Estado: ");
  Serial.println(lista[incomingByte].estado);
}


void setup() {
  Serial.begin(9600);
  //setPines();
}


void loop() {

  if (Serial.available() > 0) {
    // leer sigiente byte
    IByte = Serial.read();
    switch (IByte){
    case 'A'://modificar estado de canal n
      delay(100);
      modificarCanal();
      break;
    default:
      break;
    }
  }

  for (int i = 0; i < 5; i++){
    if(sameTime(i) && lista[i].estado) valvulasEstado(i,1);
    else valvulasEstado(i,0);
  }
  delay(50);
}

