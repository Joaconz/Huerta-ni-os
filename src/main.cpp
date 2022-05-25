#include <Arduino.h>
#include <time.h>


struct riego{           
  bool estado=1;  
  int intervalo=1;         
  int tiempo=20;   
};
int n=1;
int IByte;
struct riego lista[5];


void valvulasEstado(int i, bool b){
  digitalWrite(i+n, b);

  Serial.print("valvula: ");
  Serial.print(i+1);
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


int esperarCaracter(){
  while (!(Serial.available()>0)){
    delay(10);}
  return Serial.read();
}


bool sameTime(int i){
  if(((minute()%lista[i].intervalo)==0) && (second()<=lista[i].tiempo)) return 1;
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
  Serial.println("elegir canal");
  int incomingByte = esperarCaracter()-'1' ;
  changeBool(lista[incomingByte].estado);

  Serial.print("estado cambiado. Valvula: ");
  Serial.print(incomingByte);
  Serial.print("  Estado: ");
  Serial.println(lista[incomingByte].estado);
}


void modificarIntervalo(){ //numeros de 1 caracter
  Serial.println("elegir canal");
  int canal = esperarCaracter()-'1';
  Serial.println("periodo: ");
  int periodo = esperarCaracter()-'0';
  Serial.println("Tiempo en alto");
  int t_on = esperarCaracter()-'0';
  lista[canal].tiempo=t_on;
  lista[canal].intervalo=periodo;
}


void setup() {
  Serial.begin(9600);
  //setPines();
  lista[4].intervalo=1;
  lista[4].tiempo=10;
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
    case 'B'://modificar intervalo
      modificarIntervalo();
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

