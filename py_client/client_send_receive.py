import socket
import json
import sys
import threading

HEADER_SIZE = 1024
PORT = 12123

SERVER = "192.168.0.77"

FORMAT = "utf-8"
DISCONNECT_MESSAGE = "!DISCONNECT"
ADDR = (SERVER, PORT)


worker_end = False
start_second_thread = False

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(ADDR)
client.settimeout(1)
is_connected = True


user_name = input("enter your name:").strip()
while len(user_name) > 20:
    user_name = input("Please enter a shorter name:")


session_id = ""
player_id = 0


def sendChatMessage(message_str):
    message_dict = {
        "session_id": session_id,
        "player_id": player_id,
        "username": user_name,
        "stage": "chat",
        "message": message_str,
    }
    sendToServer(message_dict)


def sendToServer(message_dict):
    final_message_str = json.dumps(message_dict).encode(FORMAT) + b"\0"

    header_dict = {"message_size": 0, "filler": ""}
    header_dict["message_size"] = int(str(len(final_message_str)))
    header_dict["filler"] += " " * (HEADER_SIZE - 1 - len(json.dumps(header_dict)))

    final_header_str = json.dumps(header_dict).encode(FORMAT) + b"\0"

    client.send(final_header_str)
    # print(final_header_str)

    client.send(final_message_str)
    # print(final_message_str)


def setSessionPlayerID(response_dict):
    global session_id
    global player_id

    session_id = response_dict["session_id"]
    player_id = response_dict["player_id"]

    print()
    print("session_id:", session_id)
    print()
    # print("player_id:", player_id)


def makeNewSession():
    global session_id
    global player_id

    message_dict = {"stage": "prelobby", "task": "new_session"}
    sendToServer(message_dict)

    response = ""
    response_dict = {}
    while True:
        try:
            response = client.recv(1024).decode(FORMAT)
            response_dict = json.loads(response)
            break
        except:
            pass

    setSessionPlayerID(response_dict)


def joinSession():
    global player_id
    global session_id

    message_dict = {
        "stage": "prelobby",
        "task": "join_session",
        "session_id": session_id,
    }
    sendToServer(message_dict)

    response = message = client.recv(1024).decode(FORMAT)
    response_dict = json.loads(response)

    setSessionPlayerID(response_dict)


def receiveServerMessage():
    global worker_end
    while not worker_end:
        # print("trying to read")
        try:
            message_str = client.recv(1024).decode(FORMAT)
            message_dict = json.loads(message_str)
            if message_dict["stage"] == "chat":
                print(message_dict["username"] + ":" + message_dict["message"])
        except:
            pass
    print("receive thread exited")


def menu():
    global start_second_thread
    global session_id

    print("1: Make new session")
    print("2: Join existing session")
    user_input = input("Please make a selection:")
    while int(user_input) > 3:
        user_input = input("Please make a proper selection:")

    if user_input == "1":
        print("Making new session...")
        makeNewSession()
    elif user_input == "2":
        print("Joining session...")
        session_id = input("Enter the session ID:")
        joinSession()

    start_second_thread = True
    readUserInput()


def readUserInput():
    global worker_end
    while not worker_end:
        user_input = input(">")

        if user_input == "exit":
            worker_end = True
            break
        else:
            sendChatMessage(user_input)
    print("input thread exited")


thread0 = threading.Thread(target=menu)
thread0.start()

while True:
    if start_second_thread == True:
        break


thread1 = threading.Thread(target=receiveServerMessage)
thread1.start()

thread0.join()
thread1.join()
print("all threads joined")
