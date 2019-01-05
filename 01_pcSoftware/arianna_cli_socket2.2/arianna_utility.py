# -*- coding: Latin-1 -*-
'''
Created on 21/set/2017

@author: a.airaghi
'''
import config as cfg
import  arianna_db 
import math
import time
import statistics
import string

from sympy import Line,Point,Segment,Circle
import numpy
import operator as op



#*************************utilita varie****************************************************

def prt(testo,dest,gui):
    if dest==0:
        print(testo)
    if dest==1:
        try:
            gui.aggiungi(testo+'\n')
        except:
            pass;
    if dest==2:
        try:
            gui.aggiungi2(testo+'\n')
        except:
            pass;
    if dest==3:
        try:
            gui.aggiungi3(testo+'\n')
        except:
            pass;

#*************************funzioni di movimento********************************************


def svuota_coda(coda):
    if coda=='cfg.messaggiesptx':
        while cfg.messaggiesptx.empty()==False:
            a=cfg.messaggiesptx.get()
    if coda=='cfg.messaggirx':
        while cfg.messaggirx.empty()==False:
            a=cfg.messaggirx.get()
            
def elencocmd(elenco,pausa=0.1):
    for l in elenco:
        print(l)
        cfg.messaggirx.put((time.time(),l))
        time.sleep(pausa)
    
    

#********************funz geometriche e matematiche ****************************************
def rotazione_punto(x,y,ang=85,ox=0,oy=0):
    qx = ox + math.cos(math.radians(ang)) * (x - ox) - math.sin(math.radians(ang)) * (y - oy)
    qy = oy + math.sin(math.radians(ang)) * (x - ox) + math.cos(math.radians(ang)) * (y - oy)
    return [qx, qy]
    
def retta2punti(p1,p2):
    if p2[0] != p1[0]:
        m = (p2[1] - p1[1])/  (p2[0] - p1[0])
        q=p1[1]-(p1[1]-p2[1])/(p1[0]-p2[0])*p1[0]
    else:
        m=999999
        q=99999
    return[m,q]

def calcola_movimento_teo(angolo,distanza):
    pos_x=cfg.pos_teorica[0]
    pos_y=cfg.pos_teorica[1]
    if distanza<=1: #deve valore solo per r6
        distanza=1
        pos_x=cfg.posatt[1]
        pos_y=cfg.posatt[2]
    multx=1
    multy=1
    if angolo%90==0:
        angolo=angolo+0.1
    b=distanza*math.sin(math.radians(angolo))*multy
    by=float(pos_y)+float(b)
    c=distanza*math.sin(math.radians(90-angolo))*multx
    cx=float(pos_x)+float(c)
    if distanza>1:
        cfg.pos_teorica[0]=cx
        cfg.pos_teorica[1]=by
    print("target x, y",cx,by)
    return cx,by
       
def calcola_movimento(angolo,distanza):
    multx=1
    multy=1
    if angolo%90==0:
        angolo=angolo+0.1
    b=distanza*math.sin(math.radians(angolo))*multy
    by=float(cfg.posatt[3])+float(b)
    c=distanza*math.sin(math.radians(90-angolo))*multx
    cx=float(cfg.posatt[2])+float(c)
    print("target x, y",cx,by)
    return cx,by

def calcola_movimento_inv(x,y,r):
    dist=distanza_punti([float(cfg.posatt[2]),float(cfg.posatt[3])],[float(x),float(y)])
    ang=angolo_base([float(cfg.posatt[2]),float(cfg.posatt[3])],[float(x),float(y)])
    #print("dist,ang",dist,ang)
    return ['3A'+str(round(ang, 2)),'3D'+str(round(dist, 0)),r,'1r'],dist,ang


def punto_medio_seg(p1,p2):
    xm=(p1[0]+p2[0])/2
    ym=(p1[1]+p2[1])/2
    return xm , ym


def cerchio_esterno(p1,p2):
    cx,cy=punto_medio_seg(p1,p2)
    cc=[cx,cy]
    r=distanza_punti(cc, p1)
    return cc, r

