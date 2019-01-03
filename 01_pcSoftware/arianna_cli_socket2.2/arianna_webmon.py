#!/usr/bin/env python3
"""
Very simple HTTP server in python for logging requests
Usage::
    ./server.py [<port>]
"""
import threading
from http.server import BaseHTTPRequestHandler, HTTPServer
import config as cfg

def dati():
    t='<head><META HTTP-EQUIV="refresh" CONTENT="2"></head>'
    if cfg.stato[0]!=0:
        t+='<body bgcolor="red">'
    else:
        t+='<body bgcolor="green">'   
    
    t+='<table border="1">'
    t+='<tr>'
    t+='<td>posx</td><td>posy</td><td>ang</td>'
    t+='</tr>'
    t+='<tr>'
    t+='<td>'+str(int(float(cfg.posatt[2])))+'</td><td>'+str(int(float(cfg.posatt[3])))+'</td><td>'+str(int(float(cfg.posatt[4])))+'</td>'
    t+='</tr>'
    t+='</table>'
    t+='</body>'
    return t
           






class S(BaseHTTPRequestHandler):
    def _set_response(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def do_GET(self):
        self.raw_requestline
        self._set_response()
        if str(self.raw_requestline).find('posizione')>=0:
            self.wfile.write(format(dati()).encode('utf-8'))
        else:
            self.wfile.write(format("").encode('utf-8'))
    def log_message(self, format, *args):
        return


class serverwebmon (threading.Thread):
    def __init__(self, threadID, name,porta):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        self.porta=porta
    
    def run(self,server_class=HTTPServer, handler_class=S, port=''):
        server_address = ('', self.porta)
        httpd = server_class(server_address, handler_class)
        try:
            httpd.serve_forever()
        except:
            pass

