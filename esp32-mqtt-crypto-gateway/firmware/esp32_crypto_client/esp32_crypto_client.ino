#include <WiFi.h>
#include <PubSubClient.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define LED 13

const char *ssid = "WiFi_name";
const char *password = "WiFi_password";

const char *mqtt_server = "pc_ip";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

int durum = 1;

void callback(char *topic, byte *payload, unsigned int length) {
  char gelen_fiyat[50];
  int uzunluk = length;
  if (uzunluk > 49) { uzunluk = 49; }

  for (int i = 0; i < uzunluk; i++) {
    gelen_fiyat[i] = (char)payload[i];
  }
  gelen_fiyat[uzunluk] = '\0';

  digitalWrite(LED, durum);
  durum = !durum;

  lcd.setCursor(0, 1);
  lcd.print(gelen_fiyat);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("esp32")) {
      Serial.println("Yaka kartimizla(esp32) giris yaptik. Dinleniyor...");
      client.subscribe("coin/price");
    }
    else {
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  lcd.init();
  lcd.backlight();

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi Agina Baglanildi!");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

}

void loop() {
  client.loop();

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      reconnect();
    }


    if (Serial.available() > 0) {
      lcd.clear();


      String gelen_coin = Serial.readStringUntil('\n');
      gelen_coin.trim();
      if (gelen_coin.length() > 0) {
        gelen_coin.toUpperCase();
        Serial.println(gelen_coin);


        client.publish("coin/name", gelen_coin.c_str());

      
        lcd.setCursor(0, 0);
        lcd.print(gelen_coin);
        lcd.print("USDT:");
      }
    }
  }
}



