def inters_cerchio_retta(m,q,p1,p2):
    c,r=cerchio_esterno(p1,p2)
    c1 = Circle(Point(c[0], c[1]), r)
    l1=Line(Point(10,m*10+q),Point(100,m*100+q))
    p=l1.intersection(c1)
    risu=[]
    for x in p:
        risu.append([int(x.x),int(x.y)])
    return risu





def lista_to_array(l): 
    #per usi con numpy 
    x=[]
    y=[]
    for i in l:
        x.append(i[0])
        y.append(i[1])
    xx=numpy.array(x)
    yy=numpy.array(y)
    return xx, yy  


def retta_app(lista):
    #retta con regressione
    x=[]
    y=[]
    for i in lista:
        x.append(i[0])
        y.append(i[1])
    xx=numpy.array(x)
    yy=numpy.array(y)
    A= numpy.vstack([xx, numpy.ones(len(xx))]).T
    m, c = numpy.linalg.lstsq(A, yy)[0]
    return m, c


def angolo_retta(m1,m2):
    #angolo fra due rette 
    if (1+m1*m2)!=0:
        a = (m1-m2) /(1+m1*m2)
    else:
        a=0
    x=numpy.arctan(a)
    return(numpy.degrees(x))

def pos_fine(inizio,distanza,angolo,divisore=1):
    #trov coordinate dato inizio angolo e distanza
    angolo=math.degrees(float(inizio[2]))-90+angolo
    if angolo!=90 and angolo!=-90 :
        x=distanza*math.cos(math.radians(angolo))
        #y=x*math.tan(math.radians(angolo))
        y=distanza*math.sin(math.radians(angolo))
        x=x+(inizio[0]/divisore)
        y=y+(inizio[1]/divisore)
    else:
        x=inizio[0]
        y=inizio[1]+distanza
    return x,y

def trova_cella_pos(pix,piy):
    #trova quadrato di dimensione definita nel piano cartesiano
    xi=int(pix/cfg.dim_cella)
    yi=int(piy/cfg.dim_cella)
    return [xi,yi]

def distanza_punti(a,b):
    return(pow(pow(b[0]-a[0],2)+pow(b[1]-a[1],2),0.5))

def distanza_segmenti(s1,s2):
    nvicini=0
    a=s1[0]
    b=s1[1]
    c=s2[0]
    d=s2[1]
    dist={}
    dist['ab']=[distanza_punti(a,b),1]
    dist['ac']=[distanza_punti(a,c),3]
    dist['ad']=[distanza_punti(a,d),3]
    dist['bc']=[distanza_punti(b,c),3]
    dist['bd']=[distanza_punti(b,d),3]
    dist['cd']=[distanza_punti(c,d),2]
    for ele in dist.values():
        if ele[0]<10 and ele[1]>2:
            nvicini+=1 
    maxdist=max(dist.items(), key=op.itemgetter(1))
    if maxdist[0]=='ab':
        return[nvicini,s1[0],s1[1]]
    if maxdist[0]=='ac':
        return[nvicini,s1[0],s2[0]]
    if maxdist[0]=='ad':
        return[nvicini,s1[0],s2[1]]
    if maxdist[0]=='bc':
        return[nvicini,s1[1],s2[0]]
    if maxdist[0]=='bd':
        return[nvicini,s1[1],s2[1]]
    if maxdist[0]=='cd':
        return[nvicini,s2[0],s2[1]]
    
        

def catetobase(ipot,angrad):
    x=ipot*math.cos(angrad)
    return x

def catetoalt(ipot,angrad):
    x=ipot*math.sin(angrad)
    return x


def base(a,b):
    return abs(a[0]-b[0])

def altezza(a,b):
    return abs(a[1]-b[1])

def prossimita(a,b,d):
    if (distanza_punti(a,b)<=d):
        return 1
    else:
        return 0

def polar(x,y):
    return math.hypot(x,y),math.degrees(math.atan2(y,x))


def distanza_punto_retta2(origine,destinazione,punto):
    p1=Point(origine[0],origine[1])
    p2=Point(destinazione[0],destinazione[1])
    p3=Point(punto[0],punto[1])
    l1=Line(p1,p2)
    return(l1.distance(p3)) 

