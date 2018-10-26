'''
Created on 10/apr/2018

@author: a.airaghi
'''
import cherrypy
import config as cfg
import arianna_db
import arianna_utility
import math
import time
import threading
import os
#===============================================================================
# utility web
#===============================================================================

class Benvenuto:
    def index(self):
        return "Benvenuto!"
    index.exposed = True

    def index2(self):
        return open(cfg.pgmpath+'scheda/index.html')
    index2.exposed = True
    
    def ui2(self):
        return open(cfg.pgmpath+'scheda/arianna5.0.html')
    ui2.exposed = True
    
    def mappa(self):
        arianna_db.da_db_mappa()
        #linee=arianna_db.leggosql([cfg.id_radar],"linee")
        #arianna_utility.mappa_seg(cfg.mappa,"assoluta",linee)
        arianna_utility.mappa_seg(cfg.mappa,"assoluta",'')
        return open(cfg.pgmpath+'scheda/mappasegok.html')
    mappa.exposed = True
    
    def risposte(self):
        testo=''
        posx=str(int(float(cfg.posatt[1])/10.0))
        posy=str(int(float(cfg.posatt[2])/10.0))
        angolo=str(math.degrees(float(cfg.posatt[3])))
        testo="pos:x "+posx+" y "+posy+" a "+angolo+"\n"
        if cfg.time_radar==0:
            t="non in  scansione"
        else:
            t="in scansione"
        t2=time.time()
        
        testo=testo+"ULT SCS "+cfg.id_radar+" | "+t+"\n"
        testo=testo+str(t2)
        return testo
    risposte.exposed=True
    
    def comandi_ui(self,nome,valore,cod=''):
        testo='ricevuto '+nome
        print (cod)
        if cod.find("SRV")>=0:
            cmd=cod.split("|")
            if cmd[1]=="id_radar":
                cfg.id_radar=cmd[2]
                print("id radar",cfg.id_radar)
            return cmd
            
        if str(cod)!='' and str(cod) != "None":
            print("cod",cod)
            if cod[0:1]=='9':
                arrivo=[int(cod[2:].split("|")[0]),int(cod[2:].split("|")[1])]
                print("arrivo",arrivo)
                cfg.percorso.append(arrivo)
                #comando da gestire in python
            else:
                cmd=cod.split("|")
                for i in cmd:
                    time.sleep(0.2)
                    print("i",i)
                    cfg.messaggirx.put((time.time(),i))
                
        #return testo
    comandi_ui.exposed=True

    def bottonemov(self,dato):
        print(dato)
        cfg.messaggirx.put((time.time(),dato))  #metto nella coda messaggi ricevuti il comando da eseguire
    bottonemov.exposed = True

class serverweb (threading.Thread):
    def __init__(self, threadID, name):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        
    def run(self):
        cherrypy.server.socket_host = '127.0.0.1'
        cherrypy.server.socket_port=8081
        cherrypy.server.thread_pool_max=1
        cherrypy.server.thread_pool=1
        conf = {
        '/': {
            'tools.sessions.on': True,
            'tools.staticdir.root': os.path.abspath(os.getcwd())
        },
        '/scheda': {
            'tools.staticdir.on': True,
            'tools.staticdir.dir': 'scheda'
        }
                ,
        '/images': {
            'tools.staticdir.on': True,
            'tools.staticdir.dir': 'scheda/images'
        }
    }
        
        cherrypy.config.update({
                        'log.access_file': "access1.log",
                        'log.screen': False,
                       })
        cherrypy.quickstart(Benvenuto(),'/', config=conf)

