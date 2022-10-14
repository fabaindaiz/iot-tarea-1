import socket
from Desempaquetamiento import parseData, response
from getConf import getConf

host = ("192.168.4.1", 5010)

class UDPSocket():
    
    def __init__(self):
        self.sUDP = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sUDP.bind(host)
    
    def receive(self):
        data, address = sUDP.recvfrom(1024)
        print(f"Received packet from {address}.")
        dicts = parseData(data)
        print("Data parsed and inserted into DB")
        print(f"Recibido {data}")
        sent = self.sUDP.sendto(bytes("OK: ", 'utf-8'), address)
        return dicts
    
    def disconnect(self):
        self.sUDP.close()

class TCPSocket():

    def __init__(self):
        self.sTCP = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sTCP.bind(host)
        self.sTCP.listen(5)
        print(f"Listening on {host}")
        self.conn, self.addr = s.accept()

    def receive(self):
        while True:
            try:
                data = self.conn.recv(1024)
                print(f"Received packet from {self.addr}.")
                if data != b'':
                    dicts = parseData(data)
                    print("Data parsed and inserted into DB")
                    print(f"Recibido {data}")
                    return dicts
            except ConnectionResetError:
                pass
    
    def disconnect(self):
        self.conn.close()
        self.sTCP.close()


class Server():
    
    def __init__(self):
        self.socket = TCPSocket()
        self.send_config()
        self.socket.disconnect()

    def send_config(self):
        config = getConf()
        # Envia config

    def receive(self):
        dicts = self.socket.receive()
    
        
