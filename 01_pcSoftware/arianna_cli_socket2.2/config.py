'''
Created on 02/ott/2017

@author: a.airaghi
11gen2019
    sistemazine e ordinamento parametri

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
ostacolo_distanza='49'
invx=1   #mettere meno in caso di inversione destra e sinistra
angolo_radar=2.5
max_dst=1000
versoradar='sinistra'
errore_servo=-10 #aggiusto angolo del servo in quanto non e' mai in asse perfetto con arianna


messaggirx=queue.PriorityQueue()  #comandi che arrivano da utente
messaggicli=queue.Queue(0)        #risposte da mandare a interfaccia
percorsi=queue.PriorityQueue()    #inserisco i punti da raggiungere [4]=x,y,modo,posradar 

#code verso esp
messaggiesptx=queue.Queue(0)   #coda messaggi da inviare a esp movimento
messaggiesptx_altro=queue.Queue(0) #coda messaggi da inviare a esp altro
messaggiesprx=queue.Queue(0)    #coda messaggi ricevuti da esp
messaggiesppos=queue.Queue(0) 


time_radar=0
id_radar='PDHP'
ostacoli=[]                 #punti ostacoli

#mappa numero di celle visitate,numero ok probabili, numero di ok assoluti
dim_cella=10.0 #per mappa
#timestamp x y teta
posatt=[0,0.0,0.0,1.5708,0,0,0,0,0]
#stato 0 acq pos, 1 fermo , 2 in mov,3 percorso, 
pos_teorica=[0,0]
dist_libera=999
ultimo_angolo_libero=[]
ultima_richiesta_libero=[0,0.0,0.0,0,0,0,0,0,0]


#semafori vari
stato=[0] 
richieste_fermo=[]
richieste_pos=[]
tempo_radar=0 #ogni tanto non sblocco
sem_registrazione=0
tempo_registrazione=0
tempo_datiregistrazione=0

#gestione percorsi
tipo_moto=''


#registratore
dati_registrazione=[]
num_registrazioni=4
registrazione_ultimo=0

#gestione mappa
passo_attraversamento=10
mappa={}
mappa_relativa={}
mappa_rel_ass={}
radar_ini=0
radar_ini_rel=0
dst_prec=[]
ang_prec=[]
dst_prec_rel=[]
ang_prec_rel=[]
dst_prec_rel_ass=[]
ang_prec_rel_ass=[]

#impostazione path
localpath=os.path.dirname(os.path.abspath(__file__))