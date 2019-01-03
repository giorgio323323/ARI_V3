'''
Created on 10/apr/2018

@author: a.airaghi
'''
import sqlite3 as lite
import config as cfg
import math
import arianna_utility


def scrivo_db_mappa(ang,distanza,tipo=''):
    pox,poy=arianna_utility.pos_fine([float(cfg.posatt[2]),float(cfg.posatt[3]),float(cfg.posatt[4])], float(distanza), float(ang),10)
    con = lite.connect(cfg.localpath+'\\arianna.db3', isolation_level=None)  
    cur = con.cursor()

    cur.execute("""insert into radar values(current_timestamp,"""
                +str(cfg.posatt[2])+""","""
                +str(cfg.posatt[3])+""","""
                +str(math.degrees(float(cfg.posatt[4])))+""","""
                +str(distanza)+""","""
                +str(ang)+""","""
                +str(pox)+""","""
                +str(poy)+""",'"""
                +str(cfg.id_radar)+"""','"""
                +str(tipo)+"""')""")



def scrivo_db_celle(valore):
    con = lite.connect('arianna.db3', isolation_level=None)  
    cur = con.cursor()
    strsql="insert or replace into celle values "+valore

    cur.execute(strsql)

def da_db_mappa():
    rec=leggosql([cfg.id_radar], 'celle')
    cfg.mappa={}
    for i in rec:
        cfg.mappa[str(i[2])+'|'+str(i[3])]=[1,i[0],0]
        
        
def leggosql(parametri,tipo):
    con = lite.connect('arianna.db3', isolation_level=None)  
    cur = con.cursor()
    if tipo=='mappe_elaborare':
        cur.execute("""select distinct id from radar where id ='"""+cfg.id_radar+"""'""")
        rec=cur.fetchall()
        id=[]
        for i in rec:
            id.append([i[0]])
        return id
    if tipo=='radarl':
        cur.execute("""select ostx,osty  from radar where id='"""+parametri[0]+"""' and tipo in ('l','L') order by angolo_rel""")
        rec=cur.fetchall()
        punti=[]
        for i in rec:
            punti.append([i[0],i[1]])
        return punti
    if tipo=='radart':
        cur.execute("""select ostx,osty from radar where id='"""+parametri[0]+"""'  order by angolo_rel""")
        rec=cur.fetchall()
        punti=[]
        for i in rec:
            punti.append([i[0],i[1]])
        return punti
    if tipo=='celle':
        cur.execute("""select prob,tipo,case when tipo=0 then corx_app else corx end,case when tipo=0 then cory_app else cory end from celle where id='"""+parametri[0]+"""'  """)
        rec=cur.fetchall()
        punti=[]
        for i in rec:
            punti.append([i[0],i[1],i[2],i[3]])
        return punti
    if tipo=='mappa':
        cur.execute("""select posx_ari,posy_ari,dist,angolo_rel,angle_ari from radar where id='"""+parametri[0]+"""' and tipo in ('l','L')  order by angolo_rel""")
        rec=cur.fetchall()
        punti=[]
        for i in rec:
            punti.append([i[0],i[1],i[2],i[3],i[4]])
        return punti


    #leggosql([segmenti],"scriviseg")
    if tipo=="deletec":
        cur.execute("""delete from radar where id='"""+parametri[1]+"""' and tipo not in ('l','L')""")
    if tipo=='scriviseg':
        cur.execute("""delete from cluster where id='"""+parametri[1]+"""' """)
        for i in parametri[0]:
            estremi=str(i[0][0])+"|"+str(i[0][1])+"|"+str(i[-1][0])+"|"+str(i[-1][1])
            cur.execute("""INSERT INTO cluster
                  ( id, m, q, estremi)
                  VALUES('"""+parametri[1]+"""', """+'0'+""", """+'0'+""", '"""+estremi+"""')""")
  
