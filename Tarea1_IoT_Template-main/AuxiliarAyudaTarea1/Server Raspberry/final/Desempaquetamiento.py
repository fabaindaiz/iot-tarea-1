from struct import unpack, pack
import traceback
from sqlQuery import * 

# Documentación struct unpack,pack :https://docs.python.org/3/library/struct.html#
'''
Estas funciones se encargan de parsear y guardar los datos recibidos.
Usamos struct para pasar de un array de bytes a una lista de numeros/strings. (https://docs.python.org/3/library/struct.html)
(La ESP32 manda los bytes en formato little-endian, por lo que los format strings deben empezar con <)

-dataSave: Guarda los datos en la BDD
-response: genera un OK para mandar de vuelta cuando se recibe un mensaje, con posibilidad de pedir que se cambie el status/protocol
-protUnpack: desempaca un byte array con los datos de un mensaje (sin el header)
-headerDict: Transforma el byte array de header (los primeros 10 bytes de cada mensaje) en un diccionario con la info del header
-dataDict: Transforma el byta array de datos (los bytes luego de los primeros 10) en un diccionario con los datos edl mensaje

'''

    
def response(change:bool=False, status:int=255, protocol:int=255, transport:int=255):
    OK = 1
    CHANGE = 1 if change else 0
    return pack("<BBBBB", OK, CHANGE, status, protocol, transport)


def parseData(packet):
    header = packet[:12]
    data = packet[12:]
    print(f"Largo del header: {len(header)}")
    print(f"Header: {header}")
    print(f"Largo del data: {len(data)}")
    print(f"Data: {data}")
    header = headerDict(header)
    dataD = dataDict(header["protocol"], data)
    if dataD is not None:
        dataSave(header["protocol"], header, dataD)
        
    return None if dataD is None else {**header, **dataD}

def protUnpack(protocol:int, data):
    protocol_unpack = ["<B", "<BB", "<BBfIBf", "<BBfIBff", "<BBfIBffffffff"]
    return unpack(protocol_unpack[protocol], data)

def headerDict(data):
    ID, M1, M2, M3, M4, M5, M6, transport, protocol, leng_msg = unpack("<H6BBBH", data)
    MAC = ".".join([hex(x)[2:] for x in [M1, M2, M3, M4, M5, M6]])
    return {"ID":ID, "MAC":MAC, "transport":transport, "protocol":protocol, "length":leng_msg}

def dataDict(protocol:int, data):
    if protocol not in [0, 1, 2, 3, 4, 5]:
        print("Error: protocol doesnt exist")
        return None
    def protFunc(protocol, keys):
        def p(data):
            unp = protUnpack(protocol, data)
            return {key:val for (key,val) in zip(keys, unp)}
        return p
    p0 = ["Val"]
    p1 = ["Val", "Batt_level"]
    p2 = ["Val", "Batt_level", "Temp", "Pres", "Hum", "Co"]
    p3 = ["Val", "Batt_level", "Temp", "Pres", "Hum", "Co", "RMS"]
    p4 = ["Val", "Batt_level", "Temp", "Pres", "Hum", "Co", "RMS", "Ampx", "Frecx", "Ampy", "Frecy", "Ampz", "Frecz"]
    p5 = ["Val", "Batt_level", "Temp", "Pres", "Hum", "Co", "Accx", "Accy", "Accz"]
    p = [p0, p1, p2, p3, p4, p5]

    try:
        return protFunc(protocol, p[protocol])(data)
    except Exception:
        print("Data unpacking Error:", traceback.format_exc())
        return None