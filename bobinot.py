#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

from pwlib import *

class Bot(PW):
    def __init__(self, *args, **kwargs):
        super(Bot, self).__init__(*args, **kwargs)
        self.loggingEnabled = False
        self.nick = 'bobinot'
        self.connect('heldroe.org',1338)

    def on_connected(self):
        self.auth(self.nick)
        self.message('alo')

    def on_self_message(self,msg):
        print('>'+self.nick+'< '+msg)

    def on_message(self,user,msg):
        print('<'+user+'> '+msg)
        if 'alo' in msg.split(): 
            self.message(user+': alo')

    def on_join(self,user):
        print('* '+user+' has joined')

    def on_part(self,user):
        print('* '+user+' has left')

b = Bot()
b.run()

