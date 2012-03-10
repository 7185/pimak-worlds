#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

__version__ = 'pwlib, youpi!'

ENCODING = 'utf-8'

import socket
import threading
import sys
import io
from ctypes import c_uint16, c_uint32
import struct
import binascii
from protocol import *

sys.stdout = io.TextIOWrapper(sys.stdout.detach(), errors='backslashreplace', line_buffering=True)

def update_protocol(filename='../server/Protocol.h'):
    '''python -c "import pwlib; pwlib.update_protocol()" > protocol.py'''
    f = open(filename, 'r')
    for line in f.readlines():
        l = list(filter(None, line.split(' ')))
        if l[0] == '#define' and len(l)==3:
            print(l[1]+'='+l[2][:-1])

def qstring(string):
    msg = bytes(string,'utf-16-be')
    l = struct.pack('<I',struct.unpack('>I',bytes(c_uint32(len(msg))))[0])
    return l+msg

class PW(object):
    def __init__(self, loggingEnabled = True):
        self.loggingEnabled = loggingEnabled

        self.connected = False
        self.enabled = True
        self.x = 0
        self.y = 0
        self.w = 0
        self.pitch = 0
        self.yaw = 0
        self.handlers = {}

    def connect(self, host, port = 6667, use_ssl=False):
        '''Etablish a connection to a server'''
        self.log('@ Connecting to %s port %d' % (host, port))

        self.sk = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        if use_ssl:
            import ssl
            self.sk = ssl.wrap_socket(self.sk)

        self.sk.connect((host, port))
        self.log('@ Connected')
        self.connected = True
        self._callback('on_connected')

    def run(self):
        while self.enabled:
            recv = self.sk.recv(0x100)
            self._process_message(recv)

        self.connected = False
        self.log('@ Disconnected')
        self._callback('on_disconnnected')
        self.enabled = False

    def run_threaded(self):
        thread = threading.Thread(target=self.run)
        thread.setDaemon(True)
        thread.start()
        return thread

    def log(self, txt):
        if self.loggingEnabled:
            print(txt)

    def raw(self, b):
        self.sk.send(b)
        self.log('> ' + str(binascii.hexlify(b)))


    def send(self,code,msg):
        c = struct.pack('<H',struct.unpack('>H',bytes(c_uint16(code)))[0])
        s = qstring(msg)
        l = struct.pack('<H',struct.unpack('>H',bytes(c_uint16(len(c+s))))[0])
        self.raw(l+c+s)

    def auth(self, nick='Bot'):
        self.send(CS_AUTH, nick)

    def message_public(self, msg=''):
        self.send(CS_MSG_PUBLIC, msg)
        self._callback('on_self_message_public', msg)

    def message_private(self, user, msg=''):
        self.send(CS_MSG_PRIVATE, '3:'+msg) #TODO: Userlist
        self._callback('on_self_message_public', msg)

    def _callback(self, name, *parameters):
        for inst in [self] + list(self.handlers.values()):
            f = getattr(inst, name, None)
            if not hasattr(f, '__call__'):
                continue
            self.log('calling %s() on instance %r' % (name, inst))
            f(*parameters)

    def _process_message(self, b):
        self.log('!< '+ str(binascii.hexlify(b)))
        mlength = struct.unpack('>H',b[0:2])[0]
        code = struct.unpack('>H',b[2:4])[0]
        if (code == SC_ER_NICKINUSE):
            self._callback('on_connect_error',SC_ER_NICKINUSE)
        elif (code == SC_ER_ERRONEOUSNICK):
            self._callback('on_connect_error',SC_ER_ERRONEOUSNICK)
        elif (code == SC_MSG_PUBLIC):
            slength = struct.unpack('>L',b[4:8])[0]
            s = b[8:].decode('utf-16-be')
            if len(s)*2 == slength:
                params = s.split(':')
                self._callback('on_message_public', params[0], ':'.join(params[1:]))
        elif (code == SC_MSG_PRIVATE):
            slength = struct.unpack('>L',b[4:8])[0]
            s = b[8:].decode('utf-16-be')
            if len(s)*2 == slength:
                params = s.split(':')
                self._callback('on_message_private', params[0], ':'.join(params[1:]))
        elif (code == SC_USER_JOIN):
            slength = struct.unpack('>L',b[4:8])[0]
            s = b[8:].decode('utf-16-be')
            if len(s)*2 == slength:
                self._callback('on_join', s)
        elif (code == SC_USER_PART):
            slength = struct.unpack('>L',b[4:8])[0]
            s = b[8:].decode('utf-16-be')
            if len(s)*2 == slength:
                self._callback('on_part', s)
        else:
            self.log("Can't handle packet.")
            
 
