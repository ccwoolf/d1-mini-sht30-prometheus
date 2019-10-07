#define OLED_ENABLED false
#define PROMETHEUS_ENABLED true
#define DEEP_SLEEP_PERIOD 55e6

#ifdef OLED_ENABLED
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
  #include <Fonts/TomThumb.h>
  Adafruit_SSD1306 display(0);
#endif

#if PROMETHEUS_ENABLED
  #define WIFI_SSID "mywifi"
  #define WIFI_PSK "likeimgonnaputthisongithub"
  #define PUSHGATEWAY_URL "http://pushgateway:9091"
  #define PUSHGATEWAY_JOB "environment"
  #define PUSHGATEWAY_INSTANCE "kitchen"
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
#endif

#include <Wire.h>
#include <WEMOS_SHT3X.h>
SHT3X sht30(0x45);

void setup() {
  pinMode(D8, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  #if PROMETHEUS_ENABLED
    WiFi.begin(WIFI_SSID, WIFI_PSK);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
  #endif

  #if OLED_ENABLED
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setFont(&TomThumb);
    display.setCursor(0, 10);
    display.setTextColor(WHITE);
  #endif
}

void loop() {
  if (sht30.get() == 0) {
    float temperature = sht30.cTemp;
    float humidity = sht30.humidity;

    postToPrometheus(temperature, humidity);
    displayOnScreen(String(temperature) + "'C", 2);
    displayOnScreen(String(humidity) + "%H", 2);
  }  else  {
    displayOnScreen("Error reading sensor");
  }

  if (digitalRead(D8) == HIGH) {
    while (true) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
    }
  }
  
  ESP.deepSleep(DEEP_SLEEP_PERIOD);
}

void postToPrometheus(float temperature, float humidity) {
#if PROMETHEUS_ENABLED
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(
      PUSHGATEWAY_URL
      "/metrics/job/"
      PUSHGATEWAY_JOB
      "/instance/"
      PUSHGATEWAY_INSTANCE
    );
    
    http.addHeader("Content-Type", "text/plain");
    int httpCode = http.POST(
      "# HELP Temperature (degrees celcius)\n"
      "# TYPE sht30_temperature_c gauge\n"
      "sht30_temperature_c " + String(temperature) + "\n"
      "# HELP Humidity (relative)\n"
      "# TYPE sht30_humidity_r gauge\n"
      "sht30_humidity_r " + String(humidity) + "\n"
    );    
    String payload = http.getString();
    displayOnScreen(httpCode + " " + payload);
    http.end();
  } else {
    displayOnScreen("WiFi unavailable");
  }
#endif
}

void displayOnScreen(String message) { displayOnScreen(message, 1); }
void displayOnScreen(String message, int size) {
#if OLED_ENABLED
  display.setTextSize(size);
  display.println(message);
  display.display();
#endif
}
