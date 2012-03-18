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
// float x, float y, float z, float pitch, float yaw

#endif // PROTOCOL_H
