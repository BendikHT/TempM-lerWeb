#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Firebase_ESP_Client.h>
#include <NTPClient.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// 1. Nettverksinnstillinger
#define WIFI_SSID "Telenor7321bod_EXT"
#define WIFI_PASSWORD "Duggingene2Judaisme4"

// 2. Firebase-legitimasjon
#define API_KEY "AIzaSyAzQXjGidjqF_ndyUCEGCUZcQEQ9Ip-5A8"
#define DATABASE_URL "https://tempmaaler-default-rtdb.europe-west1.firebasedatabase.app"

// Objektelementer
Adafruit_BMP280 bmp;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long lastSendTime = 0;
const long interval = 10000; // Sender hvert 10. sekund

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Start I2C på GPIO 21 og 22
  Wire.begin(21, 22);

  if (!bmp.begin(0x76)) {
    Serial.println("Kunne ikke initiere BMP280-sensoren!");
    while (1);
  }
  Serial.println("BMP280 er tilkoblet.");

  // Koble til Wi-Fi
  Serial.print("Kobler til Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTilkoblet Wi-Fi!");
  Serial.print("IP-adresse: ");
  Serial.println(WiFi.localIP());

  // Konfigurer Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Signer inn anonymt eksplisitt
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Anonym innlogging i Firebase var vellykket!");
  } else {
    Serial.printf("Feil ved anonym innlogging: %s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED && Firebase.ready() && (millis() - lastSendTime > interval || lastSendTime == 0)) {
    lastSendTime = millis();

    float temp = bmp.readTemperature();
    float press = bmp.readPressure() / 100.0F;

    Serial.println("\n--- Sender data til Firebase ---");

    // 1. Send temperatur
    if (Firebase.RTDB.setFloat(&fbdo, "/sensor/temperatur", temp)) {
      Serial.print("Temperatur lagret: ");
      Serial.print(temp);
      Serial.println(" °C");
    } else {
      Serial.println("Feil ved sending av temp: " + fbdo.errorReason());
    }

    // 2. Send lufttrykk
    if (Firebase.RTDB.setFloat(&fbdo, "/sensor/trykk", press)) {
      Serial.print("Lufttrykk lagret: ");
      Serial.print(press);
      Serial.println(" hPa");
    } else {
      Serial.println("Feil ved sending av trykk: " + fbdo.errorReason());
    }

    // 3. Send server-tidstempel
    if (Firebase.RTDB.setTimestamp(&fbdo, "/sensor/tidstempel")) {
      Serial.println("Tidstempel oppdatert!");
    } else {
      Serial.println("Feil ved sending av tidstempel: " + fbdo.errorReason());
    }
  }
}