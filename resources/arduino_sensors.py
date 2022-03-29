from flask_restful import Resource, reqparse
from common.crud import *
import json

parser = reqparse.RequestParser()

parser.add_argument("id",type=int,required=True)
parser.add_argument("info",required=True)

class Sensors(Resource):
    def post(self):
        args = parser.parse_args()
        device_id = int(args["id"])
        data = json.loads(args["info"])
        conn = create_connection("/home/fas/Documents/arduino.db")
        value = (device_id,
                data["air"],
                data["water"], 
                data["soil"]
                )
        insert_sensors_values(conn,value)
        close_connection(conn)
       
