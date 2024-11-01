#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <filesystem>
#include <format>

#include <Client.h>
#include <Packet.h>

#define DIR_NAME "sync_dir"

using namespace std;

Client::Client() : clientSocket(-1)
{
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        cerr << "Erro ao criar o socket." << endl;
        exit(EXIT_FAILURE);
    }
}

Client::~Client()
{
    close(clientSocket);
}

bool Client::connectToServer(const string &serverIP, int serverPort)
{
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);

    if (inet_pton(AF_INET, serverIP.c_str(), &serverAddress.sin_addr) <= 0)
    {
        cerr << "Endereço IP inválido." << endl;
        return false;
    }

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        cerr << "Erro ao conectar ao servidor." << endl;
        return false;
    }
    char reply;
    // Todo daqui até a linha 70 pode virar um método genérico(de uma outra classe)
    Packet packet(1, MessageType::CONNECTION, Status::SUCCESS, username.c_str());
    char buffer[1024]; // TODO deve ser o suficiente por enquanto, mas adequa-lo posteriormente.

    send(clientSocket, packet.serialize(), packet.size(), 0);

    int result = recv(clientSocket, &buffer, sizeof(buffer), 0);

    if (result <= 0)
    {
        cerr << "Erro ao receber resposta do servidor." << endl;
        return false;
    }

    packet.deserialize(buffer);

    if (packet.isStatusError())
    {
        cerr << "Erro ao conectar ao servidor." << endl;
        return false;
    }

    cout << "Conectado ao servidor!" << endl;

    return true;
}

void Client::createSyncDir()
{
    if (!std::filesystem::exists(DIR_NAME))
        std::filesystem::create_directory(DIR_NAME);
}

void Client::setUsername(const string &user)
{
    username = user;
}

void Client::sendMessage()
{
    string message;
    while (true)
    {
        cout << "Digite uma mensagem para enviar: ";
        getline(cin >> ws, message);

        if (message == "exit")
        {
            Packet packet(1, MessageType::DISCONNECTION, Status::SUCCESS, username.c_str());
            send(clientSocket, packet.serialize(), packet.size(), 0);

            send(clientSocket, packet.serialize(), packet.size(), 0);

            send(clientSocket, message.c_str(), message.size(), 0);

            cout << "Encerrando conexão..." << endl;
            break;
        }

        string fullMessage = username + ": " + message;
        send(clientSocket, fullMessage.c_str(), fullMessage.size(), 0);
    }
}

bool Client::run(const string &serverIP, int serverPort)
{
    createSyncDir();
    return connectToServer(serverIP, serverPort);
}

void createPacket()
{
}