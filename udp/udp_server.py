import socket
BUFSIZE = 1024
ip_port = ('10.85.25.87', 9999)
server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server.bind(ip_port)
while True:
    data,client_addr = server.recvfrom(BUFSIZE)
    print('server receive:', data)

    server.sendto(data.upper(),client_addr)
server.close()
