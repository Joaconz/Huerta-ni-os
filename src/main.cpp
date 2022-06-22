#include <Arduino.h>
#include <TimeLib.h>
#include <math.h>
#include <Wire.h>
#include <DS3231.h>

DS3231 reloj;

int PINES_DE_CANALES[5] = {13, 12, 11, 10, 9}; //pinOut que le corresponde a cada canal

struct riego{           
  int hora;                 //riegos por dia      
  int tiempo_encendido;     //duracion de cada riego
};

/*
* los tiempos de riegos se guardaran en la variable valvula, que es una array de array de la estructura riego
* de la forma valvulas[i][j], el valor i corresponde al canal y la j sera una array de riego, que contedra cada
* hora de riego y su duracion (.hora, .tiempo_encendido)
*/
struct riego valvulas[5][5];

void esperar_entero(int &i)
{
  while (!(Serial.available()>0))
  {
    delay(10);}
  i =  int(Serial.read())-'0';
}

void esperar_entero_n_digitos(int &i, int n)
{
  i = 0;
  for(int j = n-1; j>=0; j--)
  {
    esperar_entero(n);
    i += n*pow(10,j);
  }
}

void agregar_riego() //ingresar horaio en formato 24h 00:00 (horas, minutos)
{
  int canal;
  esperar_entero(canal);
  Serial.println(canal);
  int h, t, r = 0;
  while (valvulas[0][r].tiempo_encendido != 0 && r < 5)
    r++;
  esperar_entero_n_digitos(h, 2);
  valvulas[canal][r].hora = h;
  Serial.print("  -  ");
  Serial.print(h);
  esperar_entero_n_digitos(t, 2);
  valvulas[canal][r].tiempo_encendido = t;
  Serial.print(":");
  Serial.print(t);
  Serial.println(" ");
  Serial.print(r);
  Serial.println(" ");
}

void eliminar_riegos()
{
  int r;
  esperar_entero(r);
  digitalWrite(PINES_DE_CANALES[r], 0);
  for (int j = 0; j < 5; j++)
    valvulas[r][j].tiempo_encendido = 0;
}

void setear_hora()
{
  int h , m;
  esperar_entero_n_digitos(h, 2);
  esperar_entero_n_digitos(m, 2);
  reloj.setMinute(h);
  reloj.setSecond(m);
}
/**********************************************************************************************************/
void setup(){
  Serial.begin(9600);
  Wire.begin();
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}
/**********************************************************************************************************/
void loop() 
{
  /************************************************************/
  /*                            Menu                          */
  /************************************************************/
  if (Serial.available() > 0)  //leer y eligir una opcion 
  { 
    int Byte = Serial.read();  // leer un byte de entrada
    switch (Byte){
    case 'A':                  //agregar riego
      delay(100);
      Serial.print("   A    ");
      agregar_riego();
      
      break;
    case 'B':                  //eliminar riegos de un canal 
      delay(100);
      eliminar_riegos();
      break;
    case 'C':                  //setear hora
      delay(100);
      setear_hora();
      break;
    default:
      break;
    }
  }
  /************************************************************/
  /*                   Activar valvulas                       */
  /************************************************************/
  for (int i = 0; i<5; i++)   //recorrer vavulas
  {
    for (int j = 0; j<5; j++) //recorres riegos
    { 
      if(reloj.getMinute() == valvulas[i][j].hora)
        if (valvulas[i][j].tiempo_encendido != 0)
          digitalWrite(PINES_DE_CANALES[i], (valvulas[i][j].tiempo_encendido > reloj.getSecond()));
    }
  }
  Serial.print(reloj.getMinute());
  Serial.print(":");
  Serial.println(reloj.getSecond());
  delay(500);
}