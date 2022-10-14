import json
import sqlite3 as sql

# Documentación https://docs.python.org/3/library/sqlite3.html

def dataSave(protocol, header, data):
    with sql.connect("DB.sqlite") as con:
        cur = con.cursor()
        if protocol == 0:
            cur.execute('''insert into Datos (MAC, IDDevice, Val) values (?, ?, ?)''',
                        (header["MAC"], header["ID"], data["Val"]))
        elif protocol == 1:
            cur.execute('''insert into Datos (MAC, IDDevice, Val, Batt) values (?, ?, ?, ?)''',
                        (header["MAC"], header["ID"], data["Val"], data["Batt_level"]))
        elif protocol == 2:
            cur.execute('''insert into Datos (MAC, IDDevice, Val, Batt, Temp, Press, Hum, Co) values (?, ?, ?, ?, ?, ?, ?, ?)''',
                        (header["MAC"], header["ID"], data["Val"], data["Batt_level"], data["Temp"], data["Pres"], data["Hum"], data["Co"]))
        elif protocol == 3:
            cur.execute('''insert into Datos (MAC, IDDevice, Val, Batt, Temp, Press, Hum, Co, RMS) values (?, ?, ?, ?, ?, ?, ?, ?, ?)''',
                        (header["MAC"], header["ID"], data["Val"], data["Batt_level"], data["Temp"], data["Pres"], data["Hum"], data["Co"], data["RMS"]))
            
            ["Val", "Batt_level", "Temp", "Pres", "Hum", "Co", "RMS", "Ampx", "Frecx", "Ampy", "Frecy", "Ampz", "Frecz"]
        elif protocol == 4:
            cur.execute('''insert into Datos (MAC, IDDevice, Val, Batt, Temp, Press, Hum, Co, RMS, Ampx, Frecx, Ampy, Frecy, Ampz, Frecz) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)''',
                        (header["MAC"], header["ID"], data["Val"], data["Batt_level"], data["Temp"], data["Pres"], data["Hum"], data["Co"], data["RMS"], data["Ampx"], data["Frecx"], data["Ampy"], data["Frecy"], data["Ampz"], data["Frecz"]))
        else:           
            cur.execute('''insert into Datos (MAC, IDDevice, Val, Batt) values (?, ?, ?, ?)''', 
            (header["MAC"], header["ID"], data["Val"], data["Batt_level"]))
        # json.dumps(data)))