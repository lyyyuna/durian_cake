import socket
import argparse
import select
import os
import sys

parser = argparse.ArgumentParser(description='netcat')
parser.add_argument('--server', default=False, action='store_true')
parser.add_argument('--ip', default='127.0.0.1', type=str)
parser.add_argument('--port', default=5555, type=int)

args = parser.parse_args()

def relay(sock: socket.socket):
    poll = select.poll()
    poll.register(sock, select.POLLIN)
    poll.register(sys.stdin, select.POLLIN)

    done = False
    while not done:
        events = poll.poll(10000)
        for fileno, event in events:
            if event & select.POLLIN:
                if fileno == sys.stdin.fileno():
                    data = os.read(sys.stdin.fileno(), 8192)
                    if data:
                        sock.sendall(data)
                    else:
                        sock.shutdown(socket.SHUT_WR)
                        poll.unregister(sys.stdin)
                else:
                    data = sock.recv(8192)                    
                    if data:
                        os.write(sys.stdout.fileno(), data)
                    else:
                        done = True


if args.server == True:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(('', args.port))
    sock.listen(5)
    (client_sock, _) = sock.accept()
    relay(client_sock)
else:
    sock = socket.create_connection((args.ip, args.port))
    relay(sock)