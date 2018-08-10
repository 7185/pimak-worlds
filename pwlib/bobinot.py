#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

import threading
from pwlib import *
from math import atan2, pi
from time import sleep, strftime


class Bot(PW):
    def __init__(self, *args, **kwargs):
        super(Bot, self).__init__(*args, **kwargs)
        self.logging_enabled = False
        self.nickname = 'bobinot'
        self.connect('7185.fr', 6670)
        self.following = 0
        self.move_speed = 5
        self.current_move_thread = 0

    def move(self, dest_x, dest_z):
        thread_id = self.current_move_thread
        tick = 200
        length = ((dest_x - self.x) ** 2 + (dest_z - self.z) ** 2) ** 0.5
        direction = atan2(dest_x - self.x, dest_z - self.z) + pi
        n = int(length * (1 / self.move_speed))
        if n > 0:
            x_gap = (dest_x - self.x) / n
            z_gap = (dest_z - self.z) / n
            gaps = [[self.x + i * x_gap, self.z + i * z_gap] for i in range(1, n + 1)]
        else:
            gaps = [[dest_x, dest_z]]
        for p in gaps:
            if thread_id != self.current_move_thread:
                break
            self.set_position(p[0], self.y, p[1], yaw=direction)
            self.send_position()
            sleep(tick / 1e3)

    def on_connected(self):
        self.auth()
        self.message_public('alo')
        self.set_position(1430, 10, 960)
        self.send_position()

    def on_self_message_public(self, msg):
        self.display('>' + self.nickname + '< ' + msg)

    def on_message_public(self, user, msg):
        self.display('<' + user + '> ' + msg)
        m = msg.split()
        if 'alo' in m:
            self.message_public(user + ': slt')
        elif '!loc' in m:
            self.message_public(
                user + ': je suis en ' + str(int(self.x)) + ', ' + str(int(self.y)) + ', ' + str(int(self.z)))
        elif '!list' in m:
            s = ''
            for u in self.userlist:
                s += str(u) + ':' + self.userlist[u].nickname + ' '
            self.message_public(s)
        elif '!follow' in m:
            u = self.get_id_by_nick(user)
            if self.following != u:
                self.following = u
            else:
                self.following = 0
        elif '!come' in m:
            uid = self.get_id_by_nick(user)
            u = self.userlist[uid]
            self.current_move_thread += 1
            thread = threading.Thread(target=self.move, args=(u.x, u.z))
            thread.daemon = True
            thread.start()
        elif '!speed' in m and len(m) > 1 and m[1].isdigit():
            self.move_speed = int(m[1])

    def on_message_private(self, user, msg):
        self.display('-' + user + '- ' + msg)
        if 'alo' in msg.split():
            self.message_private(user, 'slt')

    def on_avatar_position(self, user, x, y, z, pi, ya):
        if self.following == user:
            u = self.userlist[user]
            self.set_position(u.x, u.y, u.z, u.pitch, u.yaw)
            self.send_position()

    def on_user_join(self, user):
        self.display('* ' + user + ' has joined')

    def on_user_part(self, user):
        self.display('* ' + user + ' has left')

    def on_connect_error(self, reason):
        if reason == SC_ER_NICKINUSE:
            self.display('! Diconnected (nick already taken)')
        elif reason == SC_ER_ERRONEOUSNICK:
            self.display('! Diconnected (illegal nick)')

    def display(self, msg):
        print(strftime("[%H:%M:%S] ") + msg)


b = Bot()
b.run()