def distanza_punto_retta1(m,q,p):
    d=0
    d=(abs(p[1]-(m*p[0]+q)))/pow((1+m*m),0.5)
    return d

def distanza_punto_segmento(origine,destinazione,punto):
    p1=Point(origine[0],origine[1])
    p2=Point(destinazione[0],destinazione[1])
    p3=Point(punto[0],punto[1])
    s1=Segment(p1,p2)
    return(s1.distance(p3))


def punto_proiettato_retta(origine,destinazione,punto):
    p1=Point(origine[0],origine[1])
    p2=Point(destinazione[0],destinazione[1])
    p3=Point(punto[0],punto[1])
    l1=Line(p1,p2)
    s1=l1.perpendicular_line(p3)
    i= l1.intersection(s1)
    return([float(i[0].x),float(i[0].y)])

def pos_punto(a,b,c):
    # ritorna la posizione del 3 punto rispetto alla retta passante per i primi due
    SegnoProdottoVettore = ((b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]))
    if SegnoProdottoVettore>0:
        return 1
    else:
        return -1


def angolo_base(a,b):
    #ritorna l'angolo per andare dal punto a a l punto b rispetto all'angolo base
    # andrebbe migliorata tenendo conto dell'angolo attuale 
    if (a[0]<=b[0] and a[1]<=b[1]):
        return math.degrees(math.atan2(altezza(a,b),base(a,b)))
    elif (a[0]>=b[0] and a[1]>=b[1]):
        return math.degrees(math.atan2(altezza(a,b),base(a,b)))-180
    elif (a[0]>=b[0] and a[1]<=b[1]):
        return 180-math.degrees(math.atan2(altezza(a,b),base(a,b)))
    elif (a[0]<=b[0] and a[1]>=b[1]):
        return -1*math.degrees(math.atan2(altezza(a,b),base(a,b)))


def intersezione(p1,p2,p3,p4):
    Delta0 = (p4[1] - p3[1]) * (p2[0] - p1[0]) - (p4[0] - p3[0]) * (p2[1] - p1[1])
    if Delta0 == 0:
        pass;
        #segementi paralleli
    else:
        Delta1 = (p4[0] - p3[0]) * (p1[1] - p3[1]) - (p4[1] - p3[1]) * (p1[0] - p3[0])
        Delta2 = (p2[0] - p1[0]) * (p1[1] - p3[1]) - (p2[1] - p1[1]) * (p1[0] - p3[0])
        ka = Delta1 / Delta0
        kb = Delta2 / Delta0
        p0x = p1[0] + ka * (p2[0] - p1[0])
        p0y = p1[1] + ka * (p2[1] - p1[1])
        return [1,[p0x,p0y]]
    return [0,[0,0]]    
        
        
def minimoangolo(ap,ar): 
    gr_ari=(math.degrees(ap)%360)
    if ap<0:
        gr_ari=-1*gr_ari
    gr_new1=ar
    if (ar>=0):
        gr_new2=ar-360
    else:
        gr_new2=360-ar  
    delta=(gr_new1-gr_ari)
    if (math.fabs((gr_new1-gr_ari))<180):
        return (math.degrees(ap)+delta)
    delta=(gr_new2-gr_ari)
    return (math.degrees(ap)+delta)
    
    #devo scegliere se meglio gr_new1 o gr_new2 


# da non usare
def minimoangolo2(ap,ar):
    return ar 
    ap=math.degrees(ap)
    ar_opp=ar-360
    if ap>0:
        dst1=abs(ap-ar)
        dst2=abs(ap-ar_opp)
        if dst1<=dst2:
            return(ar)
        else:
            return(ar_opp)
    else:
        dst1=abs(ap-ar)
        dst2=abs(ap-ar_opp)
        if dst1<=dst2:
            return(ar)
        else:
            return(ar_opp)
        

#********************funz geometriche e matematiche fine****************************************

#===============================================================================
# inizio funzioni di mappatura
#===============================================================================





