import socket
from Desempaquetamiento import parseData, response
from sqlQuery import getConf


class TCPSocket():

    def __init__(self):
        self.host = ("192.168.4.1", 5010)
        self.sTCP = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sTCP.bind(self.host)
        self.sTCP.listen(2)
        print(f"Listening TCP on {self.host}")
        
    def connect(self):
        self.conn, self.addr = self.sTCP.accept()
        
    def receive(self):
        try:
            data = self.conn.recv(1024)
            print(f"Received '{data}' from {self.addr}")
            return parseData(data)
        except Exception as e:
            pass
    
    def send(self, data):
        try:
            data = self.sTCP.send(data)
            print(f"Sent '{data}' to {self.addr}")
            return resp
        except Exception as e:
            pass
    
    def disconnect(self):
        self.conn.close()


class UDPSocket():
    
    def __init__(self):
        self.host = ("192.168.4.1", 5011)
        self.sUDP = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sUDP.bind(self.host)
        print(f"Listening UDP on {self.host}")

    def receive(self):
        try:
            data, self.addr = sUDP.recvfrom(1024)
            print(f"Received '{data}' from {self.addr}")
            return parseData(data)
        except Exception as e:
            pass
    
    def send(self, data):
        try:
            data = self.sUDP.sendto(data, self.addr)
            print(f"Sent '{data}' to {self.addr}")
            return data
        except Exception as e:
            pass


class Server():
    
    def __init__(self):
        self.TCP = TCPSocket()
        self.UDP = UDPSocket()
        self.TCP.connect()

        self.socket = self.TCP
        self.protocol = 0
        self.transport = 0
        self.change = False

    def get_response():
        config = getConf()
        if self.protocol != config[0]: 
            self.protocol = config[0]
            self.change = True
        if self.transport != config[1]:
            self.transport = config[1]
            self.change = True
        else:
            self.change = False

        return response(self.change, 10, self.protocol, self.transport)


    def mainLoop(self):
        while True:
            # Recibe un valor
            data_dict = self.socket.receive()

            # Obtiene la respuesta
            if resp != None:
                # En caso de obtener los datos
                data_resp = self.get_response()
            else:
                # En caso de perder el paquete
                data_resp = response(self.change, 20, self.protocol, self.transport)
        
            # Envia la respuesta
            self.socket.send(data_resp)

            # Hace los cambios necesarios
            if self.change:
                if self.transport == 0:
                    self.socket = self.TCP
                    self.TCP.connect()
                else:
                    self.socket = self.UDP
                    self.TCP.disconnect()

server = Server()
server.mainLoop()