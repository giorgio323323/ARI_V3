'''
Created on 29/set/2017

@author: a.airaghi
'''
import socket
import time
import sqlite3 as lite
import math
import config as cfg
import random
import arianna_utility

def calcola_movimento(angolo,distanza,xpre,ypre):
    multx=1
    multy=1
    if angolo%90==0:
        angolo=angolo+0.1
    b=distanza*math.sin(math.radians(angolo))*multy
    by=float(ypre)+float(b)
    c=distanza*math.sin(math.radians(90-angolo))*multx
    cx=float(xpre)+float(c)
    print("target x, y",cx,by)
    return cx,by

def distanza_punti(a,b):
    return(pow(pow(b[0]-a[0],2)+pow(b[1]-a[1],2),0.5))

def calcola_ostacolo(ox,oy,dx,dy):
        a=arianna_utility.distanza_punto_segmento([ox,oy], [dx,dy], [1000,0])
        if a<500:
            return True
        return False
        
        
        
def letturadb(id='PAAA'):
    con = lite.connect('arianna.db3', isolation_level=None)  
    cur = con.cursor()
    cur.execute("""select angolo_rel,dist*10 from radar where id='"""+id+"""'  order by angolo_rel""")
    rec=cur.fetchall()
    letture=[]
    for i in rec:
        letture.append("echo-"+str(int(i[0]))+"-"+str(int(i[1]))+"-"+str(int(i[1]))+"-"+str(int(i[1]))+"-"+str(int(i[1]))+"-"+str(int(i[1]))+"-"+str(int(i[1])))
        #punti.append([i[0],i[1]])
    return letture


 



def Main2():
    host = "127.0.0.1"
    port = 8181
    stato=0
     
    mySocket = socket.socket()
    mySocket.bind((host,port))
     
    mySocket.listen(1)
    conn, addr = mySocket.accept()
    print ("Connection from: " + str(addr))
    cont=0
    contrit=0
    test=0
    data=''
    time.sleep(2)
    dati2="!pos: 0;0;0;0;0;0;0;0;0 ?"
    conn.send(dati2.encode())
    angolo=0
    distanza=0
    xpre=0
    ypre=0
    x=0
    y=0
    iniziocmd=0
    comando=''
    while True:
            
                
            #conn.send(dati2.encode())
            data=''
            dati2=''
            cont+=1
            conn.settimeout(0)
            if comando=='':
                try:
                    data = conn.recv(1)
                    data=data.decode("utf-8") 
                    if data=="!":
                        iniziocmd=1
                        t=''
                        continue
                    if iniziocmd==1 and data!='?':
                        t=t+data
                    if data=="?" and t!='':
                        
                        iniziocmd=0
                        comando=t
                        #print("comando",comando)
                        t=''
                    if not data:
                        break
    
                except:
                    pass;
            if random.random() >0.95:
                dati2="!pos: "+str(time.time())+";"+str(x)+";"+str(y)+";"+str(angolo)+";0;0;0;0;"+str(comando)[2:4]+"?"
                conn.send(dati2.encode())
                
            if comando!='':
                if comando.find("1q")>=0:
                    dati2='!echo-10-999-999-999-999-999-999-999-999-999-999?'
                    conn.send(dati2.encode())
                    dati2=''
                    time.sleep(0.3)
                    dati2='!echo-30-40-40-40-40-40-40-40-40-40-40?'
                    conn.send(dati2.encode())
                    dati2=''
                    time.sleep(0.3)
                    dati2='!echo-90-498-498-498-498-498-498-498-498-498-498?'
                    conn.send(dati2.encode())
                    dati2=''
                    time.sleep(0.3)

                
                if comando.find("1r")>=0:
                    dati2="!r: 0;"+str(comando)[2:4]+"?"
                    #print("dati2",dati2)
                    conn.send(dati2.encode())
                if comando.find("1p")>=0:
                    dati2="!pos: 0;"+str(x)+";"+str(y)+";"+str(angolo)+";0;0;0;0;"+str(comando)[2:4]+"?"
                    #print("dati2",dati2)
                    conn.send(dati2.encode())
                if comando.find('3A')>=0:
                    angolo=str(comando)[2:]
                if comando.find('3D')>=0:
                    distanza=str(comando)[2:]
                if comando.find('3R4')>=0:
                    print("simu ang",angolo)
                    
                    x,y=calcola_movimento(float(angolo),float(distanza),xpre,ypre)
                    ostacolo=calcola_ostacolo(x,y,xpre,ypre)
                    if ostacolo==True:
                        dati2='!mis;50?'
                        conn.send(dati2.encode())
                        dati2=''
                        x=xpre
                        y=ypre
                    else:
                        dati2='!mis;51?'
                        conn.send(dati2.encode())
                        dati2=''
                        xpre=x
                        ypre=y

                    distanza=''
                    
                    
                
                
                comando=''
            
            time.sleep(0.2)

             
    conn.close()

     
if __name__ == '__main__':
    Main2()
    