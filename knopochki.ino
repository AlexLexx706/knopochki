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
byte cPins[Cols]= {4,2,6};		// столбцы с 0 по 2

// команда для библиотеки клавиатуры
// инициализирует экземпляр класса Keypad
Keypad kpd= Keypad(makeKeymap(keymap), rPins, cPins, Rows, Cols);


#define EYE_BLINK_STEP_TIMEOUT 30
struct Eye {
	int s_x, s_y;
	int offset;
	int e_x, e_y;
	int state;

	unsigned long s_time;
	unsigned long blink_timeout;

	unsigned long es_time;
	unsigned long wait_move_timeout;
};

void setup() {
	Serial.begin(115200);	// инициализация монитора последовательного порта
	matrix.setIntensity(0); // яркость
	matrix.setRotation( 0, 1 );
	matrix.setRotation( 1, 1 );
	matrix.setRotation( 2, 1 );
	matrix.setRotation( 3, 1 );
	matrix.fillScreen(LOW);
	matrix.drawPixel(0, 15, HIGH);
	matrix.write();
	randomSeed(analogRead(0));
	matrix.setIntensity(3); // яркость
}

int flag = 0;
int timespeed = 300, flagtime = -1;
int x = 0, y = 6;
int ys[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int flagx = -1, flagy = -1;
char next_key = NO_KEY;


void rain() {
	//simple rain
	short data[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int i, j;
	unsigned long s_time, c_time = millis();

	while (1) {
		//1. update data
		for (i = 0; i < sizeof(data)/sizeof(data[0]); i++) {
			data[i] = data[i] << 1 | (random(2) ? 1 : 0);

			//2. draw collum
			for (j = 0; j < 16; j++) {
				matrix.drawPixel(15 - j, i, (1 << j) & data[i] ? HIGH : LOW);
			}
		}
		matrix.write();

		//wait for timeout and check for the keyboard pressed
		while (1) {
			if ((next_key = kpd.getKey()) != NO_KEY) {
				return;
			} else if ( (c_time = millis()) - s_time >= 1000) {
				s_time = c_time;
				break;
			}
		}
	}
}


void matrix_demo() {
	//simple rain
	typedef struct DataDesc {
		short data;
		unsigned long s_time;
		unsigned long timeout;
	};
	unsigned long s_time, c_time = millis();
	DataDesc desc[16];
	int i, j;
	DataDesc  *cur_desc;

	//init desc
	for (i = 0; i < sizeof(desc) / sizeof(desc[0]); i++) {
		desc[i].data = 0;
		desc[i].s_time = c_time;
		desc[i].timeout = random(4, 15) * 10;
	}

	while (1) {
		//1. update data
		c_time = millis();

		for (i = 0; i < sizeof(desc) / sizeof(desc[0]); i++) {
			cur_desc = &desc[i];

			if (c_time - cur_desc->s_time >= cur_desc->timeout) {
				cur_desc->s_time = c_time;
				cur_desc->data = cur_desc->data << 1 | (random(2) ? 1 : 0);
			}

			//2. draw collum
			for (j = 0; j < 16; j++) {
				matrix.drawPixel(15 - j, i, (1 << j) & cur_desc->data ? HIGH : LOW);
			}
		}
		matrix.write();

		//wait for timeout and check for the keyboard pressed
		if ((next_key = kpd.getKey()) != NO_KEY) {
			return;
		}
	}
}


void spiral() {
	int x = 0;
	int y = 0;
	int state = 0;
	int offset = 0;
	matrix.fillScreen(LOW);

	while ((next_key = kpd.getKey()) == NO_KEY) {
		matrix.drawPixel(x, y, HIGH);

		switch (state) {
			//move right
			case 0: {
				if (x + 1 > 15 - offset) {
					state = 1;
				} else {
					x++;
				}
				break;
			}
			//move down
			case 1: {
				if (y + 1 > 15 - offset) {
					state = 2;
				} else {
					y++;
				}
				break;
			}
			//move left
			case 2: {
				if (x - 1 < offset) {
					state = 3;
					offset++;
				} else {
					x--;
				}
				break;
			}
			//mode up
			case 3: {
				if (y - 1 < offset) {
					state = 0;
					//exit
					if (offset == 8) {
						x = 0;
						y = 0;
						state = 0;
						offset = 0;
						matrix.fillScreen(LOW);
					}
				} else {
					y--;
				}
				break;
			}
		}
		matrix.write();
		delay(100);
	}
}
// Если кнопка нажата, эта кнопка сохраняется в переменной keypressed.
// Если keypressed не равна NO_KEY, то выводим значение в последовательный порт.


void draw_eye(const Eye & eye) {
	//1. draw base
	if (eye.offset <= 0 ) matrix.drawLine(eye.s_x + 1, eye.s_y, eye.s_x + 4, eye.s_y, HIGH);
	if (eye.offset <= 1 ) matrix.drawLine(eye.s_x, eye.s_y + 1, eye.s_x + 5, eye.s_y + 1, HIGH);
	if (eye.offset <= 2 ) matrix.drawLine(eye.s_x, eye.s_y + 2, eye.s_x + 5, eye.s_y + 2, HIGH);
	if (eye.offset <= 3 ) matrix.drawLine(eye.s_x, eye.s_y + 3, eye.s_x + 5, eye.s_y + 3, HIGH);
	if (eye.offset <= 4 ) matrix.drawLine(eye.s_x, eye.s_y + 4, eye.s_x + 5, eye.s_y + 4, HIGH);
	if (eye.offset <= 5 ) matrix.drawLine(eye.s_x, eye.s_y + 5, eye.s_x + 5, eye.s_y + 5, HIGH);
	if (eye.offset <= 6 ) matrix.drawLine(eye.s_x, eye.s_y + 6, eye.s_x + 5, eye.s_y + 6, HIGH);
	if (eye.offset <= 7 ) matrix.drawLine(eye.s_x + 1, eye.s_y + 7, eye.s_x + 4, eye.s_y + 7, HIGH);

	//2. draw center
	matrix.drawLine(eye.s_x + eye.e_x + 2, eye.s_y + eye.e_y + 3, eye.s_x + eye.e_x + 3, eye.s_y + eye.e_y + 3, LOW);
	matrix.drawLine(eye.s_x + eye.e_x + 2, eye.s_y + eye.e_y + 4, eye.s_x + eye.e_x + 3, eye.s_y + eye.e_y + 4, LOW);
}

void draw_nose(int x, int y) {
	matrix.fillRect(x, y, 2, 2, HIGH);
}

void draw_mouth(int x, int y, char state) {
	switch (state) {
		case 0: {
			matrix.drawLine(x, y, x + 7, y, HIGH);
			matrix.drawLine(x + 2, y + 1, x + 5, y + 1, HIGH);
			matrix.drawLine(x + 3, y + 2, x + 4, y + 2, HIGH);
			break;
		}
		case 1: {
			matrix.drawLine(x, y + 1, x + 7, y + 1, HIGH);
			break;
		}
		case 2: {
			matrix.drawRect(x + 1, y, 6, 3, HIGH);
			break;
		}
		case 3: {
			matrix.drawLine(x, y, x + 7, y, HIGH);
			matrix.drawLine(x + 2, y + 1, x + 5, y + 1, HIGH);
			matrix.drawLine(x + 7, y, x + 7, y - 1, HIGH);
			break;
		}
		case 4: {
			matrix.drawLine(x, y + 1, x + 7, y + 1, HIGH);
			matrix.drawLine(x, y + 2, x + 7, y + 2, HIGH);
			break;
		}
		case 5: {
			matrix.drawLine(x, y, x + 7, y, HIGH);
			matrix.drawLine(x + 2, y + 1, x + 5, y + 1, HIGH);
			matrix.drawLine(x, y, x, y - 1, HIGH);
			break;
		}
	}
}

void process_eye(Eye & eye) {
	unsigned long c_time = millis();
	// Serial.print("state:");
	// Serial.print(eye.state);
	// Serial.print(" offset:");
	// Serial.print(eye.offset);
	// Serial.print(" blink_timeout:");
	// Serial.println(eye.blink_timeout);

	switch (eye.state) {
		//1. start blink
		case 0: {
			eye.state = 1;
			eye.s_time = c_time;
			break;
		}
		//2. move_down
		case 1: {
			if (c_time - eye.s_time >= EYE_BLINK_STEP_TIMEOUT) {
				eye.offset += 1;
				eye.s_time  = c_time;
			}
			// goto move up
			if (eye.offset >= 6) {
				eye.offset = 6;
				eye.state = 2;
			}
			break;
		}
		case 2: {
			if (c_time - eye.s_time >= EYE_BLINK_STEP_TIMEOUT) {
				eye.offset -= 1;
				eye.s_time  = c_time;
			}

			// goto wait state
			if (eye.offset <= 0) {
				eye.offset = 0;
				eye.state = 3;
				eye.blink_timeout = 500 * random(3, 30);
			}
			break;
		}
		case 3: {
			if (c_time - eye.s_time  >= eye.blink_timeout) {
				eye.offset = 0;
				eye.s_time  = c_time;
				eye.state = 0;
			}
			break;
		}
	}

	//move eye
	if (c_time - eye.es_time > eye.wait_move_timeout) {
		eye.e_x = random(-1, 2);
		eye.e_y = random(-1, 2);
		eye.wait_move_timeout = 500 * random(1, 20);
		eye.es_time = c_time;
	}
}

void eyes() {
	Eye l_e = {1, 0, 0, 1, 1, 0, 3000, 0, 3000};
	Eye r_e = {9, 0, 0, 1, 1, 0, 3000, 0, 3000};
	unsigned long s_time = millis();
	unsigned long c_time = s_time;
	char mouth_type = 0;
	unsigned long mouth_duration = 3000;
	
	while ((next_key = kpd.getKey()) == NO_KEY) {
		process_eye(l_e);
		r_e.offset = l_e.offset;
		r_e.e_x = l_e.e_x;
		r_e.e_y = l_e.e_y;

		matrix.fillScreen(LOW);
		draw_eye(l_e);
		draw_eye(r_e);
		draw_nose(7, 7);

		//update 
		draw_mouth(4, 10, mouth_type);

		c_time = millis();
		if (c_time - s_time > mouth_duration) {
			s_time = c_time;
			mouth_type = random(6);
			mouth_duration = random(1, 20) * 500;
		}
		matrix.write();
	}
}

void fig_reflection(char fig_type) {
	timespeed = 300;
	flagtime = -1;
	x = 0;
	y = 6;
	flagx = -1;
	flagy = -1;
	unsigned long s_time = millis();
	unsigned long c_time = s_time;

	while ((next_key = kpd.getKey()) == NO_KEY) {
		c_time = millis();

		//wait fro next step
		if (c_time - s_time >= timespeed) {
			s_time = c_time;
			matrix.fillScreen(LOW);
			matrix.write();

			if(x == 0 || x == 12) {
				flagx*= -1;
			}

			if(y == 0 || y == 12) {
				flagy*= -1;
			}

			//draw rect
			if (fig_type == 0) {
				matrix.drawRect(x, y, 4, 4, HIGH);
				matrix.fillRect(x, y, 4, 4, HIGH);
			//draw circle
			} else if (fig_type == 1) {
				matrix.drawCircle( x+2, y+2, 2, HIGH );
				matrix.fillCircle( x+2, y+2, 2, HIGH );
			}
			matrix.write();
			// Serial.println(x);
			// Serial.println(y);

			if (timespeed == 20 || timespeed == 320) {
				flagtime*= -1;
			}

			timespeed+= 10*flagtime;
			x+=flagx;
			y+=flagy;
		}
	}
}

void random_points() {
	int count = 0;
	bool exit = false;

	while (!exit) {
		matrix.fillScreen(LOW);

		while(count++ < 256) {
			matrix.drawPixel(random(16), random(16), HIGH);
			matrix.write();
			delay(20);

			if ((next_key = kpd.getKey()) != NO_KEY) {
				exit = true;
				break;
			}
		}
		count = 0;
	}

}

void growing_rect() {
	int a = 7, side = 2;
	unsigned long s_time = millis();
	unsigned long c_time = s_time;
	while ((next_key = kpd.getKey()) == NO_KEY) {
		c_time = millis();
		
		//wait for next step
		if (c_time - s_time >= 500) {
			s_time = c_time;
			matrix.fillScreen(LOW);
			matrix.drawRect(a, a, side, side, HIGH);
			if (a != 7) {
				matrix.drawRect(a+1, a+1, side-2, side-2, HIGH);
			}
			if (a < 6) {
				matrix.drawRect(a+2, a+2, side-4, side-4, HIGH);
			}
			matrix.write();
			a--;
			side+=2;

			if (a < -2) {
				a = 7;
				side = 2;
			}
		}
	}
}

void crazy_lines() {
	int pos = 0, flagpos = 1;
	unsigned long s_time = millis();
	unsigned long c_time = s_time;

	while ((next_key = kpd.getKey()) == NO_KEY) {
		c_time = millis();

		//wait for next step
		if (c_time - s_time > 100) {
			s_time = c_time;

			matrix.fillScreen(LOW);
			matrix.drawLine(pos, 0, pos, 15, HIGH);
			matrix.drawLine(0, pos, 15, pos, HIGH);
			matrix.drawLine(0, 15-pos, 15, 15-pos, HIGH);
			matrix.write();
			pos += flagpos;

			if (pos == 15 || pos == 0) {
				flagpos*=-1;
			}
		}
	}
}

void light_point() {
	while((next_key = kpd.getKey()) == NO_KEY) {
		matrix.fillScreen(LOW);
		matrix.drawPixel(random(16), random(16), HIGH);
		matrix.write();
		delay(20);
	}
}

void loop() {
	char keypressed = kpd.getKey();
	if (keypressed != NO_KEY || next_key != NO_KEY) {
		//use addition var for next step
		if (keypressed == NO_KEY) {
			keypressed = next_key;
		}
		next_key = NO_KEY;

		Serial.println(keypressed);
		Serial.println(flag);

		//static figures
		if(flag == 0) {
			matrix.fillScreen(LOW);
			matrix.write();

			switch(keypressed) {
				case '1': {
					matrix.drawCircle( 8, 8, 7, HIGH );
					matrix.write();
					break;
				}
				case '2': {
					matrix.drawTriangle(2, 12, 8, 2, 16, 12, HIGH);
					matrix.write();
					break;
				}
				case '3': {
					matrix.drawRect(2, 2, 14, 14, HIGH);
					matrix.write();
					break;
				}
				case '0': {
					matrix.fillScreen(LOW);
					matrix.drawPixel(0, 15, HIGH); 
					matrix.write();
					flag = 1;
					Serial.println(flag);
					break;
				}
				case '4': {
					matrix.drawRoundRect(1, 5, 14, 8, 4, HIGH);
					matrix.write();
					break;
				}
				case '5': {
					matrix.drawLine(5, 3, 11, 3, HIGH);
					matrix.drawLine(11, 3, 15, 8, HIGH);
					matrix.drawLine(15, 8, 11, 13, HIGH);
					matrix.drawLine(11, 13, 5, 13, HIGH);
					matrix.drawLine(5, 13, 1, 8, HIGH);
					matrix.drawLine(1, 8, 5, 3, HIGH);
					matrix.write();
					break;
				}
				case '6': {
					matrix.drawRect(6, 0, 4, 16, HIGH);
					matrix.drawRect(0, 6, 16, 4, HIGH);
					matrix.fillRect(6, 0, 4, 16, HIGH);
					matrix.fillRect(0, 6, 16, 4, HIGH);
					matrix.write();
					break;
				}
				case '7': {
					matrix.drawRect(0, 0, 4, 4, HIGH);
					matrix.drawRect(0, 4, 4, 4, HIGH);
					matrix.drawRect(0, 8, 4, 4, HIGH);
					matrix.drawRect(0, 12, 4, 4, HIGH);
					matrix.drawRect(4, 2, 4, 4, HIGH);
					matrix.drawRect(4, 6, 4, 4, HIGH);
					matrix.drawRect(4, 10, 4, 4, HIGH);
					matrix.drawRect(8, 4, 4, 4, HIGH);
					matrix.drawRect(8, 8, 4, 4, HIGH);
					matrix.drawRect(12, 6, 4, 4, HIGH);
					matrix.write();
					break;
				}
				case '8': {
					matrix.drawCircle(3, 7, 4, HIGH);
					matrix.drawCircle(9, 7, 2, HIGH);
					matrix.drawCircle(13, 7, 1, HIGH);
					matrix.write();
					break;
				}
				case '9': {
					matrix.drawRect(0, 0, 15, 15, HIGH);
					matrix.drawCircle(7, 7, 6, HIGH);
					matrix.drawRect(4, 4, 7, 7, HIGH);
					matrix.drawCircle(7, 7, 2, HIGH);

					matrix.write();
					break;
				}
				default:{
					matrix.drawPixel(0, 15, HIGH);
					matrix.write();
				}
			}
		}
		//dynamic figures
		else if(flag == 1) {
			switch (keypressed) {
				case '1': {
					fig_reflection(0);
					break;
				}
				case '2': {
					fig_reflection(1);
					break;
				}
				case '3': {
					random_points();
					break;
				}
				case '4': {
					growing_rect();
					break;
				}
				case '5': {
					crazy_lines();
					break;
				}
				case '6': {
					light_point();
					break;
				}
				case '7': {
					rain();
					break;
				}
				case '8': {
					spiral();
					break;
				}
				case '9': {
					eyes();
					break;
				}
				case '*': {
					matrix_demo();
					break;
				}
				case '0': {
					matrix.fillScreen(LOW);
					matrix.drawPixel(0, 15, HIGH); 
					Serial.println("tak blet");
					matrix.write();
					flag = 0;
					break;
				}
			}
		}
	}

}
