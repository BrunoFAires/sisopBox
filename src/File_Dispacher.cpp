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

list<Packet> filePacking(const string fileName)
{

    ifstream file(string(DIR_NAME) + "/" + fileName, std::ios::binary);

    if (!file.is_open())
    {
        cerr << "ERROR: couldn't open file." << endl;
        // TODO: algum jeito de interromper a função em caso de erro.
    }

    list<Packet> filePackets;
    uint32_t id = 1;
    size_t bytesToRead = 0;
    std::streampos remaining_bytes = 0;

    remaining_bytes = file.tellg();
    file.seekg(0, std::ios::end);
    remaining_bytes = file.tellg() - remaining_bytes;
    file.seekg(0, std::ios::beg);

    while (remaining_bytes > 0)
    {
        if (remaining_bytes >= MAX_PACKET_SIZE)
        {
            bytesToRead = MAX_PACKET_SIZE;
            remaining_bytes -= MAX_PACKET_SIZE;
        }
        else
        {
            bytesToRead = remaining_bytes;
            remaining_bytes = 0;
        }
        vector<char> buffer(bytesToRead);

        file.read(buffer.data(), bytesToRead);
        std::streamsize readBytes = file.gcount();

        if (readBytes > 0)
        {
            Packet packet(id, MessageType::DATA, Status::SUCCESS, buffer.data());
            filePackets.push_back(packet);
            id++;
        }
    }

    file.close();

    Packet fileNamePacket(0, MessageType::DATA, Status::SUCCESS, fileName.data());
    fileNamePacket.setMessage(fileName);
    filePackets.push_front(fileNamePacket);

    return filePackets;
}

void fileUnpacking(list<Packet> &packets)
{

    string fileName = packets.front().getMessage();
    packets.pop_front();

    std::ofstream file(string(DIR_NAME) + "/" + fileName, std::ios::binary | std::ios::trunc);

    if (!file)
    {
        cerr << "ERROR: couldn't create or locate file." << endl;
        return;
    }

    for (const auto &packet : packets)
    {
        const char *message = packet.getMessage();
        uint16_t messageSize = packet.getMessageSize();

        file.write(message, messageSize);
    }

    file.close();
}