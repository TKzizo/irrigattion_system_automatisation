from flask_restful import Resource
from flask import request
from common.crud import *

class Sensors(Resource):
    def post(self):
        data = request.get_json()
        conn = create_connection("arduino.db")
        value = (int(data["id"]),
                int(data["info"]["air"]["hum"]),
                int(data["info"]["air"]["temp"]),
                int(data["info"]["water"]), 
                int(data["info"]["soil"])
                )
        insert_sensors_values(conn,value)
        close_connection(conn)
        return "successfuly update database"
