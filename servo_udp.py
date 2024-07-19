import socket

ip = "0.0.0.0"
port = 8888

soc = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
soc.bind((ip,port))
while True:
    data , addr = soc.recvfrom(1024)
    print("Recieved data:  ",data.decode())
    