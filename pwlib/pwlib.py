#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

# Copyright (c) 2012, Thibault Signor <tibsou@gmail.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import asyncio
import binascii
import io
import struct
import sys
from typing import Callable
from ctypes import c_uint16, c_uint32, c_double
from contextlib import suppress
from protocol import *

__version__ = '0.2'


HEARTBEAT_RATE = 60

sys.stdout = io.TextIOWrapper(sys.stdout.detach(), errors='backslashreplace', line_buffering=True)


def update_protocol(filename: str='../server/Protocol.h') -> None:
    """python -c "import pwlib; pwlib.update_protocol()" > protocol.py"""
    f = open(filename, 'r')
    for line in f.readlines():
        l = list(filter(None, line.split(' ')))
        if l[0] == '#define' and len(l) == 3:
            print(l[1] + '=' + l[2][:-1])


def qstring(string: str) -> bytes:
    msg = bytes(string, 'utf-16-be')
    l = struct.pack('<I', struct.unpack('>I', bytes(c_uint32(len(msg))))[0])
    return l + msg


class Timer:
    def __init__(self, timeout: int, callback: Callable) -> None:
        self._timeout = timeout
        self._callback = callback
        self._task = asyncio.ensure_future(self._job())

    async def _job(self) -> None:
        await asyncio.sleep(self._timeout)
        await self._callback()

    async def cancel(self) -> None:
        if not self._task.cancelled():
            self._task.cancel()
            with suppress(asyncio.CancelledError):
                await self._task


class User:
    def __init__(self, nick: str="") -> None:
        self.nickname = nick
        self.x = 0
        self.y = 0
        self.z = 0
        self.pitch = 0
        self.yaw = 0

    def set_position(self, x: float = 0.0, y: float = 0.0, z: float = 0.0, pitch: float = 0.0, yaw: float = 0.0):
        self.x = x
        self.y = y
        self.z = z
        self.pitch = pitch
        self.yaw = yaw


