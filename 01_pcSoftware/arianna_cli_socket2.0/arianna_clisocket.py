#!/usr/bin/python
# -*- coding: Latin-1 -*-
'''
Created on 21/set/2017

@author: a.airaghi

>p9300   per cambiare tempo rilevamento posizione

'''

from tkinter import Tk
import threading
import config as cfg
import time
import  arianna_db 
import socket
import arianna_utility
import subprocess
import arianna_web
import math
import arianna_gui
import sys
#import navigazione as navi

statosmf = threading.Semaphore()


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#ip locale
ip=([l for l in ([ip for ip in socket.gethostbyname_ex(socket.gethostname())[2] if not ip.startswith("127.")][:1], [[(s.connect(('8.8.8.8', 53)), s.getsockname()[0], s.close()) for s in [socket.socket(socket.AF_INET, socket.SOCK_DGRAM)]][0][1]]) if l][0][0])
UDP_IP = ip
UDP_PORT = 8888
soudp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
soudp.bind((UDP_IP, UDP_PORT))


datipostxt=open("pos.csv","w")
inviocmd=open("cmd.txt","w")
datipostxt.write("dati\n")
datipostxt.close()

#ipclient= '192.168.88.129'
def ricerca_arianna(sock):
    sock.settimeout(3.0)
    try:
        data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
        print("ip",data,addr)
        return addr[0]
    except:
        print("non trovo arianna")
        return '0'



simu=1
attcom=0
cont=0
while attcom==0:
    
    if simu==0:
        
        file=cfg.localpath+"\\simulatore.py"
        command1 = subprocess.Popen([sys.executable,file], shell=True)
        ipclient='127.0.0.1'
        TCP_PORT = 8181
        BUFFER_SIZE = 256
        attcom=1
    elif simu==1:
        
        ipclient=''
        print("cerco arianna")
        a=ricerca_arianna(soudp)
        if len(a)>6:
            ipclient=a
            attesa_arianna=0
            TCP_PORT = 81
            BUFFER_SIZE = 256
            attcom=1
        cont+=1
        if cont>1:
            simu=0
            print("vado in simulazione")




        
