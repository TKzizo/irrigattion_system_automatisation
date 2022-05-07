import sqlite3
from sqlite3 import Error

def create_connection(db_file):
    """ create a database connection to a SQLite database """
    conn = None
    try:
        conn = sqlite3.connect(db_file)
        
    except Error as e:
        print(e)
    finally:
        return conn



def close_connection(conn):
    conn.close()


def insert_sensors_values(conn,values):
    sql = """ INSERT INTO arduino_sensors (id,date,air_hum,air_temp,water,soil) 
              VALUES(?,datetime("now"),?,?,?,?)
          """

    cur = conn.cursor()
    cur.execute(sql,values)
    conn.commit()

    return cur.lastrowid

