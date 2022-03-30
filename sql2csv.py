import csv
import sqlite3 as sql

dbfile = r"path/to/db.db"
conn = sql.connect(dbfile)
cur = conn.cursor()
data = cur.execute("SELECT * FROM mytable")
with open('output.csv', 'wb') as f:
    writer = csv.writer(f)
    writer.writerow(['Column 1', 'Column 2', ...])
    writer.writerows(data)