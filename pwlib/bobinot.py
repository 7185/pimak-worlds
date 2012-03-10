#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

from pwlib import *
from time import strftime

class Bot(PW):
    def __init__(self, *args, **kwargs):
        super(Bot, self).__init__(*args, **kwargs)
        self.loggingEnabled = False
        self.nick = 'bobinota'
        self.connect('heldroe.org',1338)

    def on_connected(self):
        self.auth(self.nick)
        self.message_public('alo')

    def on_self_message_public(self,msg):
        self.display('>'+self.nick+'< '+msg)

    def on_message_public(self,user,msg):
        self.display('<'+user+'> '+msg)
        if 'alo' in msg.split(): 
            self.message_public(user+': alo')

    def on_message_private(self,user,msg):
        self.display('-'+user+'- '+msg)
        if 'alo' in msg.split(): 
            self.message_private(user,'alo')

    def on_join(self,user):
        self.display('* '+user+' has joined')

    def on_part(self,user):
        self.display('* '+user+' has left')
    def on_connect_error(self,reason):
        if reason == SC_ER_NICKINUSE:
            self.display('! Diconnected (nick already taken)')
        elif reason == SC_ER_ERRONEOUSNICK:
            self.display('! Diconnected (illegal nick)')
    def display(self,msg):
        print(strftime("[%H:%M:%S] ")+msg)

b = Bot()
b.run()

