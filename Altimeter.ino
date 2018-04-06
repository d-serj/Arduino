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

const uint8_t Sova[] PROGMEM = {
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
float zeroPreassure = 1005.00;
float maxAltitude = 0;
float altitude = 0;
uint32_t buttonMillis = 3000;

void setup()
{
	Serial.begin(9600);

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
	ssd1306_drawBitmap(0, 0, 128, 64, Sova);
	delay(1000);
	ssd1306_clearScreen();

	// Первое заполнение массива
	for (uint8_t i = 0; i < MEASUREMENTS; i++)
	{
		preassureArray[i] = bme.readPressure();
		preassureAverage += preassureArray[i];
		delay(50);
	}
	preassureAverageFinal = preassureAverage / (float)MEASUREMENTS;
	zeroPreassure = preassureAverageFinal / 100.0;
	altitude = calcAltitude(zeroPreassure, preassureAverageFinal);

	delay(1000);
	ssd1306_clearScreen();
	printToScreen(altitude, 0);

	/*ssd1306_printFixed(0, 8, "Normal text", STYLE_NORMAL);
	ssd1306_printFixed(0, 16, "Bold text", STYLE_BOLD);
	ssd1306_printFixed(0, 24, "Italic text", STYLE_ITALIC);
	ssd1306_negativeMode();
	ssd1306_printFixed(0, 32, "Inverted bold", STYLE_BOLD);
	ssd1306_positiveMode();
	delay(5000);
	ssd1306_clearScreen();
	ssd1306_printFixedN(0, 0, "N3", STYLE_NORMAL, FONT_SIZE_8X);
	delay(5000);*/
}

void loop()
{
	uint32_t currentMillis;
	static uint32_t screenMillis;
	static uint32_t measurementMillis;
	static bool checkButton;
	static bool buttonPress1;
	static uint8_t formattingText;
	static uint8_t countMeasurements;

	button1.update();

	buttonPress1 = button1.fell();
	checkButton = HIGH;

	currentMillis = millis();

	// Определяем долгое нажатие кнопки для сброса нулевой точки и максимальной высоты
	if (buttonPress1 == LOW && (currentMillis - buttonMillis > LONGPRESS))
	{
		zeroPreassure = preassureAverageFinal / 100.0;
		maxAltitude = 0;
		//Serial.println(zero_preassure);
		buttonPress1 = HIGH;
		buttonMillis = currentMillis;
	}

	if (buttonPress1 == LOW)
	{
		//++formattingText;

		if (++formattingText > 2)
			formattingText = 0;

		buttonPress1 = HIGH;
		printToScreen(altitude, formattingText);
	}

//---- Усреднение значений давления. Измерение каждый цикл с задержкой DELAY_TO_MEASURE_MS ----------
	if (currentMillis - measurementMillis >= DELAY_TO_MEASURE_MS)
	{
		preassureArray[countMeasurements] = bme.readPressure();
		preassureAverage += preassureArray[countMeasurements];
		//Serial.println(preassureArray[countMeasurements]);
		
		//countMeasurements++;

		// Если дошли до конца массива
		if (++countMeasurements > MEASUREMENTS - 1)
		{
			// Получаем финальное среднее значение с выборки измерений
			preassureAverageFinal = preassureAverage / MEASUREMENTS;
			// Расчитываем высоту
			altitude = calcAltitude(zeroPreassure, preassureAverageFinal);
			
			// Расчитвыем максимально набранную высоту
			if (altitude > maxAltitude)
				maxAltitude = altitude;

			// Сбрасываем счетчик и среднее значене
			countMeasurements = 0;
			preassureAverage = 0;
		}

		measurementMillis = currentMillis;
	}

	// Раз в DELAY_TO_SCREEN_MS обновляем экран
	if (currentMillis - screenMillis >= DELAY_TO_SCREEN_MS)
	{
		// IN MM/RH
		//Serial.println(float((preassureAverage)*0.007501), 1);
		printToScreen(altitude, formattingText);
		screenMillis = currentMillis;
	}

	Serial.println(preassureAverage);
}

/* Печать высоты и максимальной высоты на экран, с необходимым форматированием */
void printToScreen(float data, uint8_t formatting)
{
	char buffer[20];
	char buffer2[20];

	dtostrf(data, 4, 2, buffer);
	dtostrf(maxAltitude, 4, 2, buffer2);

	ssd1306_clearScreen();

	switch (formatting)
	{
	case 0:
		sprintf(buffer, "Alt%s M", buffer);
		ssd1306_printFixed(0, 32, buffer, STYLE_NORMAL);
		sprintf(buffer2, "MAX %s M", buffer2);
		ssd1306_printFixed(0, 48, buffer2, STYLE_NORMAL);
		break;
	case 1:
		dtostrf(data, 4, 2, buffer);
		sprintf(buffer, "%s", buffer);
		ssd1306_printFixedN(0, 4, buffer, STYLE_NORMAL, FONT_SIZE_4X);
		break;
	case 2:
		dtostrf(data, 5, 2, buffer);
		sprintf(buffer, "%s", buffer);
		ssd1306_printFixedN(0, 4, buffer, STYLE_ITALIC, FONT_SIZE_4X);
		break;
	default:
		break;
	}
}

/* Расчет высоты в метрах */
float calcAltitude(float seaLevelhPa, float pressure) 
{
	float altitude;
	
	pressure /= 100.0;
	altitude = 44330.0 * (1.0 - pow(pressure / seaLevelhPa, 0.190295));
	return altitude;
}

