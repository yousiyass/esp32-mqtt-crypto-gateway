from fastapi import FastAPI
import sqlite3

app = FastAPI()


@app.get("/home")
def gecmis():
    conn = sqlite3.connect("python_son.db", check_same_thread=False)
    cursor = conn.cursor()

    cursor.execute("SELECT COUNT(*) FROM veriler")
    toplam_adet = cursor.fetchone()[0]

    cursor.execute("SELECT * FROM veriler ORDER BY id DESC")
    kayitlar = cursor.fetchall()

    sonuclar = []
    for satir in kayitlar:
        sonuclar.append({
            "id": satir[0],
            "coin_name": satir[1],
            "coin_price": satir[2]
        })

    conn.close()

    return {
        "kayit_sayisi": toplam_adet,
        "kayitlar": sonuclar
    }