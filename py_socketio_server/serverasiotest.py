import asyncio
import aiohttp
import json
import socket
import aiohttp.web
import socketio
import threading

from app_server_connect import AppServerConnect
from TSQueue import TSQueue


SIO_SERVER_PORT = 12123

global asc, app, sio, main_thread, s2w_loop, socketio_loop, runner

s2w_queue = TSQueue()


sio = socketio.AsyncServer(async_mode='aiohttp', 
                           async_handlers=True, 
                           cors_allowed_origins='*')
app = aiohttp.web.Application()
sio.attach(app)


@sio.on('*')
def catch_all(event, sid, *args):
    data = args[0]
    
    if event == 'test_message':
        data["sid"] = sid
        print('TestMessage with SID from', sid, ':', data)
        
        message_str = json.dumps(data)
        print("Sending message to App Server:", message_str)
        print(">>Thread:", threading.current_thread().ident)
        print()
        
        asc.sendToServer(message_str)
        print()
        
    # await server_to_web()
    

@sio.on('connect')
async def connect(sid, environ):
    print('New connection:', sid)
    # print(">>Thread:", threading.current_thread().ident)
    print()


# @sio.on('message')
# async def my_message(sid, data):
#     print('Message from', sid, ':', data)
#     print()


# @sio.on('test_message')
# async def my_message(sid, data):
    
#     data["sid"] = sid
#     print('TestMessage with SID from', sid, ':', data)
    
#     # print('Returning message to sid:', sid)
#     # sio.emit(event="server_message", data=data, room=sid)
    
#     message_str = json.dumps(data)
#     print("Sending message to App Server:", message_str)
#     print(">>Thread:", threading.current_thread().ident)
#     print()
    
#     asc.sendToServer(message_str)
#     print()


@sio.on('disconnect')
async def disconnect(sid):
    print('Client disconnected:', sid)
    print()


async def server_to_web():
    # print("one")
    # await asyncio.sleep(1)
    # print("two")
    
    print("in Server2Web")
    while True:
        await asyncio.sleep(0.1)
        print(">In s2w loop...")
        
        server_message_str = await asc.receive_message()
        
        print("server_message_str", server_message_str)
        
        server_message_dict = json.loads(server_message_str)
        
        sid = server_message_dict["sid"]
        print("Sending message to web:", server_message_dict)
        await sio.emit(event="server_message", data=server_message_str, room=sid)
        # print("Received Message from AppServer:", server_message_str)
        # print("Adding Message to s2w_queue:", server_message_str)
        # s2w_queue.enqueue(server_message_str)



async def main():
    global asc, app, sio
    asc = AppServerConnect()
    
    loop = asyncio.get_event_loop()
    
    # ####### socketio server events
    # runner = aiohttp.web.AppRunner(app)
    # await runner.setup()
    # # loop.run_until_complete(runner.setup)
    
    # site = aiohttp.web.TCPSite(runner=runner, host='0.0.0.0', port=12123)
    # await site.start()
    # # loop.run_until_complete(site.start())
    
    
    # ####### server to web events
    # task = asyncio.create_task(server_to_web())
    # # asyncio.ensure_future(server_to_web(), loop=loop)
    
    
    # # loop.run_forever()
    # await asyncio.Event().wait()
    
    
    await asyncio.gather(
        aiohttp.web._run_app(app, host='0.0.0.0', port=SIO_SERVER_PORT),
        server_to_web()
    )
    
    
    


if __name__ == "__main__":
    
    asyncio.run(main())
    
    # main()
    
    # asc = AppServerConnect()
    # aiohttp.web.run_app(app, host='0.0.0.0', port=SIO_SERVER_PORT)

