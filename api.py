from flask import Flask
from flask_restful import Api
from resources.arduino_sensors import Sensors

app = Flask(__name__)
api = Api(app)

api.add_resource(Sensors,"/")

if __name__ == "__main__":
    app.run(host="0.0.0.0",port=8000)
