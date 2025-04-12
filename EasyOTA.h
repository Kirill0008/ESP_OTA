#ifndef EasyOTA_h
#define EasyOTA_h

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <ESPmDNS.h>
#endif
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

class EasyOTA {
private:
  String _password;
  String _hostname;
  bool _usePassword = false;
  bool _useHostname = false;

  void setupWiFi(const char* ssid, const char* wifi_pass) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, wifi_pass);

    Serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
  }

public:
  // Установка пароля для OTA
  void setPassword(const char* password) {
    _password = password;
    _usePassword = true;
  }

  // Установка имени устройства
  void setHostname(const char* hostname) {
    _hostname = hostname;
    _useHostname = true;
  }

  // Инициализация OTA
  void begin(const char* ssid, const char* wifi_pass) {
    setupWiFi(ssid, wifi_pass);

    if (_useHostname) {
      ArduinoOTA.setHostname(_hostname.c_str());
#if defined(ESP32)
      WiFi.setHostname(_hostname.c_str());
#endif
    }

    if (_usePassword) {
      ArduinoOTA.setPassword(_password.c_str());
    }

    ArduinoOTA.onStart([this]() {
      Serial.println("OTA Update Started");
      if (this->onStartCallback) this->onStartCallback();
    });

    ArduinoOTA.onEnd([this]() {
      Serial.println("\nOTA Update Finished");
      if (this->onEndCallback) this->onEndCallback();
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([this](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (this->onErrorCallback) this->onErrorCallback(error);
      else ESP.restart();
    });

    ArduinoOTA.begin();
    Serial.println("OTA service ready");
  }

  void handle() {
    ArduinoOTA.handle();
  }

  // Callback функции
  std::function<void()> onStartCallback = nullptr;
  std::function<void()> onEndCallback = nullptr;
  std::function<void(ota_error_t)> onErrorCallback = nullptr;
};

#endif
/*
EXAMPLE CODE
#include "EasyOTA.h"
EasyOTA ota; // Создаём экземпляр класса
const char* ssid = "ssid";
const char* wifi_pass = "pass";
void setup() {
    Serial.begin(115200);
    // Опциональные настройки
    //ota.setHostname("MySmartDevice");  // Установка имени устройства
    ota.setPassword("OtaPass");   // Пароль для обновлений
    // Callback-функции
    ota.onStartCallback = []() {
        Serial.println("Starting update...");
        digitalWrite(LED_BUILTIN, LOW); // Пример: включение LED при обновлении
    };
    
    ota.onEndCallback = []() {
        Serial.println("Update complete!");
        digitalWrite(LED_BUILTIN, HIGH);
    };

    // Инициализация
    ota.begin(ssid, wifi_pass);
    // Другой ваш код...

}

// the loop function runs over and over again forever
void loop() {
  ota.handle(); // Обязательный вызов в loop()
// Другой ваш код...
}

*/