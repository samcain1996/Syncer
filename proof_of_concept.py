import socket
import os

# Map file names to file contents
files = dict()

host = '192.168.50.160'
port = '3000'

filename = 'readme.md'

files[filename] = "test"

def UpdateFile(filename, data) -> bool:
    if filename in files.keys():
        files[filename] = data
        return True
    
    return False

def AddFile(filename, data) -> bool:
    files[filename] = data
    return True

def ConnectToServer(host, port) -> socket:
    sock = socket.socket()

    sock.connect((host, port))
    

    return sock
#filesize = os.path.getsize(filename)

#sock = socket.socket()

#sock.connect((host, port))