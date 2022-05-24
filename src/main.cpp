#include <Arduino.h>
#include <time.h>

struct riego{           
  bool estado=1;  
  int intervalo=2;         
  int tiempo=20;   
};

int n=1;

struct riego lista;

void valvulasEstado(int i, bool b){
  digitalWrite(i+n, b);
  Serial.print("estado = ");
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
  if((!(minute()%lista.intervalo)) && (second()<=lista.tiempo)) return 1;
  else return 0;
}
void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void setup() {
  Serial.begin(9600);
  //setPines();
}

void loop() {
  for (int i = 0; i < 1; i++){
    if(sameTime(i) && lista.estado) valvulasEstado(i,1);
    else valvulasEstado(i,0);
  }
  Serial.print(minute());
  printDigits(second());
  Serial.println();
  delay(1000);
}