def inizializza_mappa():
    print("inizializzo mappe")
    cfg.mappa_relativa={}
    cfg.mappa_rel_ass={}
    cfg.radar_ini=1
    cfg.dst_prec=[]
    cfg.ang_prec=[]
    cfg.radar_ini_rel=1
    cfg.dst_prec_rel=[]
    cfg.ang_prec_rel=[]
    cfg.dst_prec_rel_ass=[]
    cfg.ang_prec_rel_ass=[]

   
def probabilita_ostacolo(distanza,angolo,tipo):
    #probabilita di ostacolo=1/numero di ostacoli possibili
    probabilita=0
    if tipo=='ok' and angolo==1:
        probabilita=-1
    if tipo=='ko' and angolo==1:
        probabilita=1
    if angolo>1 and tipo=='ko':
        a=ampiezza_laser(distanza)
        probabilita=1/a
    if angolo>1 and tipo=='ok':
        probabilita=-1
    return probabilita



def salva_lett_pre(tipo,ang,dst):
    if tipo=='assoluta':
        if ang<9999:
            cfg.ang_prec.append(ang)
        if dst>-1:
            cfg.dst_prec.append(dst)
    elif tipo=='rel_assoluta':
        if ang<9999:
            cfg.ang_prec_rel_ass.append(ang)
        if dst>-1:
            cfg.dst_prec_rel_ass.append(dst)
    else:
        if ang<9999:
            cfg.ang_prec_rel.append(ang)
        if dst>-1:
            cfg.dst_prec_rel.append(dst)



def crea_mappa(lettura,mappauso,tipo,verso,posattuale,dadb=0,dst=0,angolo=0):
    inizio=[0.0,0.0,0.0]
    cfg.dim_cella=1
    cfg.trova_cella_pos=10
    inizio[0]=float(posattuale[0])/10         #mi riporto all'origine degli assi
    inizio[1]=float(posattuale[1])/10
    inizio[2]=float(posattuale[2])
        #angolo=math.degrees(float(cfg.posatt[3]))-90+angolo
    if dst<0:
        print("lettura non valida")
        return
    if tipo=="assoluta":
        #print("distanza: ",dst)
        pass;
    salva_lett_pre(tipo, 9999, dst)
    salva_lett_pre(tipo, angolo, -1)
    if cfg.radar_ini==1:
        cfg.radar_ini=0
        salva_lett_pre(tipo, 9999, dst)
    test=1
    if dst>cfg.max_dst:
        test=2        #ritengo solo di non aver ostacoli fino alla massima distanza
    elif ampiezza_laser(dst)==1:
        test=99       #caso sicuro
                        

    rangeini=0
    rangefin=1
    if test==99:
        rangeini=angolo
        rangefin=angolo+1
        probok=1
        probko=1
    if test==0 and verso=='sx':
        rangeini=angolo+(cfg.angolo_radar/2)
        rangefin=angolo+(cfg.angolo_radar/2)+1
        probok=probabilita_ostacolo(dst,1,'ok')
        probko=probabilita_ostacolo(dst,1,'ko')
        
    if test==0 and verso=='dx':
        rangeini=angolo-(cfg.angolo_radar/2)
        rangefin=angolo-(cfg.angolo_radar/2)+1
        probok=probabilita_ostacolo(dst,1,'ok')
        probko=probabilita_ostacolo(dst,1,'ko')
    if test==1 and verso=='sx':
        rangeini=angolo-(cfg.angolo_radar/2)        #tutte occupate alla distanza prevista
        rangefin=angolo+(cfg.angolo_radar/2)
        probok=-100
        probko=probabilita_ostacolo(dst,cfg.angolo_radar ,'ko')
        crea_matrice_mappa(int(rangeini),int(rangefin), dst,inizio,mappauso,probok,probko)
        rangeini=angolo-(cfg.angolo_radar/2)
        rangefin=angolo-(cfg.angolo_radar/2)+1
        probok=probabilita_ostacolo(dst,1,'ok')
        probko=probabilita_ostacolo(dst,1,'ko')
        dst=cfg.dst_prec[-2]
  
    if test==1 and verso=='dx':
        rangeini=angolo-(cfg.angolo_radar/2)        #tutte occupate alla distanza prevista
        rangefin=angolo+(cfg.angolo_radar/2)
        probok=-100
        probko=probabilita_ostacolo(dst,cfg.angolo_radar ,'ko')
        crea_matrice_mappa(int(rangeini),int(rangefin), dst,inizio,mappauso,probok,probko)
        rangeini=angolo+(cfg.angolo_radar/2)+1
        rangefin=angolo+(cfg.angolo_radar/2)+2
        probok=probabilita_ostacolo(dst,1,'ok')
        probko=probabilita_ostacolo(dst,1,'ko')
    if test==2:
        cfg.dst_prec.append(dst)
        rangeini=angolo-(cfg.angolo_radar/2)        #tutte libere alla distanza prevista
        rangefin=angolo+(cfg.angolo_radar/2)
        probok=1
        probko=0
    if test==3:
        rangeini=angolo-(cfg.angolo_radar/2)        #tutte occupate alla distanza prevista
        rangefin=angolo+(cfg.angolo_radar/2)
        probok=probabilita_ostacolo(dst,cfg.angolo_radar ,'ok')
        probko=probabilita_ostacolo(dst,cfg.angolo_radar ,'ko')
        if probko<1:
            pass;
    crea_matrice_mappa(int(rangeini),int(rangefin), dst,inizio,mappauso,probok,probko)    
    

