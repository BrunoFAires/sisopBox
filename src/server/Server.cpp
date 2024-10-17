#include "Server.h"
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

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
    serverAddress.sin_port = htons(5000);
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
    listen(serverSocket, 5);
    cout << "Servidor escutando na porta " << ntohs(serverAddress.sin_port) << endl;

    while (true)
    {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0)
        {
            cerr << "Erro ao aceitar conexão." << endl;
            continue;
        }

        clientThreads.push_back(std::thread(handleClient, clientSocket));
    }
}

void Server::handleClient(int clientSocket)
{
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0)
        {
            cout << "Conexão com o cliente encerrada ou erro." << endl;
            break;
        }

        cout << "Mensagem do cliente: " << buffer << endl;
    }

    close(clientSocket);
}
