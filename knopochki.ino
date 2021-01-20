/* Пример кода для руководства по работе с матричной клавиатурой 3x4
   совместно с платой Arduino.
   Данный код выводит значение нажатой на клавиатуре кнопки в 
   монитор последовательного порта. 
 */
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

#include "Keypad.h"



int pinCS = 9;
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
}


// Если кнопка нажата, эта кнопка сохраняется в переменной keypressed.
// Если keypressed не равна NO_KEY, то выводим значение в последовательный порт.
void loop()
{
  char keypressed = kpd.getKey();
  if (keypressed != NO_KEY)
  { 
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
    }


    
    Serial.println(keypressed);
  }
}
