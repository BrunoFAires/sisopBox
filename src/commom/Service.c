#include <Service.h>

void sendPacket(int socket_id, Packet packet)
{
    int totalBytesSent = 0;

    const char *buffer = packet.serialize();
    int packetSize = packet.size();

    while (totalBytesSent < packetSize)
    {
        int bytesSent = send(socket_id, buffer + totalBytesSent, packetSize - totalBytesSent, 0);
        if (bytesSent == -1)
        {
            throw std::runtime_error("Erro ao enviar pacote pelo socket.");
        }

        totalBytesSent += bytesSent;
    }

    delete[] buffer;
}

Packet receivePacket(int socket_id)
{
    Packet recevedPacket;
    char buffer[2500];
    int totalBytesReceived = 0;
    int headerSize = recevedPacket.headerSize();

    while (totalBytesReceived < headerSize)
    {
        int bytesRead = recv(socket_id, buffer + totalBytesReceived, headerSize - totalBytesReceived, 0);

        if (bytesRead == -1)
        {
            throw std::runtime_error("Erro ao receber pacote do socket. 1");
        }

        if (bytesRead == 0)
        {
            throw std::runtime_error("A conexão foi fechada pelo cliente.");
        }

        totalBytesReceived += bytesRead;
    }

    totalBytesReceived = 0;
    recevedPacket.deserialize(buffer);
    cout << recevedPacket.getMessageSize();

    while (totalBytesReceived < recevedPacket.getMessageSize())
    {
        int bytesRead = recv(socket_id, buffer + headerSize + totalBytesReceived, recevedPacket.getMessageSize() - totalBytesReceived, 0);

        if (bytesRead == -1)
        {
            throw std::runtime_error("Erro ao receber pacote do socket.");
        }

        if (bytesRead == 0)
        {
            throw std::runtime_error("A conexão foi fechada pelo cliente.");
        }

        totalBytesReceived += bytesRead;
    }

    recevedPacket.deserialize(buffer);

    if (recevedPacket.isStatusError())
    {
        throw std::invalid_argument("Ocorreu um erro ao processar o pacote recebido.");
    }

    return recevedPacket;
}

void receiveFile(Packet packet, int socket_id)
{
    Packet recevedPacket;
    char buffer[2500];
    int totalBytesReceived = 0;
    list<Packet> filePackets;

    filePackets.push_front(packet);
    cout << packet.getTotalPackets();
    for (int i = 0; i < packet.getTotalPackets() - 1; i++)
    {
        Packet receivedPacket = receivePacket(socket_id);
        filePackets.push_back(receivedPacket);
    }

    for (const auto &pkt : filePackets)
    {
        cout << "a" << endl;
        cout << pkt.getMessage() << endl;
    }
}