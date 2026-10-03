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

int status = 1;

void callback(char *topic, byte *payload, unsigned int length) {
  char incoming_price[50];
  int msg_len = length;
  if (msg_len > 49) { msg_len = 49; }

  for (int i = 0; i < msg_len; i++) {
    incoming_price[i] = (char)payload[i];
  }
  incoming_price[msg_len] = '\0';

  digitalWrite(LED, status);
  status = !status;

  lcd.setCursor(0, 1);
  lcd.print(incoming_price);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("esp32-crypto-client")) {
      Serial.println("We logged in using our badge (esp32-crypto-client). Listening...");
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

    if (Serial.available() > 0) {
      lcd.clear();


      String incoming_coin = Serial.readStringUntil('\n');
      incoming_coin.trim();
      if (incoming_coin.length() > 0) {
        incoming_coin.toUpperCase();
        Serial.println(incoming_coin);


        client.publish("coin/name", incoming_coin.c_str());

      
        lcd.setCursor(0, 0);
        lcd.print(incoming_coin);
        lcd.print("USDT:");
      }
    }
  }
}



























