from fastapi import FastAPI
import sqlite3

app = FastAPI()


@app.get("/past")
def look_past():
    conn = sqlite3.connect("crypto_telemetry.db", check_same_thread=False)
    cursor = conn.cursor()

    cursor.execute("SELECT COUNT(*) FROM data")
    total_quantity = cursor.fetchone()[0]

    cursor.execute("SELECT * FROM data ORDER BY id DESC")
    records = cursor.fetchall()

    results = []
    for line in records:
        sonuclar.append({
            "id": line[0],
            "coin_name": line[1],
            "coin_price": line[2]
        })

    conn.close()

    return {
        "kayit_sayisi": toplam_adet,
        "kayitlar": sonuclar
    }
