/********** Countdown timer by Dubina Sergey. Ukraine 2017 ***********/
/*********************************************************************/
#include <TimerOne.h>
#include <Bounce2.h>
#include "TM1637.h"

#define ON 1
#define OFF 0
#define B_START 5
#define B_RISE 6
#define B_RESET 7
#define BUZZER 8
#define CLK 3
#define DIO 2
#define DEBOUNCETIME 5

int8_t TimeDisp[] = {0x00, 0x00, 0x00, 0x00};
unsigned char ClockPoint = 1;
unsigned char Update;
unsigned char halfsecond = 0;
unsigned char second     = 0;
unsigned char minute     = 60;
bool isStart             = false;
bool firstStart          = true;
int8_t value1;
int8_t value2;
int8_t value3;

Bounce debouncer1 = Bounce(); // Антидребезг "Старт"
Bounce debouncer2  = Bounce(); // Антидребезг "+1"
Bounce debouncer3 = Bounce(); // Антидребезг "Сброс"
TM1637 tm1637(CLK, DIO);
//TM1637 tm(8,9);

void setup()
{
    pinMode(B_START, INPUT);
    pinMode(B_RISE, INPUT);
    pinMode(B_RESET, INPUT);
	
    tm1637.set(5);
    //tm.set();
    tm1637.init();
    //tm.init();
	Serial.begin(9600);
  
    //Пин и интервал кнопок для антидребезга
    debouncer1.attach(B_START);
    debouncer1.interval(DEBOUNCETIME);
  
    debouncer2.attach(B_RISE);
    debouncer2.interval(DEBOUNCETIME);
  
    debouncer3.attach(B_RESET);
    debouncer3.interval(DEBOUNCETIME);

    Timer1.initialize(500000);      //timing for 500ms
    Timer1.stop();
    Timer1.attachInterrupt(TimingISR);  //declare the interrupt serve routine:TimingISR
    tm1637.display(TimeDisp);
    //tm.display(TimeDisp);
}
void loop()
{
	readButtons();
    
    if (Update == ON)
    {
        TimeUpdate();
        tm1637.display(TimeDisp);
        //tm.display(TimeDisp);
    }

    if (minute == 0 && second == 0)
        Timer1.stop();
}

void TimingISR(void)
{
    halfsecond ++;
    Update = ON;

    if (halfsecond == 2) 
    {    
        if (second == 0)
        {
            minute --;
            second = 60;
        }

        second --;
        halfsecond = 0;

        Serial.print(minute);
        Serial.print(" : ");
        Serial.println(second);
        Serial.println();
    }
    // Serial.println(second);
    ClockPoint = (~ClockPoint) & 0x01;
}

void TimeUpdate(void)
{
	if (ClockPoint)
    {
        tm1637.point(POINT_OFF);
		
		if (minute == 0 && second <= 20 && second > 0)
			tone(BUZZER, 1900, 100);
        //tm.point(POINT_ON);
		
    }
    else
    {
        tm1637.point(POINT_ON);
        //tm.point(POINT_OFF);
    } 

    TimeDisp[0] = minute / 10;
    TimeDisp[1] = minute % 10;
    TimeDisp[2] = second / 10;
    TimeDisp[3] = second % 10;
    Update = OFF;
}

void startFunc (void)
{
    // инвертируем старт
    isStart = !isStart;
    // Если разрешен старт стартуем таймер
    if (!isStart)
        Timer1.stop();
    else
        Timer1.start();
    if (firstStart)
    {
        minute = 60;
        second = 0;
    }

    Serial.println(isStart);
    
    firstStart = false;
}

void resetFunc (void)
{
    minute = 0;
    second = 0;
    TimeUpdate();
    tm1637.display(TimeDisp);
    Serial.print(minute);
    Serial.print(" : ");
    Serial.println(second);
    firstStart = true;
    Timer1.stop();
    // Делаем флаг продолжения счета в false чтобы в функции startFunc начался счет
    isStart = false;
}

/* Функция управления кнопкой RISE. Увеличение минут на +1 за каждое нажатие */
void riseFunc (void)
{
	// Добавим +1 к минутам
	minute++;
	TimeUpdate();				// Обновим время
	tm1637.display(TimeDisp);
	Serial.print(minute);
	Serial.print(" : ");
    Serial.println(second);
	firstStart = false;
}

// Функция обработки нажатия кнопок
void readButtons (void)
{
	debouncer1.update();
	debouncer2.update();
	debouncer3.update();
	
	value1 = debouncer1.rose();
	value2 = debouncer2.rose();
	value3 = debouncer3.rose();
	
	// Если нажата кнопка старт/стоп и флаг сработки кнопки true
	if (value1 == HIGH)
		startFunc();

    if (value2 == HIGH)
        riseFunc();

    if (value3 == HIGH)
        resetFunc();
}

