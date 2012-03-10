#ifndef PROTOCOL_H
#define PROTOCOL_H

// Client -> Server
#define CS_AUTH             0x00
#define CS_MSG_PUBLIC       0x01
#define CS_MSG_PRIVATE      0x02
#define CS_HEARTBEAT        0x04
#define CS_USER_LIST        0x06

#define CS_AVATAR_POSITION  0x10

// Server -> Client
#define SC_MSG_PUBLIC       0x01
#define SC_MSG_PRIVATE      0x02
#define SC_MSG_EVENT        0x03
#define SC_USER_JOIN        0x04
#define SC_USER_PART        0x05
#define SC_USER_LIST        0x06
#define SC_ER_NICKINUSE     0x07
#define SC_ER_ERRONEOUSNICK 0x08

#define SC_AVATAR_POSITION  0x10

#endif
