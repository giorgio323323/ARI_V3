'''
Created on 02/ott/2017

@author: a.airaghi
'''
import queue
import os

#parametri di configurazione fisica arianna
ED="1"
ED_BASE="1"
BASELINE="153"
DIAM_RUOTA=48
encoderppr=20



messaggirx=queue.PriorityQueue()  #comandi che arrivano da utente
messaggicli=queue.Queue(0)        #risposte da mandare a interfaccia

#code verso esp
messaggiesptx=queue.Queue(0)   #coda messaggi da inviare a esp movimento
messaggiesptx_altro=queue.Queue(0) #coda messaggi da inviare a esp altro
messaggiesprx=queue.Queue(0)    #coda messaggi ricevuti da esp
messaggiesppos=queue.Queue(0) 

time_radar=0

id_radar='PDHP'
passo_attraversamento=10

#lista percorso
percorso = list()
ostacoli=[]                 #punti ostacoli

bloccarisp=0
#####10,45,90,135,170
tilt_Angle_home=-1


timer_sleep=0
#mappa numero di celle visitate,numero ok probabili, numero di ok assoluti
dim_cella=10.0 #per mappa
errore_lettura=10


#timestamp x y teta
posatt=[0,0.0,0.0,1.5708,0,0,0,0]
#stato 0 fermo, 1 prenotato mov , 2 in mov
stato=[0]
richieste_fermo=[]
dist_libera=999

tiporadar=1    #1 tfmini ,2 radar, 3 VL53L0X



radar_ini=0

radar_ini_rel=0
                
pgmpath=''

mappa_tst={}
mappa={}
mappa_relativa={}
mappa_rel_ass={}
mappafile="mappa.txt"
arianna_mappa="ariannapos.txt"
if tiporadar==2:
    angolo_radar=20.0
    max_dst=250
    divisore_dist=1  #valido per tfmini
if tiporadar==1:
    angolo_radar=2.5
    max_dst=500
    divisore_dist=1  #valido per tfmini
if tiporadar==3:
    angolo_radar=10.0
    max_dst=150
    divisore_dist=1  #valido per tfmini
  
dst_prec=[]
ang_prec=[]

dst_prec_rel=[]
ang_prec_rel=[]
dst_prec_rel_ass=[]
ang_prec_rel_ass=[]
versoradar='sinistra'
localpath=os.path.dirname(os.path.abspath(__file__))