def crea_matrice_mappa(rangeini,rangefin,dst,inizio,mappauso,probok,probko):
    inserimentodb=""
    for ii in range (int(rangeini),int(rangefin)):         #se lettura infinita nella distanza metto 5 m e considero occupato da -30 a +30
        x,y=pos_fine(inizio, dst, ii)
        celleok,cellako=attraversamento(inizio[0],inizio[1],int(x),int(y))
        for i in celleok:
            probok=-100
            sicure=0
            inserimentodb=inserimentodb+"(current_timestamp,'"+str(cfg.id_radar)+"',"+str(i[0])+","+str(i[1])+","+str(probok)+","+str(sicure)+","+str(i[0])+","+str(i[1])+"),"
        for i in cellako:
            sicure=1
            ko_app=approssimocella(i)
            inserimentodb=inserimentodb+"(current_timestamp,'"+str(cfg.id_radar)+"',"+str(i[0])+","+str(i[1])+","+str(probko)+","+str(sicure)+","+str(ko_app[0])+","+str(ko_app[1])+"),"
        if abs(int(rangeini)-int(rangefin))>1:
            arianna_db.scrivo_db_mappa(ii,dst,tipo='c')
    arianna_db.scrivo_db_celle(inserimentodb[:-1])

    
def attraversamento(pix,piy,pfx,pfy):
    #cerco le celle attraversate da origine a fine
    celle_visitate=[]
    cellako=[]
    cellako.append(trova_cella_pos(pfx,pfy))
    if (pfx-pix)!=0:
        m = float(pfy - piy)/(pfx-pix)
    else:
        m=0
    q = piy - (m*pix)
    if abs(m)<1:
        if int(pfx)>int(pix):
            ini=int(pix)
            fin=int(pfx)
        else:
            ini=int(pfx)
            fin=int(pix)        
        for i in range(ini,fin,cfg.passo_attraversamento):
            y=m*i+q
            indice=trova_cella_pos(i,y)
            indice=approssimocella(indice)
            if indice not in (celle_visitate) and distanza_punti(indice, trova_cella_pos(pfx,pfy))>10:
                celle_visitate.append(indice)
    else:
        if int(pfy)>int(piy):
            ini=int(piy)
            fin=int(pfy)
        else:
            ini=int(pfy)
            fin=int(piy)        
        for i in range(ini,fin,cfg.passo_attraversamento):
            x=(i-q)/m
            indice=trova_cella_pos(x,i)
            indice=approssimocella(indice)
            if indice not in (celle_visitate) and distanza_punti(indice, trova_cella_pos(pfx,pfy))>10:
                celle_visitate.append(indice)        
    return celle_visitate,cellako

