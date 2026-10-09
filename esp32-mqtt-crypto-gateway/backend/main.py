import sqlite3
import paho.mqtt.client as mqtt
import time
import requests

import json

conn = sqlite3.connect("crypto_telemetry.db", check_same_thread=False)
cursor = conn.cursor()

cursor.execute("""
    CREATE TABLE IF NOT EXISTS data (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    coin_name TEXT,
    coin_price REAL,
    time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    )
""")
conn.commit()

def on_connect(client, userdata, flags, reason_code, properties=None):
    print("Connected and listening...")
    client.subscribe("esp32/crypto/requests")

def on_message(client, userdata, msg):
    try:
        incoming_package = json.loads(msg.payload.decode("utf-8"))

        coin_name = incoming_package["coin"]
        symbol = coin_name + "USDT"
        url = f"https://api.binance.com/api/v3/ticker/price?symbol={symbol}"

        binance_response = requests.get(url).json()
        coin_price = round(float(binance_response["price"]), 2)

        package = {"coin": coin_name, "price": coin_price}
        json_package = json.dumps(package)
        client.publish("esp32/crypto/response", json_package)

        print(json_package)

        cursor.execute("INSERT INTO data (coin_name, coin_price) VALUES (?, ?)", (coin_name, coin_price))
        conn.commit()

    except json.JSONDecodeError:
        print("The package arrived defective or damaged.")
    except KeyError:
        print("Binance Coin not found or API error.")


client = mqtt.Client(client_id="python-crypto-gateway")

broker_ip = "YOUR_MQTT_BROKER_IP"
broker_port = 1883

client.on_connect = on_connect
client.on_message = on_message

client.connect(broker_ip, broker_port)

client.loop_start()

try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    client.loop_stop()
    client.disconnect()














