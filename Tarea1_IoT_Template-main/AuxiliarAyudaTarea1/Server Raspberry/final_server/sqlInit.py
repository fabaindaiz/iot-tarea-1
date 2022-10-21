createData = '''CREATE TABLE Datos (
    MessageId INTEGER PRIMARY KEY,
    MAC TEXT NOT NULL,
    IDDevice TEXT NOT NULL,
    Timestamp DATETIME DEFAULT (datetime('now', 'localtime')),
    Val INTEGER,
    Batt FLOAT,
    Temp FLOAT,
    Press INTEGER,
    Hum INTEGER,
    Co FLOAT,
    RMS FLOAT,
    Ampx FLOAT, 
    Frecx FLOAT,
    Ampy FLOAT,
    Frecy FLOAT,
    Ampz FLOAT,
    Frecz FLOAT,
    Accx TEXT,
    Accy TEXT,
    Accz TEXT
);'''
# Para el Timestamp probar: Timestamp DATETIME DEFAULT (datetime('now', 'localtime')),

createLogs = '''CREATE TABLE Logs (
    LogId INTEGER PRIMARY KEY,
    IDDevice TEXT NOT NULL,
    TransportLayer INTEGER,
    ProtocolID INTEGER,
    Timestamp DATETIME DEFAULT (datetime('now', 'localtime'))
);'''

createConf = '''CREATE TABLE Conf (
    ConfId INTEGER PRIMARY KEY,
    ProtocolID INTEGER,
    TransportLayer INTEGER
);'''

import sqlite3 as sql


conn = sql.connect("DB.sqlite")
cur = conn.cursor()
cur.execute(createData)
cur.execute(createLogs)
cur.execute(createConf)
conn.close()

# inicializa la BDD