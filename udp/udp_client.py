
import socket
import time

BUFSIZE = 1024
client = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
while 1:
    msg = "test udp, buffsize is 1024"
    ip_port = ('127.0.0.1', 9999)

    start = time.time()
    client.sendto(msg.encode('utf-8'), ip_port)

    data,server_addr = client.recvfrom(BUFSIZE)
    end = time.time()

    print('client recvfrom:', data, server_addr)
    print('cost:', end-start)
    break

client.close()
