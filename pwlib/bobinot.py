#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

from pwlib import *
from time import strftime

class Bot(PW):
    def __init__(self, *args, **kwargs):
        super(Bot, self).__init__(*args, **kwargs)
        self.loggingEnabled = True
        self.nickname = 'bobinota'
        self.connect('heldroe.org',1338)

    def on_connected(self):
        self.auth()
        self.message_public('alo')

    def on_self_message_public(self,msg):
        self.display('>'+self.nickname+'< '+msg)

    def on_message_public(self,user,msg):
        self.display('<'+user+'> '+msg)
        m = msg.split()
        if 'alo' in m: 
            self.message_public(user+': slt')
        elif '!list' in m:
            s = ''
            for u in self.userlist:
                s+=u+':'+self.userlist[u].nickname+' '
            self.message_public(s)
        elif 'viens' in m:
            u = self.userlist[self.getidbynick(user)]
            self.setposition(u.x,u.y,u.z,u.pitch,u.yaw)
            self.message_public('je vais en : '+str(u.x)+', '+str(u.y)+', '+str(u.z))
            self.sendposition()

    def on_message_private(self,user,msg):
        self.display('-'+user+'- '+msg)
        if 'alo' in msg.split(): 
            self.message_private(user,'slt')

    def on_user_join(self,user):
        self.display('* '+user+' has joined')

    def on_user_part(self,user):
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

