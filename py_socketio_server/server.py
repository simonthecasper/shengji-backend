import eventlet
import json
import socket
import socketio
import threading

from app_server_connect import AppServerConnect
from TSQueue import TSQueue


SIO_SERVER_PORT = 12123

global asc, app, sio, main_thread

s2w_queue = TSQueue()

sio = socketio.AsyncServer(cors_allowed_origins='*')
app = socketio.ASGIApp(sio)



@sio.on('connect')
def connect(sid, environ):
    print('New connection:', sid)
    print(">>Thread:", threading.current_thread().ident)
    print()


@sio.on('message')
def my_message(sid, data):
    print('Message from', sid, ':', data)
    print()


@sio.on('test_message')
def my_message(sid, data):
    
    data["sid"] = sid
    print('TestMessage with SID from', sid, ':', data)
    
    # print('Returning message to sid:', sid)
    # sio.emit(event="server_message", data=data, room=sid)
    
    message_str = json.dumps(data)
    print("Sending message to App Server:", message_str)
    print(">>Thread:", threading.current_thread().ident)
    print()
    
    asc.sendToServer(message_str)
    print()    


@sio.on('disconnect')
def disconnect(sid):
    print('Client disconnected:', sid)
    print()



def web_to_server():
    while(True):
        pass
    # eventlet.wsgi.server(eventlet.listen(('', SIO_SERVER_PORT)), app);


def server_to_web():
    while(True):
        server_message_str = asc.receive_message()
        
        print("Received Message from AppServer:", server_message_str)
        print("Adding Message to s2w_queue:", server_message_str)
        s2w_queue.enqueue(server_message_str)


def poll_s2w():
    while(True):
        if not s2w_queue.is_empty():
            server_message_str = s2w_queue.dequeue()
        
            print("Removed Message from s2w_queue:", server_message_str)
            
            server_message_dict = json.loads(server_message_str)
            
            sid =  server_message_dict["sid"]
            server_message_dict.pop("sid", None)
            
            server_message_str = json.dumps(server_message_dict)
            
            print("Sending message to SID:", sid)
            print(">>Message:", server_message_dict)
            print(">>Thread:", threading.current_thread().ident)
            print(">>ThreadMatch:", threading.current_thread().ident == main_thread)
            print()
            
            sio.emit(event="server_message", data=server_message_str, room=sid)


main_thread = threading.current_thread().ident
print("\n*************************************")
print("Main Thread :", threading.current_thread().ident)
print("************************************")

asc = AppServerConnect()

# w2s_thread = threading.Thread(target=web_to_server, name='w2s_thread')
s2w_thread = threading.Thread(target=server_to_web, name='s2w_thread')
# w2s_thread.start()
s2w_thread.start()


eventlet.wsgi.server(eventlet.listen(('', SIO_SERVER_PORT)), app);

print("asdflasdjkl;fdsajkl;asfdjkl;asjkl;dfjkl;asdjkl;")

poll_s2w()


