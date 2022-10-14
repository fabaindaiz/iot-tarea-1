import sqlite3 as sql


def getConf():
    fetchConfig = ''' SELECT ProtocolID, TransportLayer FROM Conf WHERE Confid=1'''
    with sql.connect("DB.sqlite") as con:
        cur = con.cursor()
        res = cur.execute(fetchConfig)
        return res.fetchall()[0]
        
print(getConf())



