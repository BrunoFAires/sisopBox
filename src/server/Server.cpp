#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <string>

#include <Server.h>
#include <Packet.h>
#include <global_settings.h>

#define PORT 5000

using namespace std;

Server::Server()
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0)
    {
        cerr << "Erro ao criar o socket do servidor." << endl;
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        cerr << "Erro ao vincular o socket." << endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
}

Server::~Server()
{
    close(serverSocket);

    for (auto &th : clientThreads)
    {
        if (th.joinable())
        {
            th.join();
        }
    }
}

void Server::start()
{
    if (listen(serverSocket, 5) < 0)
    {
        cerr << "Erro ao escutar na porta." << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Servidor escutando na porta " << ntohs(serverAddress.sin_port) << endl;

    while (true)
    {
        int socket_id = accept(serverSocket, nullptr, nullptr);
        if (socket_id < 0)
        {
            cerr << "Erro ao aceitar conexão." << endl;
            continue;
        }
        clientThreads.emplace_back([this, socket_id]()
                                   { this->handle_client_activity(socket_id); });
    }
}

void Server::handle_client_activity(int socket_id)
{
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = recv(socket_id, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0)
        {
            cerr << "Conexão encerrada ou erro ao receber dados." << endl;
            global_settings::disconnect_client(socket_id, global_settings::socket_id_dictionary.get(socket_id));
            break;
        }

        Packet receivedPacket;
        receivedPacket.deserialize(buffer);

        if (receivedPacket.isConnectionPacket())
        {
            bool success = global_settings::connect_client(socket_id, receivedPacket.getMessage());
            std::string message = success ? "Conexão bem-sucedida." : "Erro ao conectar.";
            Packet replyPacket(1, MessageType::CONNECTION, success ? Status::SUCCESS : Status::ERROR, message.c_str());

            auto serializedData = replyPacket.serialize();
            send(socket_id, serializedData, replyPacket.size(), 0);
        }
        else if (receivedPacket.isDisconnectionPacket())
        {
            cout << receivedPacket.getMessage() << endl;
            global_settings::disconnect_client(socket_id, global_settings::socket_id_dictionary.get(socket_id));
            break;
        }
    }
    close(socket_id);
}