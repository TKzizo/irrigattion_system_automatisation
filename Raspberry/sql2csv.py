import csv
import sqlite3 as sql

dbfile = r"./API/arduino.db"
conn = sql.connect(dbfile)
cur = conn.cursor()
data = cur.execute("SELECT * FROM arduino_sensors")
fields = ["id","date","air_hum","air_temp","water","soil"]
with open('output.csv', 'w') as f:
    writer = csv.writer(f)
    writer.writerow(fields)
    writer.writerows(data)
