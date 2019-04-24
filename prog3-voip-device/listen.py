import socket

BROADCAST_PORT = 4097
STREAMING_PORT = 4098

client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
client.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
client.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
client.bind(("", BROADCAST_PORT))
while True:
    data, addr = client.recvfrom(1024)
    print("received message: %s"%data)
    