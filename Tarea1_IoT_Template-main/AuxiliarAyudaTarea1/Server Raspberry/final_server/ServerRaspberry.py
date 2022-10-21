import socket
from Desempaquetamiento import parseData, response
from sqlQuery import getConf, setConf
import keyboard


class TCPSocket():

    def __init__(self):
        self.host = ("192.168.4.1", 5010)
        self.sTCP = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sTCP.bind(self.host)
        self.sTCP.listen(2)
        print(f"Listening TCP on {self.host}")
        
    def receive(self):
        try:
            self.conn, self.addr = self.sTCP.accept()
            data = self.TCP_frag_recv()
            print(f"[TCP] Received '{data}' from {self.addr}")
            return parseData(data)
        except Exception as e:
            print(e)
            pass
    
    def send(self, data):
        try:
            self.conn.send(data)
            self.conn.close()
            print(f"[TCP] Sent '{data}' to {self.addr}")
            return data
        except Exception as e:
            pass
    
    def TCP_frag_recv(self):
        doc = b""
        while True:
            try:
                self.conn.settimeout(5)
                data = self.conn.recv(1024)
                if data == b'\0':
                    break
                else:
                    doc += data
            except TimeoutError:
                self.conn.send(b'\0')
                raise
            except Exception:
                self.conn.send(b'\0')
                raise
            self.conn.send(b'\1')
        return doc


class UDPSocket():
    
    def __init__(self):
        self.host = ("192.168.4.1", 5011)
        self.sUDP = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sUDP.bind(self.host)
        print(f"Listening UDP on {self.host}")

    def receive(self):
        try:
            data, self.addr = self.UDP_frag_recv()
            print(f"[UDP] Received '{data}' from {self.addr}")
            return parseData(data)
        except Exception as e:
            pass
    
    def send(self, data):
        try:
            self.sUDP.sendto(data, self.addr)
            print(f"[UDP] Sent '{data}' to {self.addr}")
            return data
        except Exception as e:
            pass
    
    def UDP_frag_recv(self):
        doc = b""
        addr = None
        while True:
            try:
                data, addr = self.sUDP.recvfrom(1024)
                print(data)
                if data == b'\0':
                    break
                else:
                    doc += data
            except TimeoutError:
                raise
            except Exception as e:
                raise
            #s.sendto(b'\1', addr)
        return (doc, addr)


class Server():
    
    def __init__(self):
        self.TCP = TCPSocket()
        self.UDP = UDPSocket()

        self.socket = self.TCP
        self.protocol = 0
        self.transport = 0
        self.change = False

    def get_response(self):
        config = getConf()
        print(config[0])
        if self.protocol != config[0]: 
            self.protocol = config[0]
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
            if data_dict != None:
                # En caso de obtener los datos
                data_resp = self.get_response()
            else:
                # En caso de perder el paquete
                data_resp = response(self.change, 20, self.protocol, self.transport)
        
            # Envia la respuesta
            self.socket.send(data_resp)

            # Hace los cambios necesarios
            #if self.change:
            if self.transport == 0:
                print("Changed to TCP")
                self.socket = self.TCP
            else:
                print("Changed to UDP")
                self.socket = self.UDP
            keyboardHandler(self.protocol, self.transport)
                
            
def keyboardHandler(protocol, transport):
    if keyboard.is_pressed('0'):
        setConf(0, transport)
        print("Cambiado a protocolo 0")
    elif keyboard.is_pressed('1'):
        setConf(1, transport)
        print("Cambiado a protocolo 1")
    elif keyboard.is_pressed('2'):
        setConf(2, transport)
        print("Cambiado a protocolo 2")
    elif keyboard.is_pressed('3'):
        setConf(3, transport)
        print("Cambiado a protocolo 3")
    elif keyboard.is_pressed('4'):
        setConf(4, transport)
        print("Cambiado a protocolo 4")
    elif keyboard.is_pressed('5'):
        setConf(5, transport)
        print("Cambiado a protocolo 5")
        
    elif keyboard.is_pressed('T'):
        setConf(protocol, 0)
        print("Cambiado a transporte TCP")
    elif keyboard.is_pressed('U'):
        setConf(protocol, 1)
        print("Cambiado a transporte UDP")
    
    
    


server = Server()
server.mainLoop()