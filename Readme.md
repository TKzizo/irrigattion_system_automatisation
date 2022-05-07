> # Projet FAS: Autonomous Irrigation System

## File structure 
**RASBPERRY**: containes the Rasperry pi setup.

**RASBPERRY/API**: contains the API code.

**RASPBERRY/arduino.db**: Sqlite3 databse.


**RASPBERRY/sql2csv.py**: convert sqldatabase into csv file

**Arduino**: containes the arduino board code. 

## Setup Raspberry:

**install database ( sqlite3 )**:
```sh
sudo apt update
sudo apt upgrade
sudo apt install sqlite3
```
**setup the database**:
```sql
CREATE TABLE arduino_sensors(id INTEGER, date DATETIME, air_hum INTEGER, air_temp INTEGER, water INTEGER, soil INTEGER, PRIMARY KEY(id,date));

```
**install python and FLASK**:
```sh
sudo apt install python3 python3-pip python-dev
pip3 -V
pip3 install -r requirements.txt
```

**lunch the server**:
```sh
python3 ./api.py
```
