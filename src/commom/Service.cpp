
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

void receiveFile(Packet packet, int socket_id, optional<string> username, string dirName)
{
    Packet recevedPacket;
    char buffer[2500];
    int totalBytesReceived = 0;
    list<Packet> filePackets;

    string dir = dirName;

    if (username)
    {
        dir = dir + "/" + *username;
    }

    filePackets.push_front(packet);
    for (int i = 0; i < packet.getTotalPackets() - 1; i++)
    {
        Packet receivedPacket = receivePacket(socket_id);
        filePackets.push_back(receivedPacket);
    }

    fileUnpacking(filePackets, dir);
}

void sendFile(int socket_id, string dir, string filename)
{
    sendFile(socket_id, dir, filename, false);
}

void sendFile(int socket_id, string dir, string filename, bool syncFile)
{
    list<Packet> packets = filePacking(dir, filename, syncFile);
    for (const auto &packet : packets)
    {
        sendPacket(socket_id, packet);
    }
}

void syncFiles(int socket_id, string dir, string username)
{
    try
    {
        string dirName = dir + "/" + username;
        for (const auto& entry : std::filesystem::directory_iterator(dirName))
        {
            if (entry.is_regular_file())
            {
                const std::string& filename = entry.path().filename().string(); 
                sendFile(socket_id, dirName, filename, true);
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "Erro ao acessar o diretório: " << e.what() << '\n';
    }
}

void deleteFile(string filePath)
{
    cout << filePath << endl;
    remove(filePath.c_str());
}

void getSyncDir(int socket_id, string username)
{
    Packet packet(0, 1, MessageType::FETCH, Status::SUCCESS, username.size(), username.c_str());
    sendPacket(socket_id, packet);
}