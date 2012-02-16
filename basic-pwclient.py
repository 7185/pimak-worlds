#!/usr/bin/env python3

from ctypes import c_uint16, c_uint32
import sys
import socket
import struct
import binascii

def qstring(string):
    msg = bytes(string,'utf-16-be')
    l = struct.pack('<I',struct.unpack('>I',bytes(c_uint32(len(msg))))[0])
    return l+msg

def sendmsg(code,msg):
    c = struct.pack('<H',struct.unpack('>H',bytes(c_uint16(code)))[0])
    s = qstring(msg)
    l = struct.pack('<H',struct.unpack('>H',bytes(c_uint16(len(c+s))))[0])
    return l+c+s


if len(sys.argv)!=3:
  sys.stderr.write('usage: %s destination port\n'%sys.argv[0])
  sys.exit(1)

ipAddress=socket.gethostbyname(sys.argv[1])
portNumber=int(sys.argv[2])
clientSocket=socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
toAddr=(ipAddress, portNumber)
clientSocket.connect(toAddr)

msgcode = sendmsg(0,'user')
clientSocket.send(msgcode)
print('sent: '+str(binascii.hexlify(msgcode)))

while True:
  m=sys.stdin.readline()
  if not m: break
  msg = sendmsg(1,m)
  clientSocket.send(msg)
  #print('sent: '+str(binascii.hexlify(msg)))

  msg=clientSocket.recv(0x100)
  if msg:
   #print('recv: '+str(binascii.hexlify(msg)))
    print(msg.decode('utf-16-be'))

clientSocket.close()