class comunicazione (threading.Thread):
    def __init__(self, threadID, name):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
    
    def run(self):
        messaggio=""
        while 1:
            try:
                s.settimeout(3) 
                s.connect((ipclient, TCP_PORT))    #aggiungere verifica se funziona
            except socket.error as msg:
                if str(msg)[:45]=="[WinError 10056] Richiesta di connessione ino":
                    pass;
                else:
                    pass;
            messaggio=self.risp_ari(messaggio)
            if cfg.posatt[5]=='0':
                self.com_ari_mov()
            self.com_ari_altro()
            time.sleep(0.1)


            
    def risp_ari(self,messaggio):   #acquisizione risposte da arianna, da mettere nella coda giusta
        s.settimeout(0)
        try:
            raw_bytestream = s.recv(BUFFER_SIZE)      #metto nella coda di ricezione le info di arduino
            messaggio+=str(raw_bytestream)
            arianna_utility.prt(messaggio,1,my_gui)
            mex=arianna_utility.gestiscirisp(messaggio)

            for m in mex[0]:

                if m[0:3]=='mis':
                    cfg.dist_libera=int(m.split(";")[1])
                    #print(m)
                elif m[0:3]=='pos':
                    cfg.messaggiesppos.put(m)
                    print(m)
                    arianna_utility.prt(m, 2, my_gui)
                elif m[0:4]=='echo':

                    cfg.messaggiesprx.put(m)
                elif m[0:2]=='ir':
                    
                    cfg.messaggiesprx.put(m)
                    print(m)
                
                elif m[0:2]=='r:':
                    if m[0:4]=='r: 0':
                        chiave=m.split(";")[1]
                        if chiave in cfg.richieste_fermo:
                            statosmf.acquire()
                            cfg.stato[0]=1  #sblocco successivi movimenti
                            statosmf.release()
                            cfg.richieste_fermo=[]
                            print("sblocco")
                        else:
                            if cfg.stato[0]==1: 
                                print("chiave non riconosciuta")
                    cfg.messaggiesprx.put(m)

                else:
                    cfg.messaggiesprx.put(m)
            messaggio=""
            for m in mex[1]:
                messaggio+m
                return messaggio
        except Exception as msg:
            if str(msg).find("[WinError 10035]")<0:
                print(msg)
                messaggio=''
            return messaggio
        
        
    def com_ari_mov(self):   #invio comandi per arianna coda movimento
        '''devo gestire l'attesa del fermo la gestisco mettendo la variabile  moto=1 dopo un R e chiamando
        un r con una chiave precisa a ripetizione solo quando mi torna un r:0 con chiave giusto rimetto a 0
        '''
        destinatario='M'      #M= arduino MEGA   E=ESP 
        if cfg.messaggiesptx.empty()==False and cfg.stato[0]<=0 and time.time()>cfg.timer_sleep:
            time.sleep(0.3)
            #mystring="!"+cfg.messaggiesptx.get()+"?"
            mystring=cfg.messaggiesptx.get()
            if mystring.find("sleep")>=0:  #addormento procedura per n secondi
                mio_sl=int(mystring.split(";")[1])
                mio_sl=time.time()+mio_sl
                cfg.timer_sleep=mio_sl
                return

            if mystring.find("1q")>=0:
                cfg.time_radar=1

                mystring=arianna_utility.cmdradar(mystring)    #trovo verso
            if mystring.find("3R")>=0:  #se e movimento svuoto le richieste di attese
                cfg.richieste_fermo=[]
            if mystring.find("1r")>=0:   #attesa
                statosmf.acquire()
                cfg.stato[0]=2
                statosmf.release()
                idunivoco=arianna_utility.idmsg()
                cfg.richieste_fermo.append(idunivoco)
                mystring=mystring+idunivoco
                
            if mystring.find("3A")>=0:   #attesa
                angnew=arianna_utility.minimoangolo(float(cfg.posatt[3]), float(mystring[2:]))
                mystring="3A"+str(angnew)
            mystring="!"+mystring+"?"
            print("mystring",mystring)
            t=bytes(mystring, 'utf-8')
            try:
                inviocmd=open("cmd.txt","a")
                inviocmd.write(mystring+"\n")
                inviocmd.close()
                s.send(t)
            except:
                print("errore coda mov")
                pass
            mystring=""
        if cfg.stato[0]==2:  #mando una nuova richiesta di fermo
            time.sleep(0.2)
            idunivoco=arianna_utility.idmsg()
            cfg.richieste_fermo.append(idunivoco)
            mystring="!1r"+idunivoco+"?"
            t=bytes(mystring, 'utf-8')
            try:
                inviocmd=open("cmd.txt","a")
                inviocmd.write(mystring+"\n")
                inviocmd.close()
                s.send(t)
            except:
                print("errore coda mov")
                pass
            mystring="" 
        
        if cfg.stato[0]==1:  #mando una nuova richiesta di posizione
            time.sleep(0.2)
            idunivoco=arianna_utility.idmsg()
            cfg.richieste_pos.append(idunivoco)
            mystring="!1p"+idunivoco+"?"
            t=bytes(mystring, 'utf-8')
            try:
                inviocmd=open("cmd.txt","a")
                inviocmd.write(mystring+"\n")
                inviocmd.close()
                s.send(t)
            except:
                print("errore coda mov")
                pass
            mystring="" 
    
    def com_ari_altro(self): #invio comandi per arianna altro non movimento
        if cfg.messaggiesptx_altro.empty()==False:
            mystring="!"+cfg.messaggiesptx_altro.get()+"?"
            t=bytes(mystring, 'utf-8')
            time.sleep(0.2)
            try:
                s.send(t)
            except:
                print("errore coda altro")
                pass
 

    
