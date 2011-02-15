#ifndef PROTOCOL_H
#define PROTOCOL_H

/******************************************************
 * Packet
 *
 *    quint16     quint16            QString
 * +-----------+-----------+------------------------+
 * |messageSize|messageCode|         message        |
 * +-----------+-----------+------------------------+
 *       |
 *       +---->[     sizeof(messageCode+message)    ]
 *
 ******************************************************/

// Client -> Server
#define CS_AUTH       0
#define CS_PUBMSG     1
#define CS_PRIVMSG    2
#define CS_USERLIST   6

// Server -> Client
#define SC_PUBMSG     1
#define SC_PRIVMSG    2
#define SC_EVENT      3
#define SC_JOIN       4
#define SC_PART       5
#define SC_USERLIST   6
#define SC_NICKINUSE  7
#define SC_ERRONEOUSNICK 8

#endif
