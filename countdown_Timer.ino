/********** Countdown timer by Dubina Sergey. Ukraine 2017 ***********/
/*********************************************************************/
#include <TimerOne.h>
#include <Bounce2.h>
#include <util/atomic.h>
#include "TM1637.h"

#define ON  1
#define OFF 0

/* BEGIN PINS */
#define B_START 5
#define B_RISE  6
#define B_RESET 7
#define BUZZER  8
#define CLK1    3
#define DIO1    2
#define CLK2    11
#define DIO2    10
/* END PINS */

#define DEBOUNCETIME 5

/* Debugging on/off */
//#define DEBUG
/********************/


int8_t           TimeDisp[] = {0x00, 0x00, 0x00, 0x00};
int8_t           ClockPoint = 1;
bool             isStart    = false;
bool             firstStart = true;
volatile uint8_t second     = 0;
volatile uint8_t minute     = 60;
volatile uint8_t Update;
int8_t           startButtonPress;
int8_t           riseButtonPress;
int8_t           resetButtonPress;

Bounce debouncer1 = Bounce();       // Антидребезг "Старт\Пауза"
Bounce debouncer2 = Bounce();       // Антидребезг "+1"
Bounce debouncer3 = Bounce();       // Антидребезг "Сброс"
TM1637 tm1637_1(CLK1, DIO1);
TM1637 tm1637_2(CLK2,DIO2);

void setup()
{
    pinMode(B_START, INPUT);
    pinMode(B_RISE, INPUT);
    pinMode(B_RESET, INPUT);

    tm1637_1.set(3);                    // Яркость дисплея
    tm1637_2.set(3);
    tm1637_1.init();
    tm1637_2.init();

#ifdef DEBUG
    Serial.begin(9600);
#endif
    
    debouncer1.attach(B_START);         //Пин кнопок и интервал для антидребезга
    debouncer1.interval(DEBOUNCETIME);

    debouncer2.attach(B_RISE);
    debouncer2.interval(DEBOUNCETIME);

    debouncer3.attach(B_RESET);
    debouncer3.interval(DEBOUNCETIME);

    Timer1.initialize(500000);          //timing for 500ms
    Timer1.stop();
    Timer1.attachInterrupt(TimingISR);  //declare the interrupt serve routine:TimingISR
    
    tm1637_1.display(TimeDisp);
    tm1637_2.display(TimeDisp);
}

void loop()
{
    readButtons();

    if (Update == ON)
    {
        TimeUpdate();
        tm1637_1.display(TimeDisp);
        tm1637_2.display(TimeDisp);
    }

    if (minute == 0 && second == 0)
        Timer1.stop();
}

/* Функция вызываемая по прерыванию (подсчет времени, 2 раза по пол секунды) */
void TimingISR(void)
{
    static uint8_t halfsecond = 0;
    
    Update = ON;

    if (++halfsecond == 2)
    {
        if (second == 0)
        {
            minute --;
            second = 60;
        }

        second --;
        halfsecond = 0;

#ifdef DEBUG
    Serial.print(minute);
    Serial.print(" : ");
    Serial.println(second);
#endif

    }
    ClockPoint = (~ClockPoint) & 0x01;
}

/* Мигаем ":", обновляем массив данных для вывода на дисплей, пищим зуммером если осталось <20сек */
void TimeUpdate(void)
{
    // Мигаем ":" и пищим если осталось 20 секунд
    if (ClockPoint)
    {
        tm1637_1.point(POINT_OFF);
        tm1637_2.point(POINT_OFF);

        if (minute == 0 && second <= 20 && second > 0)
            tone(BUZZER, 1900, 100);
    }
    
    else
    {
        tm1637_1.point(POINT_ON);
        tm1637_2.point(POINT_ON);
    }
    
    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
        TimeDisp[0] = minute / 10;
        TimeDisp[1] = minute % 10;
        TimeDisp[2] = second / 10;
        TimeDisp[3] = second % 10;
        Update = OFF;
    }
}

/* Функция управления кнопкой "Старт\Пауза". Счет или пауза счета при нажатии */
void startFunc (void)
{
    isStart = !isStart;
    
    if (!isStart)                           // Если разрешен старт стартуем таймер
        Timer1.stop();
    else
        Timer1.start();
        
    if (firstStart)                         // Если первый старт, то сбрасываем минуты и секунды
    {
        ATOMIC_BLOCK(ATOMIC_FORCEON)
        {
            minute = 60;
            second = 0;
            Serial.println(TCCR1B, BIN);
        }
    }
    
    firstStart = false;                     // Флаг первого старта в false чтобы следующие нажатие не сбросило время в 60:00
}

/* Функция управления кнопкой "СБРОС". Сброс минут и секунд до 0, остановка таймера */
void resetFunc (void)
{
    Timer1.stop();
    // Флаг старта в true, чтобы при нажатии "старт" выставить время в 60:00
    firstStart = true;
    
    // Присвоим минутам и секундам 0, после чего обновим буффер дисплея и отобразим время
    minute = 0;
    second = 0;
    
    TimeUpdate();
    tm1637_1.display(TimeDisp);
    tm1637_2.display(TimeDisp);
    
    // Делаем флаг продолжения счета в false чтобы в функции startFunc инвертировать в true
    isStart = false;

#ifdef DEBUG
    Serial.print(minute);
    Serial.print(" : ");
    Serial.println(second);
    Serial.println(TCCR1B, BIN);
#endif
}

/* Функция управления кнопкой RISE. Увеличение минут на +1 за каждое нажатие */
void riseFunc (void)
{
    if (minute < 99)
    {
        ATOMIC_BLOCK(ATOMIC_FORCEON)
        {
            ++minute;
        }
    }
        
    
    // Обновим время
    TimeUpdate();
    tm1637_1.display(TimeDisp);
    tm1637_2.display(TimeDisp);
    
#ifdef DEBUG
    Serial.print(minute);
    Serial.print(" : ");
    Serial.println(second);
#endif

    firstStart = false;
}

/* Функция обработки нажатия кнопок */
void readButtons (void)
{
    // Обновляем состояния кнопок
    debouncer1.update();
    debouncer2.update();
    debouncer3.update();

    // = 1 если изменение по положительному фронту
    startButtonPress = debouncer1.rose();
    riseButtonPress  = debouncer2.rose();
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
