import socket

# UDP server information
udp_ip = "0.0.0.0"  # Listen on all available interfaces
udp_port = 12345  # Port your ESP32 is sending UDP packets to

# Create UDP socket
udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Bind the socket to the IP address and port
udp_socket.bind((udp_ip, udp_port))

print("UDP server started. Waiting for data...")

# Receive and print incoming UDP packets
while True:
    data, addr = udp_socket.recvfrom(1024)  # Buffer size is 1024 bytes
    print("Received message:", data.decode("utf-8"))

# Close the UDP socket (not necessary in this example)
udp_socket.close()