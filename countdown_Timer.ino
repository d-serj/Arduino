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

int8_t    TimeDisp[] = {0x00, 0x00, 0x00, 0x00};
int8_t    ClockPoint = 1;
int8_t    Update;
uint8_t   halfsecond = 0;
uint8_t   second     = 0;
uint8_t   minute     = 60;
bool      isStart    = false;
bool      firstStart = true;
int8_t    startButtonPress;
int8_t    riseButtonPress;
int8_t    resetButtonPress;

Bounce debouncer1 = Bounce();   // Антидребезг "Старт\Пауза"
Bounce debouncer2  = Bounce();  // Антидребезг "+1"
Bounce debouncer3 = Bounce();   // Антидребезг "Сброс"
TM1637 tm1637_1(CLK, DIO);
//TM1637 tm1637_2(8,9);

void setup()
{
    pinMode(B_START, INPUT);
    pinMode(B_RISE, INPUT);
    pinMode(B_RESET, INPUT);
	
    tm1637_1.set(5);
    //tm1637_2.set();
    tm1637_1.init();
    //tm1637_2.init();
	Serial.begin(9600);
  
    //Пин кнопок и интервал для антидребезга
    debouncer1.attach(B_START);
    debouncer1.interval(DEBOUNCETIME);
  
    debouncer2.attach(B_RISE);
    debouncer2.interval(DEBOUNCETIME);
  
    debouncer3.attach(B_RESET);
    debouncer3.interval(DEBOUNCETIME);

    Timer1.initialize(500000);      //timing for 500ms
    Timer1.stop();
    Timer1.attachInterrupt(TimingISR);  //declare the interrupt serve routine:TimingISR
    tm1637_1.display(TimeDisp);
    //tm1637_2.display(TimeDisp);
}

void loop()
{
    readButtons();
    
    if (Update == ON)
    {
        TimeUpdate();
        tm1637_1.display(TimeDisp);
        //tm1637_2.display(TimeDisp);
    }

    if (minute == 0 && second == 0)
        Timer1.stop();
}

/* Функция вызываемая по прерыванию (подсчет времени) */
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

/* Мигаем ":", обновляем массив данных для вывода на дисплей, пищим зуммером если осталось <20сек */
void TimeUpdate(void)
{
	// Мигаем ":" и пищим если осталось 20 секунд
	if (ClockPoint)
    {
        tm1637_1.point(POINT_OFF);
		//tm1637_2.point(POINT_OFF);
		
		// Если осталось 20 секунд, то пишим зуммером
		if (minute == 0 && second <= 20 && second > 0)
			tone(BUZZER, 1900, 100);
    }
    else
    {
        tm1637_1.point(POINT_ON);
        //tm1637_2.point(POINT_ON);
    }

    TimeDisp[0] = minute / 10;
    TimeDisp[1] = minute % 10;
    TimeDisp[2] = second / 10;
    TimeDisp[3] = second % 10;
    Update = OFF;
}

/* Функция управления кнопкой "Старт\Пауза". Счет или пауза счета при нажатии */
void startFunc (void)
{
    // инвертируем состояние кнопки
    isStart = !isStart;
    // Если разрешен старт стартуем таймер
    if (!isStart)
        Timer1.stop();
    else
        Timer1.start();

    // Если первый старт, то сбрасываем минуты и секунды
    if (firstStart)
    {
        minute = 60;
        second = 0;
    }

    //Serial.println(isStart);
    
    // Флаг первого старта в false чтобы следующие нажатие не сбросило время в 60:00
    firstStart = false;
}

/* Функция управления кнопкой "СБРОС". Сброс минут и секунд до 0, остановка таймера */
void resetFunc (void)
{
    // Присвоим минутам и секундам 0, после чего обновим буффер дисплея и отобразим время
    minute = 0;
    second = 0;
    TimeUpdate();
    tm1637_1.display(TimeDisp);
    
    Serial.print(minute);
    Serial.print(" : ");
    Serial.println(second);
    // Флаг старта в true, чтобы при нажатии "старт" выставить время в 60:00
    firstStart = true;
    Timer1.stop();
    // Делаем флаг продолжения счета в false чтобы в функции startFunc инвертировать в true
    isStart = false;
}

/* Функция управления кнопкой RISE. Увеличение минут на +1 за каждое нажатие */
void riseFunc (void)
{
	// Добавим +1 к минутам
	minute++;
    // Обновим время
	TimeUpdate();				
	tm1637_1.display(TimeDisp);
    Serial.print(minute);
    Serial.print(" : ");
    Serial.println(second);
	firstStart = false;
}

// Функция обработки нажатия кнопок
void readButtons (void)
{
	// Обновляем состояния кнопок
	debouncer1.update();
	debouncer2.update();
	debouncer3.update();

    // = 1 если изменение по положительному фронту
	startButtonPress = debouncer1.rose();
	riseButtonPress = debouncer2.rose();
	resetButtonPress = debouncer3.rose();
	
	// Если нажата кнопка старт/пауза
	if (startButtonPress == HIGH)
		startFunc();

    // Если нажата кнопка +1
    if (riseButtonPress == HIGH)
        riseFunc();

    // Если нажата кнопка сброс
    if (resetButtonPress == HIGH)
        resetFunc();
}
