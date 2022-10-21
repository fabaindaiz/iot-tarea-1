import sqlite3 as sql


def getConf():
    with sql.connect("DB.sqlite") as con:
        cur = con.cursor()
        res = cur.execute('''SELECT ProtocolID, TransportLayer FROM Conf WHERE Confid=1''')
        return res.fetchall()[0]
    
def setConf(new_protocol, new_transport):
    with sql.connect("DB.sqlite") as con:
        cur = con.cursor()
        cur.execute('''UPDATE Conf SET ProtocolID = ?, TransportLayer = ? WHERE ConfId = 1''',
                    (new_protocol, new_transport))
    

def dataSave(protocol, header, data):
    with sql.connect("DB.sqlite") as con:
        cur = con.cursor()
        cur.execute('''INSERT into Logs (IDDevice, TransportLayer, ProtocolID) values (?, ?, ?)''',
                        (header["ID"], header["transport"], header["protocol"])) 

        if protocol == 0: # Protocolo de configuración
            pass
        
        elif protocol == 1:
            cur.execute('''INSERT into Datos (MAC, IDDevice, Val, Batt) values (?, ?, ?, ?)''',
                        (header["MAC"], header["ID"], data["Val"], data["Batt_level"]))
            
        elif protocol == 2:
            cur.execute('''INSERT into Datos (MAC, IDDevice, Val, Batt, Temp, Press, Hum, Co) values (?, ?, ?, ?, ?, ?, ?, ?)''',
                        (header["MAC"], header["ID"], data["Val"], data["Batt_level"], data["Temp"], data["Pres"], data["Hum"], data["Co"]))
        elif protocol == 3:
            cur.execute('''INSERT into Datos (MAC, IDDevice, Val, Batt, Temp, Press, Hum, Co, RMS) values (?, ?, ?, ?, ?, ?, ?, ?, ?)''',
                        (header["MAC"], header["ID"], data["Val"], data["Batt_level"], data["Temp"], data["Pres"], data["Hum"], data["Co"], data["RMS"]))
        elif protocol == 4:
            cur.execute('''INSERT into Datos (MAC, IDDevice, Val, Batt, Temp, Press, Hum, Co, RMS, Ampx, Frecx, Ampy, Frecy, Ampz, Frecz) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)''',
                        (header["MAC"], header["ID"], data["Val"], data["Batt_level"], data["Temp"], data["Pres"], data["Hum"], data["Co"], data["RMS"], data["Ampx"], data["Frecx"], data["Ampy"], data["Frecy"], data["Ampz"], data["Frecz"]))
        elif protocol == 5:
            cur.execute('''INSERT into Datos (MAC, IDDevice, Val, Batt, Temp, Press, Hum, Co, Accx, Accy, Accz) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)''',
                        (header["MAC"], header["ID"], data["Val"], data["Batt_level"], data["Temp"], data["Pres"], data["Hum"], data["Co"], data["Accx"], data["Accy"], data["Accz"]))
        else:
            pass