import json
import socket
import select
import sys
import threading
from time import sleep
import asyncio
import functools

from concurrent.futures import ThreadPoolExecutor


SERVER_IP = "127.0.0.1"
SERVER_PORT = 54329
ADDR = (SERVER_IP, SERVER_PORT)

FORMAT = "utf-8"
HEADER_SIZE = 1024


_executor = ThreadPoolExecutor(1)

class AppServerConnect:
    def __init__(self):
        self.app_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.app_server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.app_server.bind(('localhost', 2000))
        
        while True:
            try:
                self.app_server.connect(ADDR)
                print("Connected to AppServer")
                break
            except:
                print("Failed connection to AppServer...")
                print("Trying again shortly\n")
                sleep(5)

    
    async def receive_message(self):
        loop = asyncio.get_event_loop()
        
        msg = await loop.run_in_executor(None, functools.partial(self.app_server.recv, 1024))
        print("msg:", msg)
        
        msg_str = msg.decode(FORMAT)
        print(msg_str)
        
        return msg_str
    
    
    async def looping(self):
        await asyncio.sleep(0)
    
    
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

        