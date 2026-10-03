import sqlite3
import paho.mqtt.client as mqtt
import time
import requests
from fastapi import FastAPI

app = FastAPI()

conn = sqlite3.connect("python_son.db", check_same_thread=False)
cursor = conn.cursor()

cursor.execute("""
    CREATE TABLE IF NOT EXISTS veriler (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    coin_name TEXT,
    coin_price REAL,
    time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    )
""")
conn.commit()

def on_connect(client, userdata, flags, reason_code, properties=None):
    print("Baglanildi ve dinleniyor...")

    client.subscribe("coin/name")

def on_message(client, userdata, msg):
    gelen_coin_adi = msg.payload.decode("utf-8").strip()
    try:
        sembol = gelen_coin_adi + "USDT"
        url = f"https://api.binance.com/api/v3/ticker/price?symbol={sembol}"

        response = requests.get(url)
        data = response.json()

        coin_fiyat = float(data["price"])
        last_coin_fiyat = round(coin_fiyat, 2)

        print(f"{gelen_coin_adi}: {last_coin_fiyat}")

        client.publish("coin/price", last_coin_fiyat)

        cursor.execute("INSERT INTO veriler (coin_name, coin_price) VALUES (?, ?)", (sembol, last_coin_fiyat))
        conn.commit()
    except Exception as e:
        print("API hatasi veya yanlis coin girdisi: ",e)
        client.publish("hata", "HATA");

client = mqtt.Client(client_id="python_database")

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




















