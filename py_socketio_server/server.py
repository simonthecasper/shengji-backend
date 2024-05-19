import eventlet
import json
import socket
import socketio
import threading

from app_server_connect import AppServerConnect


global asc, app, sio

sio = socketio.Server(cors_allowed_origins='*')
app = socketio.WSGIApp(sio)



@sio.on('connect')
def connect(sid, environ):
    print('New connection:', sid)
    print()


@sio.on('message')
def my_message(sid, data):
    print('Message from', sid, ':', data)
    print()
    

@sio.on('test_message')
def my_message(sid, data):
    print('TestMessage from', sid, ':', data)
    
    asc.sendToServer(data)
    print()    



@sio.on('disconnect')
def disconnect(sid):
    print('Client disconnected:', sid)
    print()



def web_to_server():
    eventlet.wsgi.server(eventlet.listen(('', 12123)), app);


def server_to_web():
    
    while(True):
        server_message_str = asc.receive_message()
        server_message_dict = json.loads(server_message_str)
    pass

    

asc = AppServerConnect()

w2s_thread = threading.Thread(target=web_to_server, name='w2s_thread')
s2w_thread = threading.Thread(target=server_to_web, name='s2w_thread')


# eventlet.wsgi.server(eventlet.listen(('', 12123)), app)


print("asdf")

while(True):
    pass
