#include "Client.h"
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>

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

    cout << "Conectado ao servidor!" << endl;
    return true;
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
            cout << "Encerrando conexão..." << endl;
            break;
        }
        
        string fullMessage = username + ": " + message;
        send(clientSocket, fullMessage.c_str(), fullMessage.size(), 0);
    }
}
