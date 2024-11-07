#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <string>
#include <list>
#include <Packet.h>

#include "File_Dispatcher.h"

#define DIR_NAME "sync_dir"
#define MAX_PACKET_SIZE 1024

using namespace std;

list<Packet> filePacking(const string fileName){

    ifstream file(string(DIR_NAME) + "/" + fileName, std::ios::binary);

    if(!file.is_open()){
        cerr << "ERROR: couldn't open file." << endl;
        return;
    }

    vector<char> buffer(MAX_PACKET_SIZE);
    list<Packet> filePackets;
    uint32_t id = 1;

    while(file){
        file.read(buffer.data(), MAX_PACKET_SIZE);
        std::streamsize readBytes = file.gcount();

        if(readBytes > 0){
            Packet packet(id, MessageType::DATA, Status::SUCCESS, buffer.data());
            if(readBytes == packet.getMessageSize()){
                filePackets.push_back(packet);
                id++;
            } else{
                cerr << "ERROR: couldn't pack file." << endl;
                return;
            }
        }
    }

    file.close();

    Packet fileNamePacket(0, MessageType::DATA, Status::SUCCESS, fileName.data());
    filePackets.push_front(fileNamePacket);

    return filePackets;
}

void fileUnpacking(list<Packet>& packets){

    string filePath = packets.front().getMessage();
    packets.pop_front();

    std::ofstream file(filePath, std::ios::binary | std::ios::trunc);

    if(!file){
        cerr << "ERROR: couldn't create or locate file." << endl;
        return;
    }

    for(const auto& packet : packets){
        const char* message = packet.getMessage();
        uint16_t messageSize = packet.getMessageSize();

        file.write(message, messageSize);
    }

    file.close();
}