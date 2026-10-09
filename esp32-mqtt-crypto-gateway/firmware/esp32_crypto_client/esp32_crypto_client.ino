#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> 

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define LED 13

const char *ssid = "WiFi_name";
const char *password = "WiFi_password";

const char *mqtt_server = "YOUR_MQTT_BROKER_IP";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long previousMillis = 0; 
const int period = 5000;

int status = 1;

void callback(char *topic, byte *payload, unsigned int length) {                             
  StaticJsonDocument<256> doc_sub;
  DeserializationError error = deserializeJson(doc_sub, payload, length);
  if (error) { return; }

  const char *incoming_coin_name = doc_sub["coin"];
  float incoming_coin_price = doc_sub["price"];

  digitalWrite(LED, status);
  status = !status;

  Serial.print(incoming_coin_name);
  Serial.print("USDT: ");
  Serial.print(incoming_coin_price);
  Serial.println("$");

  lcd.setCursor(0, 0);
  lcd.print(incoming_coin_name);
  lcd.print("USDT: ");
  lcd.setCursor(5, 1);
  lcd.print(incoming_coin_price);
  lcd.print("$");
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("esp32-crypto-client")) {
      Serial.println("We logged in using our badge (esp32-crypto-client). Listening...");
      client.subscribe("esp32/crypto/response");
    }
    else { delay(5000); }
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
  Serial.println("\nConnected to the WiFi network!");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    if (millis() - previousMillis >= period) {
      previousMillis = millis();

      if (Serial.available() > 0) {
        lcd.clear();

        String incoming_coin = Serial.readStringUntil('\n');
        incoming_coin.trim();
        if (incoming_coin.length() > 0) {
          incoming_coin.toUpperCase();

          StaticJsonDocument<256> doc_pub;
          doc_pub["uptime"] = millis();
          doc_pub["coin"] = incoming_coin;
          char send_package[256];
          serializeJson(doc_pub, send_package);
          client.publish("esp32/crypto/requests", send_package);

        }
      }
    }
  }
}





