def trovadistanza(strx):
    cfg.tempo_radar=time.time()
    vett=[]
    a=strx.split("-")
    aa=a[2:-1]
    for ele in aa:
        distanza=float(ele)
        
        if distanza<=cfg.max_dst:
            vett.append(distanza)
            if distanza>100 and len(cfg.ultimo_angolo_libero)==0:
                cfg.ultimo_angolo_libero=[str(int(a[1])+cfg.errore_servo),distanza]
            if distanza>100 and cfg.ultimo_angolo_libero[1]<distanza:
                cfg.ultimo_angolo_libero=[str(int(a[1])+cfg.errore_servo),distanza]
                
    
    if len(vett)>=5:
        x=statistics.median(vett)
        arianna_db.scrivo_db_mappa(str(int(a[1])+cfg.errore_servo),x,'l')
        return x,int(a[1])+cfg.errore_servo
    
    else:
        return -1,int(str(int(a[1])+cfg.errore_servo))

def approssimocella(i):
    #considero cella =10x10
    nix=(math.floor(i[0]/10)*10)+5
    niy=(math.floor(i[1]/10)*10)+5
    return([nix,niy])
    




def mappa_seg(mappauso=cfg.mappa,tipo="assoluta",linee=[]):
    centro_arix=float(cfg.posatt[2])/10.0+5.0
    centro_ariy=float(cfg.posatt[3])/10.0+5.0
    if tipo=='assoluta':
        arianna="""
        {
                   type: 'rect',
                   xref: 'x',
                   yref: 'y',
                   x0: """+str(float(cfg.posatt[2])/10.0)+""",
                   y0: """+str(float(cfg.posatt[3])/10.0)+""",
                   x1: """+str(float(cfg.posatt[2])/10.0+10.0)+""",
                   y1: """+str(float(cfg.posatt[3])/10.0+10.0)+""",
                   line: {
                     color: 'rgb(0, 0, 255)',
                     width: 3
                   },
                   fillcolor: 'rgba(0, 0, 255, 0.7)'
                 }
        """
        centro_arix=float(cfg.posatt[2])/10.0+5.0
        centro_ariy=float(cfg.posatt[3])/10.0+5.0
        
        raggio_ari=5.0
        occhio_ariannax=catetobase(raggio_ari,float(cfg.posatt[4]))+centro_arix
        occhio_ariannay=catetoalt(raggio_ari,float(cfg.posatt[4]))+centro_ariy
        #print("centri",catetobase(raggio_ari,float(cfg.posatt[3])),catetoalt(raggio_ari,float(cfg.posatt[3])))
        direzione="""
        ,{
                     'type': 'circle',
                     'xref': 'x',
                     'yref': 'y',
                     'fillcolor': 'rgba(0, 0, 0, 1)',
                     'x0': """+str(occhio_ariannax-2.5)+""",
                     'y0': """+str(occhio_ariannay-2.5)+""",
                     'x1': """+str(occhio_ariannax+2.5)+""",
                     'y1': """+str(occhio_ariannay+2.5)+""",
                     'line': {
                         'color': 'rgba(0, 0, 0, 1)',
                     }}
        """
    p10x=""
    p07x=""
    p05x=""
    p02x=""
    p00x=""
    p10y=""
    p07y=""
    p05y=""
    p02y=""
    p00y=""
    for i in mappauso:
        visite=mappauso[i][0]
        positivi=mappauso[i][1]
        if positivi>0:
            prob=positivi
        else:
            prob=0
        certi=mappauso[i][2]
        posx=str(i).split("|")[0]
        posy=str(i).split("|")[1]
        if certi>0 or prob>0.99:
            p10x+=posx+","
            p10y+=posy+","
        elif prob>=0.75:
            p07x+=posx+","
            p07y+=posy+","
        elif prob>=0.5:
            p05x+=posx+","
            p05y+=posy+","
        elif prob>=0.2:
            p02x+=posx+","
            p02y+=posy+","
        else:
            p00x+=posx+","
            p00y+=posy+","
 
    
    if tipo=="assoluta":
        fi=open("scheda/mappaseg.html","r")
    if tipo=="assoluta":
        fo=open("scheda/mappasegok.html","w")
    pagina=fi.read()
    pagina=pagina.replace("***rettangoli***",arianna+direzione)
    pagina=pagina.replace("§xmin",str(int(centro_arix)-300))
    pagina=pagina.replace("§xmax",str(int(centro_arix)+300))
    pagina=pagina.replace("§ymin",str(int(centro_ariy)-300))
    pagina=pagina.replace("§ymax",str(int(centro_ariy)+300))
    pagina=pagina.replace("§px10",p10x)
    pagina=pagina.replace("§py10",p10y)
    pagina=pagina.replace("§px07",p07x)
    pagina=pagina.replace("§py07",p07y)
    pagina=pagina.replace("§px05",p05x)
    pagina=pagina.replace("§py05",p05y)
    pagina=pagina.replace("§px02",p02x)
    pagina=pagina.replace("§py02",p02y)
    pagina=pagina.replace("§px00",p00x)
    pagina=pagina.replace("§py00",p00y)
    pagina=pagina.replace("§linex",'')
    pagina=pagina.replace("§liney",'')
    #pagina=pagina.replace("§linex",linex[0:-1])
    #pagina=pagina.replace("§liney",liney[0:-1])
    fo.write(pagina)
    fo.close()



