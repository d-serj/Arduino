#include <Wire.h>
#include <ssd1306.h>
#include <font6x8.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Bounce2.h>

#define BUTTON1_PIN  5
#define BUTTON2_PIN  6
#define MEASUREMENTS 20
#define DELAY_TO_SCREEN_MS     1000
#define DELAY_TO_MEASURE_MS    100
#define LONGPRESS              3000

Adafruit_BMP280 bme;
Bounce button1 = Bounce();

const uint8_t Socol[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x0C, 0x08, 0x18, 0x18, 0x30, 0x70, 0x70, 0xE0, 0xE0, 0xE0,
	0xE0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFC, 0x6C, 0x6C, 0x6C,
	0xD8, 0x80, 0x00, 0xF0, 0xF0, 0x80, 0x80, 0x00, 0x00, 0xF0, 0xF0, 0x00, 0x00, 0xC0, 0xE0, 0x30,
	0x30, 0x20, 0x00, 0x00, 0xC0, 0xE0, 0x30, 0x30, 0x20, 0xE0, 0x00, 0x00, 0x30, 0xF0, 0x30, 0x30,
	0x00, 0xC0, 0xE0, 0x30, 0x30, 0x20, 0xE0, 0x00, 0x00, 0xF0, 0x60, 0xC0, 0x80, 0xC0, 0x60, 0xF0,
	0x00, 0x00, 0xC0, 0xF0, 0x50, 0x50, 0x70, 0x60, 0x00, 0xC0, 0xF0, 0x10, 0x30, 0x30, 0xE0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x03, 0x03, 0x07, 0x07, 0x0F, 0x0F, 0x0F, 0x1F, 0x3F, 0x3F, 0x7F, 0xFF, 0xFE, 0xFE, 0xFE, 0xFC,
	0xFC, 0xFC, 0xF8, 0xF8, 0xF8, 0xF8, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0xC0, 0x80, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x83, 0x83, 0x03,
	0x01, 0x00, 0x00, 0x03, 0x03, 0x02, 0x02, 0x03, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x01, 0x03,
	0x03, 0x01, 0x01, 0x00, 0x00, 0x01, 0x03, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x03, 0x03, 0x01, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x0F, 0x0F, 0x01, 0x03, 0x03, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03,
	0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x80, 0x00, 0x1E, 0x2D, 0x61, 0x61, 0x61,
	0x33, 0x00, 0x00, 0x18, 0x3C, 0x66, 0x66, 0x3C, 0x18, 0x00, 0x00, 0x7E, 0x18, 0x18, 0x66, 0x46,
	0x00, 0x18, 0x3C, 0x66, 0x66, 0x24, 0x3C, 0x00, 0x00, 0x20, 0x3E, 0x06, 0x06, 0x7E, 0x7E, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x03, 0x07, 0x07,
	0x0F, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xFC, 0xFC, 0xFC,
	0xFC, 0xFC, 0xFC, 0xF8, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x80, 0x80, 0x80, 0x80, 0xC0, 0xC0, 0xC0, 0xE0, 0xE0, 0xE0, 0xF0, 0xF0, 0xF0, 0xF8, 0xF8,
	0xFC, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x1F, 0x1F,
	0x1F, 0x1F, 0x3F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0xFC, 0xFC,
	0xFC, 0xF8, 0xF8, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x03, 0x03, 0x07, 0x07, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
	0x3F, 0x3F, 0x7F, 0x3F, 0x3F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x0F, 0x0F, 0x1F, 0x1F,
	0x1F, 0x3F, 0x3F, 0x3F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xF8,
	0xF0, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x07, 0x0F, 0x0F, 0x1F, 0x3F,
	0x7F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFE, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x03, 0x07, 0x07, 0x07, 0x0F, 0x1E, 0x1C, 0x38, 0x30,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

float preassureArray[MEASUREMENTS];
float preassureAverage = 0;
float preassureAverageFinal = 0;
float zeroPreassure = 0;
float maxAltitude = 0;
float altitude = 0;

void setup()
{
	pinMode(BUTTON1_PIN, INPUT);
	button1.attach(BUTTON1_PIN);
	button1.interval(10); // interval in ms

	if (!bme.begin()) {
		Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
		while (1);
	}

	delay(50);
	
	ssd1306_setFixedFont(ssd1306xled_font6x8);
	ssd1306_128x64_i2c_init();

	ssd1306_clearScreen();
	ssd1306_drawBitmap(0, 0, 128, 64, Socol);
	delay(500);

	// Первое заполнение массива и измерение высоты
	for (uint8_t i = 0; i < MEASUREMENTS; i++)
	{
		preassureArray[i] = bme.readPressure();
		preassureAverage += preassureArray[i];
		delay(50);
	}
	preassureAverageFinal = preassureAverage / MEASUREMENTS;
	preassureAverage = 0;
	zeroPreassure = preassureAverageFinal / 100.0;
	altitude = calcAltitude(zeroPreassure, preassureAverageFinal);
	
	delay(1000);
	ssd1306_clearScreen();
	printToScreen(0);
}

void loop()
{
	uint32_t currentMillis;
	static uint32_t screenMillis;
	static uint32_t measurementMillis;
	static uint32_t buttonMillis;
	static bool checkButton;
	static bool buttonPress1;
	static bool buttonLongPress1 = true;
	static uint8_t formattingText;
	static uint8_t countMeasurements;

	currentMillis = millis();
/*--------------------------------------------------------------------------*/
// Обработка нажатия кнопки.

	buttonPress1 = button1.update();

	// Обработка долгого нажатия кнопки
	if (!button1.read())
	{
		// Начинаем отсчёт времени нажатия
		if (buttonLongPress1)
		{
			buttonMillis = currentMillis;
			buttonLongPress1 = false;
		}

		// Прошло LONGPRESS время, тогда сбрасываем ZERO точку и MAX высоту в 0
		else if (currentMillis - buttonMillis > LONGPRESS)
		{
			zeroPreassure = preassureAverageFinal / 100.0;
			maxAltitude = 0;
			altitude    = 0;
			
			// Вывод инфо на экран
			ssd1306_clearScreen();
			ssd1306_printFixedN(10, 6, "BbICOTA", STYLE_NORMAL, FONT_SIZE_2X);
			ssd1306_printFixedN(10, 16, "and MAX", STYLE_NORMAL, FONT_SIZE_2X);
			ssd1306_printFixedN(30, 32, "BbICOTA", STYLE_NORMAL, FONT_SIZE_2X);
			ssd1306_printFixedN(10, 48, "set to 0", STYLE_BOLD, FONT_SIZE_2X);
			delay(4000);
			ssd1306_clearScreen();

			buttonMillis = currentMillis;
			buttonLongPress1 = true;
		}
	}

	else
		buttonLongPress1 = true;

	// Меняем тип отображения информации на экране
	if (buttonPress1 && button1.rose())
	{
		++formattingText;

		if (formattingText > 2)
			formattingText = 0;

		ssd1306_clearScreen();
		printToScreen(formattingText);
	}


/*------------------ Измерения --------------------------------------*/
// Измерения производятся MEASUREMENTS раз, после чего усредняются
	// Если время измерять очередное показание давления, 
	// то измеряем MEASUREMENTS - 1 раз
	if (currentMillis - measurementMillis >= DELAY_TO_MEASURE_MS)
	{
		preassureArray[countMeasurements] = bme.readPressure();
		preassureAverage += preassureArray[countMeasurements];
		//Serial.println(preassureArray[countMeasurements]);			// DEBUG
		
		countMeasurements++;

		// Если последнее измерение
		if (countMeasurements > MEASUREMENTS - 1)
		{
			preassureAverageFinal = preassureAverage / MEASUREMENTS;
			// Расчитываем высоту и максимально набранную высоту
			altitude = calcAltitude(zeroPreassure, preassureAverageFinal);

			if (altitude > maxAltitude)
				maxAltitude = altitude;

			// Сбрасываем счетчик и среднее давление в ноль
			countMeasurements = 0;
			preassureAverage = 0;
		}

		measurementMillis = currentMillis;
	}
//-------------------------------------------------------------------
// Экран
	//Обновляем экран
	if (currentMillis - screenMillis >= DELAY_TO_SCREEN_MS)
	{
		// IN MM/RH
		//Serial.println(float((preassureAverage)*0.007501), 1);
		printToScreen(formattingText);
		screenMillis = currentMillis;
	}
}

/*---- Печать информации о высоте на экран ----*/
void printToScreen(uint8_t formatting)
{
	const uint8_t bufferSize = 20;
	char buffer[bufferSize];
	char screenBuffer[bufferSize];

	switch (formatting)
	{
	case 0:
		// Высота
		dtostrf(altitude, 4, 1, buffer);
		sprintf(screenBuffer, "BbICOTA     %s M", buffer);
		ssd1306_printFixed(0, 16, "                    ", STYLE_NORMAL);
		ssd1306_printFixed(10, 16, screenBuffer, STYLE_NORMAL);

		// Очистка буферов
		for (uint8_t i = 0; i < bufferSize; ++i)
		{
			screenBuffer[i] = 0;
			buffer[i] = 0;
		}

		// Максимальная высота
		dtostrf(maxAltitude, 4, 1, buffer);
		sprintf(screenBuffer, "MAX BbICOTA %s M", buffer);
		ssd1306_printFixedN(0, 32, "                    ", STYLE_NORMAL, FONT_SIZE_NORMAL);
		ssd1306_printFixedN(10, 32, screenBuffer, STYLE_NORMAL, FONT_SIZE_NORMAL);
		break;
	case 1:
		dtostrf(altitude, 4, 1, buffer);
		ssd1306_printFixedN(10, 14, "BbICOTA", STYLE_NORMAL, FONT_SIZE_2X);
		ssd1306_printFixedN(0, 32, "      ", STYLE_NORMAL, FONT_SIZE_4X);
		ssd1306_printFixedN(0, 32, buffer, STYLE_NORMAL, FONT_SIZE_4X);
		break;
	case 2:
		dtostrf(maxAltitude, 4, 1, buffer);
		ssd1306_printFixedN(0, 6, "MAX", STYLE_NORMAL, FONT_SIZE_2X);
		ssd1306_printFixedN(40, 15, "BbICOTA", STYLE_NORMAL, FONT_SIZE_2X);
		ssd1306_printFixedN(0, 32, "      ", STYLE_NORMAL, FONT_SIZE_4X);
		ssd1306_printFixedN(0, 32, buffer, STYLE_NORMAL, FONT_SIZE_4X);
		break;
	}
}

/*---- Расчет высоты в метрах в зависимости от текущего давления ----*/
float calcAltitude(float seaLevelhPa, float pressure) 
{
	float f_altitude;
	pressure /= 100.0;
	f_altitude = 44330.0 * (1.0 - pow(pressure / seaLevelhPa, 0.190295));
	return f_altitude;
}

