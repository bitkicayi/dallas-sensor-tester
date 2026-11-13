#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <OneWire.h>
#include <DallasTemperature.h>

constexpr uint8_t OLED_SCK = D1;
constexpr uint8_t OLED_SDA = D6;
constexpr uint8_t SENSOR_DQ = D2;
constexpr uint32_t I2C_HIZI = 100000;
constexpr unsigned long GOSTERIM_MS = 3000;

// SSD1315 panel için U8g2'nin SSD1306 uyumlu ekran tanımı.
U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(
    U8G2_R0, U8X8_PIN_NONE, OLED_SCK, OLED_SDA);

OneWire oneWire(SENSOR_DQ);
DallasTemperature sensor(&oneWire);

bool ekraniBaslat() {
    Wire.begin(OLED_SDA, OLED_SCK);
    Wire.setClock(I2C_HIZI);

    const uint8_t adresler[] = {0x3C, 0x3D};
    for (const uint8_t adres : adresler) {
        Wire.beginTransmission(adres);
        if (Wire.endTransmission() == 0) {
            oled.setI2CAddress(adres << 1);
            oled.setBusClock(I2C_HIZI);
            oled.begin();
            return true;
        }
    }
    return false;
}

void setup() {
    WiFi.persistent(false);
    WiFi.mode(WIFI_OFF);

    if (ekraniBaslat()) {
        sensor.begin();
        sensor.requestTemperatures();
        const float sicaklik = sensor.getTempCByIndex(0);

        oled.clearBuffer();
        oled.setFont(u8g2_font_6x12_tf);
        oled.drawStr(0, 14, "Sicaklik");
        oled.setCursor(0, 43);

        if (sicaklik >= -55.0f && sicaklik <= 125.0f) {
            oled.setFont(u8g2_font_10x20_tf);
            oled.print(sicaklik, 1);
            oled.print(F(" C"));
        } else {
            oled.print(F("Sensor hatasi"));
        }

        oled.sendBuffer();
        delay(GOSTERIM_MS);
        oled.setPowerSave(1);
    } else {
        Serial.begin(115200);
        Serial.println(F("OLED bulunamadi. D1, D6 ve beslemeyi kontrol et."));
        Serial.flush();
    }

    ESP.deepSleep(0, WAKE_RF_DISABLED);
}

void loop() {}