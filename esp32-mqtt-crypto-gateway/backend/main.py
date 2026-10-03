import sqlite3
import paho.mqtt.client as mqtt
import time
import requests

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

    client.subscribe("coin/name")

def on_message(client, userdata, msg):
    incoming_coin_name = msg.payload.decode("utf-8").strip()
    try:
        symbol = incoming_coin_name + "USDT"
        url = f"https://api.binance.com/api/v3/ticker/price?symbol={symbol}"

        response = requests.get(url)
        data = response.json()

        coin_price = float(data["price"])
        last_coin_price = round(coin_price, 2)

        print(f"{incoming_coin_name}: {last_coin_price}")

        client.publish("coin/price", last_coin_price)

        cursor.execute("INSERT INTO data (coin_name, coin_price) VALUES (?, ?)", (symbol, last_coin_price))
        conn.commit()
    except Exception as e:
        print("API error or incorrect coin input: ",e)
        client.publish("error", "ERROR")

client = mqtt.Client(client_id="python-crypto-gateway")

broker_ip = "pc_ip"
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




















