'''
Created on 02/mag/2018

@author: a.airaghi
'''


import socket
import threading


def ricerca_arianna(sock):
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    print("ip",data,addr)
    return addr[0]


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
ip=([l for l in ([ip for ip in socket.gethostbyname_ex(socket.gethostname())[2] if not ip.startswith("127.")][:1], [[(s.connect(('8.8.8.8', 53)), s.getsockname()[0], s.close()) for s in [socket.socket(socket.AF_INET, socket.SOCK_DGRAM)]][0][1]]) if l][0][0])
UDP_IP = ip
UDP_PORT = 8888
soudp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
soudp.bind((UDP_IP, UDP_PORT))


simu=1

if simu==0:
    
    pass;
elif simu==1:
    #ipclient='192.168.43.235'
    attesa_arianna=1
    while attesa_arianna==1:
        #ipclient='192.168.1.102'
        ipclient=''
        print("cerco arianna")
        a=ricerca_arianna(soudp)
        if len(a)>12:
            ipclient=a
            attesa_arianna=0
    TCP_PORT = 81
    BUFFER_SIZE = 256
else:
    ipclient='192.168.1.102'
    TCP_PORT = 81
    BUFFER_SIZE = 256

s.settimeout(3) 
s.connect((ipclient, TCP_PORT)) 


class leggo(threading.Thread):
    def __init__(self, threadID, name):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
    def run(self):
        while 1:
            raw_bytestream = s.recv(BUFFER_SIZE) 
            print(str(raw_bytestream))
        
        
        
        
        
class scrivo(threading.Thread):
    def __init__(self, threadID, name):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name 
    def run(self): 
        while 1:    
            a=input("metti comando")
            a="!"+a+"?"
            t=bytes(a, 'utf-8')
            s.send(t)



thread1 = leggo(1, "Thread-1")
thread2 = scrivo(2, "Thread-com1")
thread1.start()
thread2.start()



