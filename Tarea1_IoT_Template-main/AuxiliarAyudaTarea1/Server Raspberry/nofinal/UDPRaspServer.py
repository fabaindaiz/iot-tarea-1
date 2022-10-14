import socket
from Desempaquetamiento import parseData, response

UDP_IP = "192.168.4.1"# "localhost" 
UDP_PORT = 5010

sUDP = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sUDP.bind((UDP_IP, UDP_PORT))


print(f"Listening for UDP packets in {UDP_IP}:{UDP_PORT}")
while True:

    while True:
        payload, client_address = sUDP.recvfrom(1024)
        print(f"Received packet from {client_address}.")
        parseData(payload)
        print("Data parsed and inserted into DB")
        print(f"Sending response to {client_address}.")
        sent = sUDP.sendto(bytes("OK: ", 'utf-8'), client_address)