class PW(User):
    def __init__(self, host: str, port: int, logging_enabled: bool=True) -> None:
        super(PW, self).__init__()
        self.host = host
        self.port = port
        self.logging_enabled = logging_enabled
        self.connected = False
        self.enabled = True
        self.handlers = {}
        self.userlist = {}
        self.reader = None
        self.writer = None
        self.heartbeat = None

    def log(self, txt: str) -> None:
        if self.logging_enabled:
            print(txt)

    def get_id_by_nick(self, nick: str) -> int:
        for i in self.userlist.items():
            if i[1].nickname == nick:
                return i[0]

    async def send(self, msg: bytes) -> None:
        self.writer.write(msg)
        self.writer.drain()

    async def recv(self) -> asyncio.StreamReader:
        data = await self.reader.read(0x100)
        return data

    async def raw(self, b: bytes):
        await self.send(b)
        if self.heartbeat:
            await self.heartbeat.cancel()
        self.heartbeat = Timer(HEARTBEAT_RATE, self.send_heartbeat)
        self.log('> ' + str(binascii.hexlify(b)))

    async def connect(self, host: str, port: int) -> None:
        self.reader, self.writer = await asyncio.open_connection(host=host, port=port)

        self.log('@ Connected')
        self.connected = True
        await self.send_heartbeat()
        await self._callback('on_connected')

    async def loop(self) -> None:
        await self.connect(self.host, self.port)
        while self.enabled:
            r = await self.recv()
            await self._process_packet(r)

        self.connected = False
        self.log('@ Disconnected')
        await self._callback('on_disconnected')
        self.enabled = False

    def run(self) -> None:
        # all the following will be replaced by: asyncio.run(self.loop())
        loop = asyncio.get_event_loop()
        try:
            loop.run_until_complete(self.loop())
        finally:
            loop.run_until_complete(loop.shutdown_asyncgens())
            loop.close()

    async def send_msg(self, code: int, msg: str) -> None:
        c = struct.pack('<H', struct.unpack('>H', bytes(c_uint16(code)))[0])
        s = qstring(msg)
        l = struct.pack('<H', struct.unpack('>H', bytes(c_uint16(len(c + s))))[0])
        await self.raw(l + c + s)

    async def ask_list(self) -> None:
        c = struct.pack('<H', struct.unpack('>H', bytes(c_uint16(CS_USER_LIST)))[0])
        l = struct.pack('<H', struct.unpack('>H', bytes(c_uint16(len(c))))[0])
        await self.raw(l + c)

    async def send_position(self) -> None:
        c = struct.pack('<H', struct.unpack('>H', bytes(c_uint16(CS_AVATAR_POSITION)))[0])
        x = struct.pack('<d', struct.unpack('>d', bytes(c_double(self.x)))[0])
        y = struct.pack('<d', struct.unpack('>d', bytes(c_double(self.y)))[0])
        z = struct.pack('<d', struct.unpack('>d', bytes(c_double(self.z)))[0])
        pi = struct.pack('<d', struct.unpack('>d', bytes(c_double(self.pitch)))[0])
        ya = struct.pack('<d', struct.unpack('>d', bytes(c_double(self.yaw)))[0])
        l = struct.pack('<H', struct.unpack('>H', bytes(c_uint16(len(c + x + y + z + pi + ya))))[0])
        await self.raw(l + c + x + y + z + pi + ya)

    async def send_heartbeat(self) -> None:
        c = struct.pack('<H', struct.unpack('>H', bytes(c_uint16(CS_HEARTBEAT)))[0])
        l = struct.pack('<H', struct.unpack('>H', bytes(c_uint16(len(c))))[0])
        await self.raw(l + c)

    async def auth(self) -> None:
        await self.send_msg(CS_AUTH, self.nickname)

    async def message_public(self, msg: str='') -> None:
        await self.send_msg(CS_MSG_PUBLIC, msg)
        await self._callback('on_self_message_public', msg)

    async def message_private(self, user: str, msg: str='') -> None:
        await self.send_msg(CS_MSG_PRIVATE, str(self.get_id_by_nick(user)) + ':' + msg)
        await self._callback('on_self_message_private', user, msg)

    async def _callback(self, name: str, *parameters) -> None:
        for inst in [self] + list(self.handlers.values()):
            f = getattr(inst, name, None)
            if not hasattr(f, '__call__'):
                continue
            self.log('calling %s() on instance %r' % (name, inst))
            if f is not None:
                await f(*parameters)

    async def _process_packet(self, b: bytes) -> None:
        self.log('< ' + str(b))
        plength = struct.unpack('>H', b[0:2])[0]
        code = struct.unpack('>H', b[2:4])[0]
        if code == SC_ER_NICKINUSE:
            await self._callback('on_connect_error', SC_ER_NICKINUSE)
        elif code == SC_ER_ERRONEOUSNICK:
            await self._callback('on_connect_error', SC_ER_ERRONEOUSNICK)
        elif code == SC_MSG_PUBLIC:
            slength = struct.unpack('>L', b[4:8])[0]
            s = b[8:].decode('utf-16-be')
            if len(s) * 2 == slength:
                params = s.split(':')
                await self._callback('on_message_public', params[0], ':'.join(params[1:]))
        elif code == SC_MSG_PRIVATE:
            slength = struct.unpack('>L', b[4:8])[0]
            s = b[8:].decode('utf-16-be')
            if len(s) * 2 == slength:
                params = s.split(':')
                await self._callback('on_message_private', params[0], ':'.join(params[1:]))
        elif code == SC_USER_LIST:
            slength = struct.unpack('>L', b[4:8])[0]
            s = b[8:].decode('utf-16-be')
            if len(s) * 2 == slength:
                l = s.split(';')
                self.userlist.clear()
                for c in l:
                    self.userlist[int(c.split(':')[0])] = User(c.split(':')[1])
                await self._callback('on_user_list')
        elif code == SC_AVATAR_POSITION:
            u = struct.unpack('>H', b[4:6])[0]
            x = struct.unpack('>d', b[6:14])[0]
            y = struct.unpack('>d', b[14:22])[0]
            z = struct.unpack('>d', b[22:30])[0]
            pitch = struct.unpack('>d', b[30:38])[0]
            yaw = struct.unpack('>d', b[38:46])[0]
            if u in self.userlist.keys():
                self.userlist[u].set_position(x, y, z, pitch, yaw)
                await self._callback('on_avatar_position', u, x, y, z, pitch, yaw)
        elif code == SC_USER_JOIN:
            slength = struct.unpack('>L', b[4:8])[0]
            s = b[8:].decode('utf-16-be')
            if len(s) * 2 == slength:
                await self.ask_list()
                await self._callback('on_user_join', s)
        elif code == SC_USER_PART:
            slength = struct.unpack('>L', b[4:8])[0]
            s = b[8:].decode('utf-16-be')
            if len(s) * 2 == slength:
                await self.ask_list()
                await self._callback('on_user_part', s)
        else:
            self.log("! Can't handle packet.")
