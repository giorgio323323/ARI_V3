#!/usr/bin/python
# -*- coding: Latin-1 -*-
'''
Created on 21/set/2017

@author: a.airaghimessaggiesptx

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
import arianna_webmon
import math
import arianna_gui
import sys
import webbrowser
#import navigazione as navi

statosmf = threading.Semaphore()  #semaforo globale
semaelabora = threading.Semaphore()  #semaforo percorsi


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
    elif simu==2:
        ipclient="0.0.0.0"
        TCP_PORT = 81
        BUFFER_SIZE = 256
        attcom=1
        
try:
    s.settimeout(3) 
    s.connect((ipclient, TCP_PORT))    #aggiungere verifica se funziona
except socket.error as msg:
    if str(msg)[:45]=="[WinError 10056] Richiesta di connessione ino":
        pass;
    else:
        pass;


        
class comunicazione_daari(threading.Thread):
    def __init__(self, threadID, name):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
    def run(self):
        messaggio=""
        while 1:
            messaggio=self.risp_ari(messaggio)
            time.sleep(0.2)
    
    def risp_ari(self,messaggio):   #acquisizione risposte da arianna, da mettere nella coda giusta
        #s.settimeout(2)
        try:
            raw_bytestream = s.recv(BUFFER_SIZE)      #metto nella coda di ricezione le info di arduino
            messaggio+=str(raw_bytestream)
        except Exception as msg:
            if str(msg).find("[WinError 10035]")<0:
                messaggio=''
                print("errore socket")
                return messaggio
        mex=arianna_utility.gestiscirisp(messaggio)
        if (time.time()-cfg.tempo_radar>5 and cfg.time_radar!=0):
            if semaelabora._value==0:
                semaelabora.release()
                arianna_utility.prt("semaforo rilasciato",3,my_gui)
            cfg.time_radar=2
            arianna_utility.prt("timeout radar",3,my_gui)
        for m in mex[0]:
            #arianna_utility.prt(str(m),3,my_gui)
            if m[0:3]=='ost' and m[0:4]!='osta':
                cfg.dist_libera=int(m.split(";")[1])
            if m[0:3]=='mis':
                arianna_utility.prt(str(m),3,my_gui)
                #print(m)
            elif m[0:4]=='osta':
                arianna_utility.prt("rilevato ostacolo",3,my_gui)
            elif m[0:3]=='pos':
                newpos=arianna_utility.deco_pos(str(m))
                try:
                    if newpos[8] in cfg.richieste_pos and newpos[8]!='':
                        if arianna_utility.controlla_new_pos(cfg.posatt, newpos):
                            cfg.posatt=newpos
                            cfg.posatt[2]=str(float(cfg.posatt[2])*cfg.invx)
                        arianna_utility.prt('newpos'+str(newpos),2,my_gui)
                        if str(newpos[5])=='0':
                            arianna_utility.prt("arianna ferma",2,my_gui)
                            cfg.stato[0]=0  #sblocco successivi movimenti
                            cfg.richieste_pos=[]
                except:
                    pass;
                cfg.messaggiesppos.put(m)
                #print('m1',m)
                arianna_utility.prt(m, 2, my_gui)
            elif m[0:4]=='echo':

                cfg.messaggiesprx.put(m)
            elif m[0:2]=='ir':
                
                cfg.messaggiesprx.put(m)
                print('m2',m)
            
            elif m[0:2]=='r:':
                if m[0:4]=='r: 0':
                    pass;
                cfg.messaggiesprx.put(m)

            else:
                pass;
                #cfg.messaggiesprx.put(m)
        messaggio=""
        for m in mex[1]:
            messaggio+m
            return messaggio




class comunicazione_perari (threading.Thread):
    def __init__(self, threadID, name):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
    
    def run(self):
        messaggio=""
        while 1:
            self.com_ari_mov()
            time.sleep(0.1)
       
    def com_ari_mov(self):   #invio comandi per arianna coda movimento
        '''devo gestire l'attesa del fermo la gestisco mettendo la variabile  moto=1 dopo un R e chiamando
        un r con una chiave precisa a ripetizione solo quando mi torna un r:0 con chiave giusto rimetto a 0
        '''
        statosmf.acquire(blocking=False)
            #M= arduino MEGA   E=ESP 
        if cfg.messaggiesptx.empty()==False:
            while cfg.stato[0]>0:
                idunivoco=arianna_utility.idmsg()
                cfg.richieste_pos.append(idunivoco)
                mystring='1p'+idunivoco
                mystring="!"+mystring+"?"
                t=bytes(mystring, 'utf-8')
                try:
                    s.send(t)
                except:
                    print("errore coda mov")
                
                time.sleep(1.5)

                
            time.sleep(0.1)

            mystring=cfg.messaggiesptx.get()
            if mystring.find('xx')>=0:
                pass;
            if mystring.find("sleep")>=0:  #addormento procedura per n secondi
                mio_sl=int(mystring.split(";")[1])
                mio_sl=time.time()+mio_sl
                cfg.timer_sleep=mio_sl
                statosmf.release()
                return

            if mystring.find("1q")>=0:
                cfg.time_radar=1
                cfg.tempo_radar=time.time()
                cfg.ultimo_angolo_libero=[]
                cfg.ultima_richiesta_libero=cfg.posatt
                mystring=arianna_utility.cmdradar(mystring)    #trovo verso
            if mystring.find("3R6")>=0:   # se ruoto annullo blocchi per ostacoli
                cfg.dist_libera=500     
            if mystring.find("3R")>=0:  #se e movimento svuoto le richieste di attese
                pass;
            if mystring.find("1r")>=0:  #se e movimento svuoto le richieste di attese
                cfg.richieste_pos=[]
                cfg.stato[0]=1
                cfg.messaggiesptx.put('xx')
            if mystring.find("3A")>=0:   #attesa
                angnew=arianna_utility.minimoangolo(float(cfg.posatt[3]), float(mystring[2:]))
                mystring="3A"+str(angnew)
            mystring="!"+mystring+"?"
            #arianna_utility.prt(mystring,1,my_gui)
            t=bytes(mystring, 'utf-8')
            try:
                if mystring.find('xx')<0:
                    s.send(t)
            except:
                print("errore coda mov")
                pass
            arianna_utility.prt(str(mystring),1,my_gui)
            mystring=""
        statosmf.release()
    
    
class elabora (threading.Thread):
    def __init__(self, threadID, name):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name      
    
    def run(self):
        while 1:
            if  cfg.messaggiesprx.empty()==False:
                msgxx=cfg.messaggiesprx.get()
                #print("msg generico",msgxx)
                if  msgxx[0:4]=='echo' :
                    arianna_utility.prt(str(msgxx),2,my_gui)
                    #arianna_utility.crea_mappa(msgxx,cfg.mappa,"assoluta",cfg.versoradar,cfg.posatt)
                    arianna_utility.trovadistanza(msgxx)   
                
                if  msgxx[0:4]=='echf' :
                    
                    if semaelabora._value==0:
                        semaelabora.release()
                    arianna_utility.inizializza_mappa()
                    cfg.time_radar=2
                    arianna_utility.prt("fine radar",2,my_gui)
            
            if  cfg.messaggiesppos.empty()==False:
                posxyt=cfg.messaggiesppos.get()
                newpos=arianna_utility.deco_pos(str(posxyt))
                if arianna_utility.controlla_new_pos(cfg.posatt, newpos):
                    cfg.posatt=newpos
                    cfg.posatt[2]=str(float(cfg.posatt[2])*cfg.invx)

            self.elaborarich()
    
        
    def elaborarich(self):
        
        statosmf.acquire(blocking=False)
        semaelabora.acquire(blocking=False)
        while cfg.messaggirx.empty()==False:
            lavoro=cfg.messaggirx.get()
            cfg.messaggiesptx.put(lavoro[1])
       
        if cfg.percorsi.empty()==False and cfg.messaggiesptx.empty()==True and cfg.stato[0]==0 and  cfg.time_radar!=1:
            
           
            destinazione=cfg.percorsi.get()
            if destinazione[1][2]=='3R3' or destinazione[1][2]=='3R1':
                cfg.messaggirx.put((time.time(),destinazione[1][3]))
                time.sleep(0.1)
                cfg.messaggirx.put((time.time(),destinazione[1][4]))
                time.sleep(0.1)
                cfg.messaggirx.put((time.time(),destinazione[1][0]))
                time.sleep(0.1)
                cfg.messaggirx.put((time.time(),destinazione[1][2]))
                time.sleep(0.1)
                cfg.messaggirx.put((time.time(),'1r'))
                time.sleep(0.1)
                statosmf.release()  
                semaelabora.release() 
                return
            
                
            

            if cfg.dist_libera<=50 and destinazione[1][2]!='3R6' and len(cfg.ultimo_angolo_libero)==0:
                print("davanti non posso andare cosa faccio?")
                cfg.messaggirx.put((time.time(),"1q10+160+10"))
                cfg.tempo_radar=time.time()
                cfg.percorsi.put(destinazione)
                statosmf.release()   
                #semaelabora.release() #non rilascio il semaforo, sarà la lettura echo a farlo
                return
            if  cfg.dist_libera<=50 and destinazione[1][2]!='3R6' and len(cfg.ultimo_angolo_libero)!=0:
                nuovoangolo=90-int(cfg.ultimo_angolo_libero[0])
                print("calcolo via di fuga ",nuovoangolo)  
                cfg.percorsi.put(destinazione)
                cfg.messaggirx.put((time.time(),'3A'+str(math.degrees(float(cfg.posatt[3]))+nuovoangolo)))
                time.sleep(0.1)
                cfg.messaggirx.put((time.time(),'3R6'))
                time.sleep(0.1)
                cfg.messaggirx.put((time.time(),'1r'))
                time.sleep(0.1)
                cfg.messaggirx.put((time.time(),'3D'+str(int(cfg.ultimo_angolo_libero[1])*10-500)))
                time.sleep(0.1)
                cfg.messaggirx.put((time.time(),'3R4'))
                time.sleep(0.1)
                cfg.messaggirx.put((time.time(),'1r'))
                time.sleep(0.1)
                cfg.ultimo_angolo_libero=[]
                statosmf.release()
                semaelabora.release()
                return

            a,dist,ang=arianna_utility.calcola_movimento_inv(destinazione[1][0], destinazione[1][1], destinazione[1][2])
            if (destinazione[1][2]=='3R6'):
                #per r6 faccio prima movimnto a 0 d e angolo e poi cambio in r4
                destinazione[1][2]='3R4'
                a[2]='3R4'
                b=['3A'+str(ang),'3R6','1r']
                for cmd in b:
                    cfg.messaggirx.put((time.time(),cmd))
                    time.sleep(0.1)
                
            if (dist<100 ):  #se la distanza è minore di 10 cm mi considero arrivato , mettere parametro?
                cfg.stato[0]=0

            else:
                cfg.percorsi.put(destinazione)
                for cmd in a:
                    cfg.messaggirx.put((time.time(),cmd))
                    time.sleep(0.1)
                    
                    
            #cfg.percorsi.put(destinazione)
        statosmf.release()
        semaelabora.release()


class mappa (threading.Thread):
    def __init__(self, threadID, name):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name      
    
    def run(self):
        while 1:
            if cfg.time_radar==2 :
                cfg.time_radar=1
                
                x=arianna_db.leggosql([], 'mappe_elaborare')
                if len(x)>0:
                    for i in x:
                        arianna_db.leggosql(['',cfg.id_radar],'deletec')
                        punti=arianna_db.leggosql(i,'mappa')
                        for i in punti:
                            arianna_utility.crea_mappa('',cfg.mappa,"assoluta","dx",[i[0],i[1],math.radians(i[4])],1,i[2],i[3])
                cfg.time_radar=0    
            else:
                time.sleep(0.1)
            
# **********param default****
cfg.stato[0]=1
cfg.messaggirx.put((time.time(),'>p9600'))
time.sleep(0.2)



if cfg.par_ini_car==1:

    cfg.messaggirx.put((time.time(),'3F'+cfg.ED))
    time.sleep(0.2)
    cfg.messaggirx.put((time.time(),'3F1'+cfg.ED_BASE))
    time.sleep(0.2)
    cfg.messaggirx.put((time.time(),'3F2'+cfg.BASELINE))
    time.sleep(0.2)
    temp=round(cfg.DIAM_RUOTA*3.14/(4*cfg.encoderppr), 4)
    print ("comandi vari inseriti")
    cfg.messaggirx.put((time.time(),'3F3'+str(temp)))
    time.sleep(0.5)
    cfg.messaggirx.put((time.time(),'3K0'+str(cfg.K0)))
    time.sleep(0.5)
    cfg.messaggirx.put((time.time(),'3E3'))
time.sleep(0.5)
cfg.messaggirx.put((time.time(),'3F4'+cfg.divisore_lidar))
time.sleep(0.2)
cfg.id_radar=arianna_utility.idmap()

#nuova mappa avvio 
root = Tk()
my_gui = arianna_gui.MyFirstGUI(root)

thread1 = arianna_web.serverweb(1, "Thread-w1",8081)
thread2 = comunicazione_daari(2, "Thread-xari")
thread3 = comunicazione_perari(2, "Thread-com1")
thread4 = elabora(3, "Thread-ela1")
thread5 = mappa(4, "Thread-map1")
thread6 = arianna_webmon.serverwebmon(5, "web_mon",8888)


# Start new Threads
thread1.start()
time.sleep(0.1)
thread2.start()
time.sleep(0.1)
thread3.start()
time.sleep(0.1)
thread4.start()
time.sleep(0.1)
thread5.start()
time.sleep(0.1)
thread6.start()

#webbrowser.open('http://127.0.0.1:8081/ui2',new=1)

root.mainloop()
#apro browser


print ("Exiting Main Thread")