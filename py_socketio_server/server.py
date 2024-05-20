import eventlet
import json
import socket
import socketio
import threading

from app_server_connect import AppServerConnect


SIO_SERVER_PORT = 12123

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
    
    data["sid"] = sid
    print('TestMessage with SID from', sid, ':', data)
    
    message_str = json.dumps(data)
    print("Sending message to App Server:", message_str)
    
    asc.sendToServer(message_str)
    print()    


@sio.on('disconnect')
def disconnect(sid):
    print('Client disconnected:', sid)
    print()



def web_to_server():
    eventlet.wsgi.server(eventlet.listen(('', SIO_SERVER_PORT)), app);


def server_to_web():
    
    while(True):
        server_message_str = asc.receive_message()
        
        print("Received Message from AppServer:", server_message_str)
        
        server_message_dict = json.loads(server_message_str)
        
        sid =  server_message_dict["sid"]
        server_message_dict.pop("sid", None)
        
        server_message_str = json.dumps(server_message_dict)
        
        print("Sending message to SID:", sid)
        print(">>Message:", server_message_dict)
        print()
        
        sio.emit(event="server_message", data=server_message_str, to=sid)




asc = AppServerConnect()

w2s_thread = threading.Thread(target=web_to_server, name='w2s_thread')
s2w_thread = threading.Thread(target=server_to_web, name='s2w_thread')

w2s_thread.start()
s2w_thread.start()


w2s_thread.join()
s2w_thread.join()
