#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

from pwlib import *

class Bot(PW):
    def __init__(self, *args, **kwargs):
        super(Bot, self).__init__(*args, **kwargs)
        self.connect('heldroe.org',1338)
    def on_connected(self):
        self.send(0,'bobinot') #auth
        self.send(1,'alo')

    def on_message(self,user,msg):
        if 'alo' in msg.split(): 
            self.send(1,user+': alo')

b = Bot()
b.run()
