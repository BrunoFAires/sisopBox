#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include <sys/socket.h>
#include <Packet.h>
#include <list>

using namespace std;

void sendPacket(int socketdId, Packet packet);
Packet receivePacket(int socket_id);
void receiveFile(Packet packet, int socket_id, string username);
void sendFile(int socket_id, string filename);

#endif
