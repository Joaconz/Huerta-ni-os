#include <Arduino.h>
#include <math.h>
#include <Wire.h>
#include <DS3231.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>

const byte rowsCount = 4;
const byte columsCount = 4;
char keys[rowsCount][columsCount] = {
   { '1','2','3', 'A' },
   { '4','5','6', 'B' },
   { '7','8','9', 'C' },
   { '*','0','#', 'D' }
};
const byte rowPins[rowsCount] = {12, 11, 10 ,9};
const byte columnPins[columsCount] = {8, 7, 6,5 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, columnPins, rowsCount, columsCount);

//Crear el objeto lcd  dirección  0x3F y 16 columnas x 2 filas
LiquidCrystal_I2C lcd(0x27,16,2);  

DS3231 reloj;

int PINES_DE_CANALES[5] = {13, 12, 11, 10, 9}; //pinOut que le corresponde a cada canal

struct riego{           
  int hora;                 //hora de riego      
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
  int car; 
  do
  {
    car = keypad.getKey()-'0';
  } while (!(car>=0 && car <=9));
  i = car;
}

void escribir_menu()
{
  lcd.clear();
  if((reloj.getSecond()/2)%2){
    lcd.setCursor(0,0);
    lcd.print("A agregar riego");
    lcd.setCursor(0,1);
    lcd.print("B eliminar riego");
    }
  else 
  { 
    lcd.setCursor(0,0);
    lcd.print("C setear hora");
    lcd.setCursor(0,1);
    lcd.print("D ver riegos");
  }
}

void esperar_entero_n_digitos(int &i, int dig)
{
  i = 0;
  int n;
  for(int j = dig-1; j>=0; j--)
  {
    esperar_entero(n);
    i += n*pow(10,j);
  }
}

void imprimir_entero(int i, int c, int f)
{
  lcd.setCursor(c, f);
  int d1, d2;
  if (i>9)
  {
    d1=i-i%10;
    d2=i-d1;
  }
  else 
  {
    d1=0;
    d2=i;
  }
  lcd.print(d1);
  lcd.setCursor(c+1,f);
  lcd.print(d2);
}

void esperar_entero_n_digitos_imprimiendo(int &i, int dig, int c, int f)
{
  i = 0;
  int n;
  for(int j = dig-1; j>=0; j--)
  {
    esperar_entero(n);
    lcd.setCursor(c, f);
    lcd.print(n);
    c++;
    i += n*pow(10,j);
  }
}
//ingresar horaio en formato 24h 00:00 (horas, minutos)
void agregar_riego() 
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("elegir valvula");
  int canal;
  esperar_entero(canal);
    lcd.setCursor(0, 1);
    lcd.println(canal);
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("horario: (hh:mm)");
  int h, t, r = 0;
  while (valvulas[canal][r].tiempo_encendido != 0 && r < 5)
    r++;
  esperar_entero_n_digitos_imprimiendo(h, 2, 0, 1);
  valvulas[canal][r].hora = h;

  lcd.setCursor(2,1);
  lcd.print(":");
  esperar_entero_n_digitos_imprimiendo(t, 2, 3, 1);
  valvulas[canal][r].tiempo_encendido = t;
  delay(1000);
  actualizar_EEprom_desde_riego(true);
}

void eliminar_riegos()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("eliminar riego");
    lcd.setCursor(0, 1);
    lcd.print("valvula: ");
  int r;
  esperar_entero_n_digitos_imprimiendo(r ,1, 8, 1);
  digitalWrite(PINES_DE_CANALES[r], 0);
  for (int j = 0; j < 5; j++)
    valvulas[r][j].tiempo_encendido = 0;
  delay(1000);
  actualizar_EEprom_desde_riego(false);
}

void setear_hora()
{
  int h , m;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("hora actual: ");
  esperar_entero_n_digitos_imprimiendo(h, 2, 0, 1);
    lcd.setCursor(2, 1);
    lcd.print(":");
  esperar_entero_n_digitos_imprimiendo(m, 2, 3, 1);
  reloj.setMinute(h);
  reloj.setSecond(m);
}

void ver_riegos()
{
  int canal;
  lcd.clear();
  lcd.print("ver valvula");
  esperar_entero_n_digitos_imprimiendo(canal, 1, 0,0);
  delay(1000);
  for (int i = 0; i < 5; i++)
  {
    if (valvulas[canal][i].tiempo_encendido)
    {    
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("horario: ");
      imprimir_entero(valvulas[canal][i].hora, 14, 0);
      lcd.setCursor(0, 1);
      lcd.print("tiempo riego: ");
      imprimir_entero(valvulas[canal][i].tiempo_encendido, 14, 1);
      delay(2000);
    }
  }
}

void actualizar_EEprom_desde_riego(bool act)
{
  if (act)
    EEPROM.put(1,riego);
  else
    EEPROM.update(1, riego);
}

void actualiazar_riego_desde_eeprom()
{
  EEPROM.get(1, riego);
}

/**********************************************************************************************************/

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  pinMode(13, OUTPUT);
  lcd.init();
  lcd.backlight();
  actualiazar_riego_desde_eeprom();
}

/**********************************************************************************************************/

void loop() 
{
  //************************************************************//
  //                             Menu                           //
  //************************************************************//

  //1 agregar riego
  //2 elimnar riego
  //3 poner horario
  //4 visulizar riegos
  escribir_menu();
    char car =keypad.getKey();  // leer un byte de entrada
    switch (car){
    case 'A':                  //agregar riego
      delay(100);
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
    case 'D':
      delay(100);
      ver_riegos();                                 //visualizar riegos
      break;
    default:
      break;
    }
  //************************************************************//
  //                    Activar valvulas                        //
  //************************************************************//
  for (int i = 0; i<5; i++)   //recorrer vavulas
    for (int j = 0; j<5; j++) //recorres riegos
      if(reloj.getMinute() == valvulas[i][j].hora)
        if (valvulas[i][j].tiempo_encendido != 0)
          digitalWrite(PINES_DE_CANALES[i], (valvulas[i][j].tiempo_encendido >= reloj.getSecond()));
  //Serial.print(reloj.getMinute());
  //Serial.print(":");
  //Serial.println(reloj.getSecond());
  delay(500);
}