#ifndef FILE_DISPATCHER_H
#define FILE_DISPATCHER_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <string>
#include <list>
#include <Packet.h>

#define DIR_NAME "sync_dir"
#define MAX_PACKET_SIZE 1024

using namespace std;

list<Packet> filePacking(const string fileName);
void fileUnpacking(list<Packet> &packets);

#endif