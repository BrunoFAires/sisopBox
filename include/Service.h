#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include <sys/socket.h>
#include <Packet.h>
#include <list>
#include <filesystem>
#include <string.h>
#include "FileDispacher.h"

#include <Service.h>
#include <optional>

using namespace std;

void sendPacket(int socketdId, Packet packet);
Packet receivePacket(int socket_id);
void receiveFile(Packet packet, int socket_id, optional<string> username, string dirName);
void sendFile(int socket_id, string dir, string filename);
void sendFile(int socket_id, string dir, string filename, bool syncFile);
void deleteFile(string filePath);
void getSyncDir();
void syncFiles(int socket_id, string dir, string username);

#endif
