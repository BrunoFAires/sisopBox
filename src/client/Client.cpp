#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <filesystem>

#include <Client.h>
#include <Packet.h>
#include <Service.h>

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

Client::Client(string username, int clientSocket, sockaddr_in serverAddress) : username(username), clientSocket(clientSocket), serverAddress(serverAddress) {};

Client::~Client()
{
    close(clientSocket);
}

Client Client::connectToServer(const string &username, const string &serverIP, int serverPort)
{

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        cerr << "Erro ao criar o socket." << endl;
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);

    if (inet_pton(AF_INET, serverIP.c_str(), &serverAddress.sin_addr) <= 0)
    {
        cerr << "Endereço IP inválido." << endl;
        throw std::runtime_error("Endereço IP inválido.");
    }

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        cerr << "Erro ao conectar ao servidor." << endl;
        throw std::invalid_argument("Erro ao conectar ao servidor.");
    }

    Packet packet(1, 1, MessageType::CONNECTION, Status::SUCCESS, username.size(), username.c_str());
    sendPacket(clientSocket, packet);
    packet = receivePacket(clientSocket);

    if (packet.isStatusError())
    {
        throw std::invalid_argument("Erro ao conectar ao servidor.");
    }

    cout << "Conectado ao servidor!" << endl;

    this->username = username;
    this->clientSocket = clientSocket;
    this->serverAddress = serverAddress;

    return *this;
}

void Client::createSyncDir()
{
    if (!std::filesystem::exists(DIR_NAME))
        std::filesystem::create_directory(DIR_NAME);
}

void Client::sendMessage()
{
    while (true)
    {
    }
}

Client Client::run(const string &username, const string &serverIP, int serverPort)
{
    createSyncDir();
    return connectToServer(username, serverIP, serverPort);
}

string Client::getUsername()
{
    return this->username;
}

int Client::getSocketId()
{
    return this->clientSocket;
}

void Client::sync()
{
    while(true){
        Packet receivedPacket = receivePacket(clientSocket);
        if(receivedPacket.isSyncPacket()){
            cout << "chegou";
            receiveFile(receivedPacket, clientSocket, username, "sync_dir");
        }
    }
}

void Client::handle_client_command()
{
    while (true)
    {
        string command;

        if (getline(cin, command))
        {
            Packet packet(0, 1, MessageType::COMMAND, Status::SUCCESS, command.size(), command.c_str());
            
            sendPacket(clientSocket, packet);
        }
    }
}