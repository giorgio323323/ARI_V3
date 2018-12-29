'''
Created on 02/ott/2017

@author: a.airaghi
'''
import queue
import os

#parametri di configurazione fisica arianna
par_ini_car=1
ED="1"
ED_BASE="1"
BASELINE="200"
DIAM_RUOTA=48
encoderppr=20
K0='8'
divisore_lidar='10'



messaggirx=queue.PriorityQueue()  #comandi che arrivano da utente
messaggicli=queue.Queue(0)        #risposte da mandare a interfaccia

#code verso esp
messaggiesptx=queue.Queue(0)   #coda messaggi da inviare a esp movimento
messaggiesptx_altro=queue.Queue(0) #coda messaggi da inviare a esp altro
messaggiesprx=queue.Queue(0)    #coda messaggi ricevuti da esp
messaggiesppos=queue.Queue(0) 
percorsi=queue.PriorityQueue()    #inserisco i punti da raggiungere [4]=x,y,modo,posradar 

time_radar=0

id_radar='PDHP'
passo_attraversamento=10


ostacoli=[]                 #punti ostacoli

bloccarisp=0
#####10,45,90,135,170
tilt_Angle_home=-1


timer_sleep=0
#mappa numero di celle visitate,numero ok probabili, numero di ok assoluti
dim_cella=10.0 #per mappa
errore_lettura=10

invx=1   #mettere meno in caso di inversione destra e sinistra
#timestamp x y teta
posatt=[0,0.0,0.0,1.5708,0,0,0,0]
#stato 0 acq pos, 1 fermo , 2 in mov,3 percorso, 
pos_teorica=[0,0]

stato=[0] 
richieste_fermo=[]
richieste_pos=[]
dist_libera=999
ultimo_angolo_libero=[]
ultima_richiesta_libero=[0,0.0,0.0,0,0,0,0,0]
tiporadar=1    #1 tfmini ,2 radar, 3 VL53L0X
tempo_radar=0 #ogni tanto non sblocco


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
    max_dst=1000
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