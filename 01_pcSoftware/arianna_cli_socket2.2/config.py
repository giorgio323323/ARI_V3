'''
Created on 02/ott/2017

@author: a.airaghi
11gen2019
    sistemazine e ordinamento parametri

'''
import queue
import os
import configparser

nome_ari=''
config = configparser.ConfigParser()
config.read('config.ini')
nome_ari=config.get('DEFAULT', 'nome_arianna')

#parametri di configurazione fisica arianna


TCP_PORT=config.getint(nome_ari, 'TCP_PORT')
UDP_PORT=config.getint(nome_ari, 'UDP_PORT')
ostacolo_distanza=50
comandi_ini=[]
try:
    if  config.get(nome_ari, 'ED')!='' :
        comandi_ini.append('3F'+config.get(nome_ari, 'ED')) 
except:
    pass;
try:
    if  config.get(nome_ari, 'ED_base')!='' :
        comandi_ini.append('3F1'+config.get(nome_ari, 'ed_base')) 
except:
    pass;
try:
    if  config.get(nome_ari, 'BASELINE')!='' :
        comandi_ini.append('3F2'+config.get(nome_ari, 'BASELINE')) 
except:
    pass;
try:
    if  config.get(nome_ari, 'diam_ruota')!='' :
        temp=round(config.getint(nome_ari, 'diam_ruota')*3.14/(4*config.getint(nome_ari, 'encoderppr ')), 4)
        comandi_ini.append('3F3'+str(temp)) 
except:
    pass;
try:
    if  config.get(nome_ari, 'K0')!='' :
        comandi_ini.append('3K0'+config.get(nome_ari, 'K0')) 
except:
    pass;
if (len(comandi_ini)>0):
    comandi_ini.append('3E3') 
try:
    if  config.get(nome_ari, 'divisore_lidar')!='' :
        comandi_ini.append('3F4'+config.get(nome_ari, 'divisore_lidar')) 
except:
    pass;
try:
    if  config.get(nome_ari, 'ostacolo_distanza')!='' :
        ostacolo_distanza=config.get(nome_ari, 'ostacolo_distanza')
        comandi_ini.append('3O1'+config.get(nome_ari, 'ostacolo_distanza')) 
except:
    ostacolo_distanza=50



invx=config.getint(nome_ari, 'invx')  #mettere meno in caso di inversione destra e sinistra
angolo_radar=config.getfloat(nome_ari, 'angolo_radar')
errore_servo=config.getint(nome_ari, 'errore_servo')  #aggiusto angolo del servo in quanto non e' mai in asse perfetto con arianna
versoradar=config.get(nome_ari, 'versoradar') 
max_dst=1000

#code di comunicazione
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
