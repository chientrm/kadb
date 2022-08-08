import os
import signal
import subprocess
import time

server_cmd = "dist/server"
server_ready_pattern = "Server is running on port"


def start_testing():
    print("Start testing")


def start_server():
    server = subprocess.Popen(server_cmd)
    time.sleep(1)
    start_testing()
    os.kill(server.pid, signal.SIGTERM)
    print("![test  ] server killed")


if __name__ == '__main__':
    start_server()