#===============================================================================
# fine utility mappa
#===============================================================================


#===============================================================================
# utility posizionamento e scansione
#===============================================================================


def controlla_new_pos(a, f):
    if len(a)<3 or len(f)<3:
        return False
    if a[2]==f[2] and a[3]==str(float(f[3])*cfg.invx) and a[4]==f[4] and a[6]==f[6]:
        return False
    else:
        return True
def deco_pos(a):
    pezzi=str(a).split(":")
    pezzi1=pezzi[1].split(";")
    pezzi1.append(0)
    return pezzi1
    
def cmdradar(comando):
    comando=comando[2:]
    print("comando",comando)
    l=comando.split('+')
    if int(l[0])<int(l[1]):
        cfg.versoradar='sx'
    else:
        cfg.versoradar='dx'
    
    r1=(3-len(l[0]))*'0'+str(l[0])
    r2=(3-len(l[1]))*'0'+str(l[1])
    r3=(2-len(l[2]))*'0'+str(l[2])
    return('1q'+r1+r2+r3)

def ampiezza_laser(dst):
    fraz_circ=360/cfg.angolo_radar
    circonferenza=math.pi*2*dst
    if (circonferenza/fraz_circ)<10:
        return 1
    else:
        return (circonferenza/fraz_circ)/10



#===============================================================================
# utility comunicazione
#===============================================================================

def idmsg():
    config=open("unicoid.txt","r")
    base=config.readline()[:2]
    config.close()
    for a in (string.ascii_uppercase+string.digits):
        for b in (string.ascii_uppercase+string.digits):
            if a+b=='ZZ':
                base='AA'
                config=open("unicoid.txt","w")
                config.write('AA')
                config.close()
                return('AA')
            if a+b>base:
                config=open("unicoid.txt","w")
                config.write(a+b)
                config.close()
                return(a+b)
def idmap():
    config=open("unicoma.txt","r")
    base=config.readline()[:4]
    config.close()
    for a in (string.ascii_uppercase+string.digits):
        for b in (string.ascii_uppercase+string.digits):
            for c in (string.ascii_uppercase+string.digits):
                for d in (string.ascii_uppercase+string.digits):
                    if a+b+c+d=='ZZZZ':
                        base='AAAA'
                        config=open("unicoma.txt","w")
                        config.write('AAAA')
                        config.close()
                        return('AAAA')
                    if a+b+c+d>base:
                        config=open("unicoma.txt","w")
                        config.write(a+b+c+d)
                        config.close()
                        return(a+b+c+d)


def gestiscirisp(stringa):
    #risposta=[]
    pezziok=[]
    pezziko=[]
    a=stringa.find("!")
    if a<0:
        pezziko.append(stringa)
    else:
        stringa=stringa[a:]
        while stringa.find("?")>0:
            a=stringa.find("?")
            pezziok.append(stringa[1:a])
            stringa=stringa[a+1:]
        pezziko.append(stringa[0:])
    return [pezziok,pezziko]

#===============================================================================
# utility db
#===============================================================================


            
        
        



