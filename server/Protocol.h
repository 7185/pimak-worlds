/**
 * Copyright (c) 2012, Thibault Signor <tibsou@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

//-----------------
// Client -> Server
//-----------------

#define CS_AUTH             0x00
// QString nickname
#define CS_MSG_PUBLIC       0x01
// QString nickname:message
#define CS_MSG_PRIVATE      0x02
// QString nickname:message
#define CS_HEARTBEAT        0x04
// Not implemented yet
#define CS_USER_LIST        0x06
// NULL
#define CS_AVATAR_POSITION  0x10
// float x, float y, float z, float pitch, float yaw


//-----------------
// Server -> Client
//-----------------

#define SC_MSG_PUBLIC       0x01
// QString nickname:message
#define SC_MSG_PRIVATE      0x02
// QString nickname:message
#define SC_MSG_EVENT        0x03
// Not implemented yet
#define SC_USER_JOIN        0x04
// QString nickname
#define SC_USER_PART        0x05
// QString nickname
#define SC_USER_LIST        0x06
// QString id1:nickname1;id2:nickname2...
#define SC_ER_NICKINUSE     0x07
// NULL
#define SC_ER_ERRONEOUSNICK 0x08
// NULL
#define SC_AVATAR_POSITION  0x10
// quint16 id, float x, float y, float z, float pitch, float yaw

#endif // PROTOCOL_H
