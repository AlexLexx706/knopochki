

/* Пример кода для руководства по работе с матричной клавиатурой 3x4
   совместно с платой Arduino.
   Данный код выводит значение нажатой на клавиатуре кнопки в 
   монитор последовательного порта. 
 */
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>


#include "Keypad.h"



int pinCS = 10;
int numberOfHorizontalDisplays = 2; // теперь у нас по-горизонтали 6 матриц
int numberOfVerticalDisplays = 2; // а по-вертикали, по-прежнему, одна
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

String tape = "Hellow word!"; // текст, который будет плыть
int wait = 30; // время между крайними перемещениями букв

int spacer = 1; // расстояние между буквами
int width = 5 + spacer; // размер шрифта





const byte Rows= 4; // количество строк на клавиатуре, 4
const byte Cols= 3; // количество столбцов на клавиатуре, 3

// определяем массив символов соответствующий распределению кнопок на клавиатуре:
char keymap[Rows][Cols]=
{
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

// соединения клавиатуры с выводами Arduino задаются так:
byte rPins[Rows]= {3,8,7,5}; // строки с 0 по 3
byte cPins[Cols]= {4,2,6};    // столбцы с 0 по 2

// команда для библиотеки клавиатуры
// инициализирует экземпляр класса Keypad
Keypad kpd= Keypad(makeKeymap(keymap), rPins, cPins, Rows, Cols);

void setup()
{
  
  Serial.begin(9600);  // инициализация монитора последовательного порта
    matrix.setIntensity(4); // яркость
    matrix.setRotation( 0, 1 );
    matrix.setRotation( 1, 1 );
    matrix.setRotation( 2, 1 );
    matrix.setRotation( 3, 1 );
    matrix.fillScreen(LOW);
    matrix.write();
    randomSeed(analogRead(0));
}
int flag = 0;
int timespeed = 300, flagtime = -1;
int x = 0, y = 6;
int ys[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int flagx = -1, flagy = -1;

// Если кнопка нажата, эта кнопка сохраняется в переменной keypressed.
// Если keypressed не равна NO_KEY, то выводим значение в последовательный порт.
void loop()
{
  
  char keypressed = kpd.getKey();
  if (keypressed != NO_KEY)
  {
    Serial.println(keypressed);
    Serial.println(flag);
    if(flag == 0){ 
        matrix.fillScreen(LOW);
        matrix.write();
        if(keypressed == '1'){
          matrix.drawCircle( 8, 8, 7, HIGH );
          matrix.write();
        }
        else if(keypressed == '2'){
          matrix.drawTriangle(2, 12, 8, 2, 16, 12, HIGH);
          matrix.write();
        }
        else if(keypressed == '3'){
          matrix.drawRect(2, 2, 14, 14, HIGH);
          matrix.write();
        }
        else if(keypressed == '0'){
          matrix.fillScreen(LOW);
          matrix.write();
          flag = 1;
          Serial.println(flag);
        }
    
        else if(keypressed == '4'){
          matrix.drawRoundRect(1, 5, 14, 8, 4, HIGH);
          matrix.write();
        }
    
        else if(keypressed == '5'){
          matrix.drawLine(5, 3, 11, 3, HIGH);
          matrix.drawLine(11, 3, 15, 8, HIGH);
          matrix.drawLine(15, 8, 11, 13, HIGH);
          matrix.drawLine(11, 13, 5, 13, HIGH);
          matrix.drawLine(5, 13, 1, 8, HIGH);
          matrix.drawLine(1, 8, 5, 3, HIGH);
          matrix.write();
        }
    
        else if(keypressed == '6'){
          matrix.drawRect(6, 0, 4, 16, HIGH);
          matrix.drawRect(0, 6, 16, 4, HIGH);
          matrix.fillRect(6, 0, 4, 16, HIGH);
          matrix.fillRect(0, 6, 16, 4, HIGH);
          matrix.write();
        }
    
        
    }
    else if(flag == 1){

      if(keypressed == '1'){
          while(1){
            matrix.fillScreen(LOW);
            matrix.write();
            if(x == 0 || x == 12)
              flagx*= -1;
            if(y == 0 || y == 12)
              flagy*= -1;    
            
            matrix.drawRect(x, y, 4, 4, HIGH);
            matrix.fillRect(x, y, 4, 4, HIGH);
            matrix.write();
            Serial.println(x);
            Serial.println(y);
            if(timespeed == 20 || timespeed == 320)
              flagtime*= -1;
            delay(timespeed);
            timespeed+= 10*flagtime;
            x+=flagx;
            y+=flagy;      
          }        
        }


      else if(keypressed == '2'){
          while(1){
            matrix.fillScreen(LOW);
            matrix.write();
            if(x == 0 || x == 11)
              flagx*= -1;
            if(y == 0 || y == 11)
              flagy*= -1;    
            
            matrix.drawCircle( x+2, y+2, 2, HIGH );
            matrix.fillCircle( x+2, y+2, 2, HIGH );
            matrix.write();
            Serial.println(x);
            Serial.println(y);
            if(timespeed == 20 || timespeed == 320)
              flagtime*= -1;
            delay(timespeed);
            timespeed+= 10*flagtime;
            x+=flagx;
            y+=flagy;      
          }        
        }
        
      else if(keypressed == '3'){
          
          while(1){
            matrix.fillScreen(LOW);
            matrix.drawPixel(random(15), random(15), HIGH);
            matrix.write();
            delay(20);
          }
      }
      else if(keypressed == '4'){
          int a = 7, side = 2;
          while(1){
            matrix.fillScreen(LOW);
            matrix.drawRect(a, a, side, side, HIGH);
            if(a != 7){
              matrix.drawRect(a+1, a+1, side-2, side-2, HIGH);
            }
            if(a < 6){
              matrix.drawRect(a+2, a+2, side-4, side-4, HIGH);
            }
            matrix.write();
            delay(80);
            a--;
            side+=2;
            if(a < -2){
              a = 7;
              side = 2;
            }
            
          }
       
       }
       else if(keypressed == '5'){
          int pos = 0, flagpos = 1;
          while(1){
            matrix.fillScreen(LOW);
            matrix.drawLine(pos, 0, pos, 15, HIGH);
            matrix.drawLine(0, pos, 15, pos, HIGH);
            matrix.drawLine(0, 15-pos, 15, 15-pos, HIGH);
            matrix.write();
            delay(100);
            pos+=flagpos;

            if(pos == 15 || pos == 0)
              flagpos*=-1;
            
          }
       
       }
       else if(keypressed == '0'){
          matrix.fillScreen(LOW);
          Serial.println("tak blet");
          matrix.write();
          flag = 0;
        }
    }
  }
    
}
