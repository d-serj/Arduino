#include <OneWire.h>

OneWire ds(9);

float temperature;
byte buff[9];

void setup() {
    Serial.begin(9600);
}

void loop() {
    ds.reset();
    ds.write(0xCC);
    ds.write(0x44);
    delay(900);
    ds.reset();
    ds.write(0xCC);
    ds.write(0xBE);
    ds.read_bytes(buff, 9);

    if (OneWire::crc8(buff, 8) == buff[8])
    {
        // Конвертация сырых данных
        temperature=  (float)((int)buff[0] | (((int)buff[1]) << 8)) * 0.0625 + 0.03125;
        Serial.println(temperature);
    }
    else
    {
        Serial.println("Error reading data");
    }
}
