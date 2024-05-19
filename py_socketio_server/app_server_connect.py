import json
import socket
import threading



SERVER_IP = "127.0.0.1"
SERVER_PORT = 54329
ADDR = (SERVER_IP, SERVER_PORT)

FORMAT = "utf-8"
HEADER_SIZE = 1024


class AppServerConnect:
    def __init__(self):
        self.app_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.app_server.connect(ADDR)
    
    
    def receive_message(self):
        while True:
            try:
                message_str = self.app_server.recv(1024).decode(FORMAT)
                return message_str
            except:
                pass
    
        
        
    def sendToServer(self, message_str):
        final_message_str = message_str.encode(FORMAT) + b"\0"

        header_dict = {"message_size": 0, "filler": ""}
        header_dict["message_size"] = int(str(len(final_message_str)))
        header_dict["filler"] += " " * (HEADER_SIZE - 1 - len(json.dumps(header_dict)))

        final_header_str = json.dumps(header_dict).encode(FORMAT) + b"\0"

        self.app_server.send(final_header_str)
        # print(final_header_str)

        self.app_server.send(final_message_str)
        # print(final_message_str)

        