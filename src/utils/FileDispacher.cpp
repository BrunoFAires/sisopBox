#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <string>
#include <list>
#include <Packet.h>
#include <vector>
#include <cmath>
#include <iostream>

#include "FileDispacher.h"

#define DIR_NAME "sync_dir"
#define MAX_PACKET_SIZE 1024

using namespace std;

list<Packet> filePacking(const string fileName)
{
    std::ifstream file(std::string(DIR_NAME) + "/" + fileName, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "ERROR: Couldn't open file." << std::endl;
        return {}; // Retorna lista vazia em caso de erro
    }

    // Determinar tamanho do arquivo
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::list<Packet> filePackets;
    uint32_t packetId = 1;
    uint32_t totalPackets = std::ceil(static_cast<double>(fileSize) / MAX_PACKET_SIZE) + 1;

    cout << "bata:" << totalPackets;

    while (fileSize > 0)
    {
        size_t bytesToRead = std::min(static_cast<std::streampos>(MAX_PACKET_SIZE), fileSize);
        std::vector<char> buffer(bytesToRead);

        file.read(buffer.data(), bytesToRead);
        std::streamsize readBytes = file.gcount();

        if (readBytes > 0)
        {
            Packet packet(packetId, totalPackets, MessageType::DATA, Status::SUCCESS, buffer.data());
            filePackets.push_back(packet);
            packetId++;
            fileSize -= readBytes;
        }
    }

    file.close();

    // Criar pacote com o nome do arquivo
    Packet fileNamePacket(0, filePackets.size() + 1, MessageType::DATA, Status::SUCCESS, fileName.c_str());
    filePackets.push_front(fileNamePacket);

    return filePackets;
}

void fileUnpacking(list<Packet> &packets)
{

    string fileName = packets.front().getMessage();
    packets.pop_front();

    std::ofstream file("teste/" + fileName, std::ios::binary | std::ios::trunc);

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