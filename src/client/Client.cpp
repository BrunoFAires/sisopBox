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

    char reply;
    // Todo daqui até a linha 70 pode virar um método genérico(de uma outra classe)
    Packet packet(1, 1, MessageType::CONNECTION, Status::SUCCESS, username.c_str());
    sendPacket(clientSocket, packet);
    // Packet receivedPacket = receivePacket2(clientSocket);

    // if (receivedPacket.isStatusError())
    // {
    //     throw std::invalid_argument("Erro ao conectar ao servidor.");
    // }

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
    string message;
    string teste = "Lorem.ipsum.dolor.sit.amet,.consectetur.adipiscing.elit..Integer.ut.molestie.purus..Phasellus.dictum.enim.nec.erat.maximus.tincidunt..Praesent.arcu.metus,.vestibulum.non.tincidunt.et,.placerat.sed.dui..Donec.tincidunt.ligula.non.mauris.laoreet,.et.ultrices.mauris.interdum..Quisque.vestibulum.felis.ligula,.sit.amet.euismod.augue.suscipit.eget..Curabitur.maximus.felis.et.erat.volutpat.efficitur..Nam.eget.augue.vel.est.tincidunt.porttitor.ac.nec.sem..Aenean.eleifend.auctor.mattis..Quisque.placerat.";
    string teste2 = "AAAAm.ipsum.dolor.sit.amet,.consectetur.adipiscing.elit..Integer.ut.molestie.purus..Phasellus.dictum.enim.nec.erat.maximus.tincidunt..Praesent.arcu.metus,.vestibulum.non.tincidunt.et,.placerat.sed.dui..Donec.tincidunt.ligula.non.mauris.laoreet,.et.ultrices.mauris.interdum..Quisque.vestibulum.felis.ligula,.sit.amet.euismod.augue.suscipit.eget..Curabitur.maximus.felis.et.erat.volutpat.efficitur..Nam.eget.augue.vel.est.tincidunt.porttitor.ac.nec.sem..Aenean.eleifend.auctor.mattis..Quisque.placerat.";
    cout << "teste";
    Packet packet3(1, 2, MessageType::DATA, Status::SUCCESS, teste.c_str());
    sendPacket(clientSocket, packet3);
    Packet packet4(2, 2, MessageType::DATA, Status::SUCCESS, teste2.c_str());
    sendPacket(clientSocket, packet4);

    while (true)
    {
        cout << "Digite uma mensagem para enviar: ";
        getline(cin >> ws, message);

        if (message == "exit")
        {
            cout << clientSocket << endl;
            cout << username << endl; // TODO veriricar o porquê de estar vindo lixo.
            Packet packet(1, 1, MessageType::DISCONNECTION, Status::SUCCESS, "bruno");
            send(clientSocket, packet.serialize(), packet.size(), 0);
            cout << "Encerrando conexão..." << endl;
            break;
        }

        string fullMessage = username + ": " + message;
        send(clientSocket, fullMessage.c_str(), fullMessage.size(), 0);
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