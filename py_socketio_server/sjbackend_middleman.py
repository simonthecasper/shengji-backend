import asyncio
import aiohttp
import json
import aiohttp.web
import socketio
import socket
from time import sleep
import functools
from concurrent.futures import ThreadPoolExecutor



SELF_IP = "10.0.0.3"
SIO_SERVER_PORT = 2500
SIO_SERVER_ADDR = (SELF_IP, SIO_SERVER_PORT)

APP_SERVER_IP = "10.0.0.2"
APP_SERVER_PORT = 54329
APP_SERVER_ADDR = (APP_SERVER_IP, APP_SERVER_PORT)

FORMAT = "utf-8"
HEADER_SIZE = 1024

_executor = ThreadPoolExecutor(1)

class AppServerConnect:
    def __init__(self):
        self.app_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.app_server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.app_server.bind(SIO_SERVER_ADDR)
        
        self.read_state = "awaiting_header"
        self.message_length = 1024
        
        while True:
            try:
                self.app_server.connect(APP_SERVER_ADDR)
                print("Connected to AppServer")
                break
            except socket.error as exc:
                print("Failed connection to AppServer...")
                print("Caught exception socket.error : %s" % exc)
                print("Trying again shortly\n")
                sleep(5)

    
    async def receive_message(self):
        loop = asyncio.get_event_loop()
        
        header = ""
        
        # Receive Header
        while len(header) == 0:
            header = await loop.run_in_executor(None, functools.partial(self.app_server.recv, self.message_length))
        
        header_str = header.decode(FORMAT)
        header_dict = json.loads(header_str)
        self.message_length = header_dict["message_length"]
        
        msg = ""
        # Receive body
        while len(msg) == 0:
            msg = await loop.run_in_executor(None, functools.partial(self.app_server.recv, self.message_length))
        
        msg_str = msg.decode(FORMAT)
        print("S2C Message Received: ", msg_str)
        print()
        
        self.message_length = 1024
        
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
        self.app_server.send(final_message_str)


global asc, app, sio, main_thread, s2w_loop, socketio_loop, runner

sio = socketio.AsyncServer(
    async_mode="aiohttp", async_handlers=True, cors_allowed_origins="*"
)
app = aiohttp.web.Application()
sio.attach(app)


@sio.on("connect")
async def connect(sid, environ):
    print("New connection:", sid)
    print()


@sio.on("message")
async def my_message(sid, data):
    data["sid"] = sid
    print("message event")
    print("C2S Received from SID", sid, ":", data)

    message_str = json.dumps(data)
    print("Forwarding message to App Server:", message_str)
    print()

    asc.sendToServer(message_str)
    print()


@sio.on("test_message")
async def my_message(sid, data):
    data["sid"] = sid
    print("test_message event")
    print("C2S Received from SID", sid, ":", data)

    message_str = json.dumps(data)
    print("Forwarding message to App Server:", message_str)
    print()

    asc.sendToServer(message_str)


@sio.on("disconnect")
async def disconnect(sid):
    print("Client disconnected:", sid)
    print()


async def server_to_web():
    print("in Server2Web...")
    while True:
        server_message_str = await asc.receive_message()

        if len(server_message_str) > 0:
            server_message_dict = json.loads(server_message_str)

            sid = server_message_dict["sid"]

            server_message_dict.pop("sid", None)
            server_message_str = json.dumps(server_message_dict)

            print("S2C sending message to client:", server_message_dict)
            await sio.emit(event="server_message", data=server_message_str, room=sid)


async def main():
    global asc, app, sio
    asc = AppServerConnect()

    loop = asyncio.get_event_loop()

    await asyncio.gather(
        aiohttp.web._run_app(app, host="0.0.0.0", port=SIO_SERVER_PORT), server_to_web()
    )


if __name__ == "__main__":
    asyncio.run(main())
