#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

from pwlib import *
from time import strftime

class Bot(PW):
    def __init__(self, *args, **kwargs):
        super(Bot, self).__init__(*args, **kwargs)
        self.loggingEnabled = False
        self.nickname = 'bobinot'
        self.connect('7185.fr', 6670)
        self.following = 0

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
        elif '!loc' in m:  
            self.message_public(user+': je suis en '+str(int(self.x))+', '+str(int(self.y))+', '+str(int(self.z)))
        elif '!list' in m:
            s = ''
            for u in self.userlist:
                s+=str(u)+':'+self.userlist[u].nickname+' '
            self.message_public(s)
        elif '!follow' in m:
            u = self.getidbynick(user)
            if self.following != u:
                self.following = u
            else:
                self.following = 0
    def on_message_private(self,user,msg):
        self.display('-'+user+'- '+msg)
        if 'alo' in msg.split(): 
            self.message_private(user,'slt')
    def on_avatar_position(self,user,x,y,z,pi,ya):
        if self.following == user:
            u = self.userlist[user]
            self.setposition(u.x,u.y,u.z,u.pitch,u.yaw)
            self.sendposition()
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

