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
bool buttonFlag          = true;
bool firstStart          = true;

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
    debouncer1.update();
    debouncer2.update();
    debouncer3.update();

    int value1 = debouncer1.rose();
    int value2 = debouncer2.rose();
    int value3 = debouncer3.rose();
    // Если нажата кнопка старт/стоп и флаг сработки кнопки true
    if (value1 == HIGH && buttonFlag)
        startFunc(); 

    if (value2)
        riseFunc();

    if (value3)
        resetFunc();
    
    if (Update == ON)
    {
        TimeUpdate();
        tm1637.display(TimeDisp);
        //tm.display(TimeDisp);
    }

    if (minute == 0 && second == 0)
        Timer1.stop();
    // В конце цикла сбрасываем флаг если не зажата не одна с кнопок
    if (!value1 && !value2 && !value3)
        buttonFlag = true;
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
    }
    // Serial.println(second);
    ClockPoint = (~ClockPoint) & 0x01;
}

void TimeUpdate(void)
{
    if (ClockPoint)
    {
        tm1637.point(POINT_ON);
        //tm.point(POINT_ON);
    }
    else
    {
        tm1637.point(POINT_OFF);
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
    
    firstStart = false;
    buttonFlag = false;
}

void resetFunc (void)
{
    minute = 0;
    second = 0;
    TimeUpdate();
    tm1637.display(TimeDisp);
    firstStart = true;
    Timer1.stop();
}

void riseFunc (void)
{
    minute++;
    TimeUpdate();
    tm1637.display(TimeDisp);
    firstStart = false;
}

void readButtons (void)
{
    
}