class elabora (threading.Thread):
    def __init__(self, threadID, name):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name      
    
    def run(self):
        while 1:
                
            if  cfg.messaggiesprx.empty()==False:
                msgxx=cfg.messaggiesprx.get()
                print("msg generico",msgxx)
                if  msgxx[0:4]=='echo' :
                    print(msgxx)  
                    #arianna_utility.crea_mappa(msgxx,cfg.mappa,"assoluta",cfg.versoradar,cfg.posatt)
                    arianna_utility.trovadistanza(msgxx)   
                
                if  msgxx[0:4]=='echf' :
                    print('----------echf------------')  
                    arianna_utility.inizializza_mappa()
                    cfg.time_radar=2
                    print("finito radar")

            
            if  cfg.messaggiesppos.empty()==False:
                posxyt=cfg.messaggiesppos.get()
                newpos=arianna_utility.deco_pos(str(posxyt))
                if newpos[8] in cfg.richieste_pos and newpos[8]!='':
                    statosmf.acquire()
                    cfg.stato[0]=0  #sblocco successivi movimenti
                    statosmf.release()
                    cfg.richieste_pos=[]
                    print("pos acquisita")
                if arianna_utility.controlla_new_pos(cfg.posatt, newpos):
                    cfg.posatt=newpos
                    cfg.posatt[2]=str(float(cfg.posatt[2])*cfg.invx)
                
            
            self.elaborarich()
    
        
    def elaborarich(self):
        
        if cfg.messaggirx.empty()==False:
            lavoro=cfg.messaggirx.get()
            cfg.messaggiesptx.put(lavoro[1])
        
        if cfg.percorsi.empty()==False and cfg.stato[0]==0 and cfg.posatt[5]=='0' :
            statosmf.acquire()
            cfg.stato[0]=-1
            statosmf.release()
            cfg.posatt[5]='1'
                
            destinazione=cfg.percorsi.get()
            a,dist=arianna_utility.calcola_movimento_inv(destinazione[1][0], destinazione[1][1], destinazione[1][2])
            if (dist<100):  #se la distanza è minore di 10 cm mi considero arrivato , mettere parametro?
                statosmf.acquire()
                cfg.stato[0]=0
                statosmf.release()
            else:
                cfg.percorsi.put(destinazione)
                for cmd in a:
                    cfg.messaggirx.put((time.time(),cmd))
                    time.sleep(0.1)
            #cfg.percorsi.put(destinazione)


class mappa (threading.Thread):
    def __init__(self, threadID, name):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name      
    
    def run(self):
        while 1:
            if cfg.time_radar==2 :
                x=arianna_db.leggosql([], 'mappe_elaborare')
                if len(x)>0:
                    for i in x:
                        arianna_db.leggosql(['',cfg.id_radar],'deletec')
                        punti=arianna_db.leggosql(i,'mappa')
                        for i in punti:
                            arianna_utility.crea_mappa('',cfg.mappa,"assoluta","dx",[i[0],i[1],math.radians(i[4])],1,i[2],i[3])
                cfg.time_radar=0    
            else:
                time.sleep(2)
            
# **********param default****
cfg.messaggirx.put((time.time(),'>p9600'))
time.sleep(0.2)
cfg.messaggirx.put((time.time(),'3F'+cfg.ED))
time.sleep(0.2)
cfg.messaggirx.put((time.time(),'3F1'+cfg.ED_BASE))
time.sleep(0.2)
cfg.messaggirx.put((time.time(),'3F2'+cfg.BASELINE))
time.sleep(0.2)
temp=round(cfg.DIAM_RUOTA*3.14/(4*cfg.encoderppr), 4)
print ("temp",temp)
cfg.messaggirx.put((time.time(),'3F3'+str(temp)))
time.sleep(0.2)
cfg.messaggirx.put((time.time(),'3E3'))
#****************************
cfg.id_radar=arianna_utility.idmap()
print("id mappa", cfg.id_radar)
#nuova mappa avvio 

thread1 = arianna_web.serverweb(1, "Thread-1")
thread2 = comunicazione(2, "Thread-com1")
thread3 = elabora(3, "Thread-ela1")
thread4 = mappa(4, "Thread-map1")

# Start new Threads
thread1.start()
thread2.start()
thread3.start()
thread4.start()

root = Tk()
my_gui = arianna_gui.MyFirstGUI(root)
root.mainloop()

print ("Exiting Main Thread")