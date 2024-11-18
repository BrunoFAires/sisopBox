#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <filesystem>
#include <csignal>

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
        throw runtime_error("Endereço IP inválido.");
    }

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        cerr << "Erro ao conectar ao servidor." << endl;
        throw invalid_argument("Erro ao conectar ao servidor.");
    }

    Packet packet(1, 1, MessageType::CONNECTION, Status::SUCCESS, username.size(), username.c_str());
    sendPacket(clientSocket, packet);
    packet = receivePacket(clientSocket);

    if (packet.isStatusError())
    {
        throw invalid_argument("Erro ao conectar ao servidor.");
    }

    cout << "Conectado ao servidor!" << endl;

    this->username = username;
    this->clientSocket = clientSocket;
    this->serverAddress = serverAddress;

    return *this;
}

void Client::createSyncDir()
{
    if (!filesystem::exists(DIR_NAME))
        filesystem::create_directory(DIR_NAME);
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
    Packet packet(0, 1, MessageType::FETCH, Status::SUCCESS, username.size(), username.c_str());
    sendPacket(clientSocket, packet);
    while (true)
    {
        Packet receivedPacket = receivePacket(clientSocket);
        if (receivedPacket.isSyncPacket())
        {
            receiveFile(receivedPacket, clientSocket, nullopt, "sync_dir");
        }
        else if (receivedPacket.isDeletePacket())
        {
            string path = "sync_dir/" + string(receivedPacket.getMessage());
            deleteFile(path);
        }
        else if (receivedPacket.isDownloadPacket())
        {
            string path = "download/" + string(receivedPacket.getMessage());
            receiveFile(receivedPacket, clientSocket, nullopt, "download");
        }
        else if (receivedPacket.isInfoPacket())
        {
            cout << receivedPacket.getMessage() << endl;
        }
    }
}

void Client::processCommand(const string commandLine)
{
    istringstream stream(commandLine);
    string command, argument;
    stream >> command;
    getline(stream, argument);

    if (!argument.empty())
    {
        size_t pos = argument.find_first_not_of(" ");
        if (pos != std::string::npos)
        {
            argument = argument.substr(pos);
        }
        else
        {
            argument.clear();
        }
    }

    if (command == "upload")
    {
        if (!argument.empty())
        {
            sendFile(clientSocket, "sync_dir", argument);
        }
        else
        {
            cout << "Erro: O nome do arquivo é obrigatório.\n";
        }
    }
    else if (command == "download")
    {
        if (!argument.empty())
        {
            Packet packet(0, 1, MessageType::DOWNLOAD, Status::SUCCESS, argument.size(), argument.c_str());
            sendPacket(clientSocket, packet);
        }
        else
        {
            cout << "Erro: O nome do arquivo é obrigatório.\n";
        }
    }
    else if (command == "delete")
    {
        if (!argument.empty())
        {
            Packet packet(0, 1, MessageType::DELETE, Status::SUCCESS, argument.size(), argument.c_str());
            sendPacket(clientSocket, packet);
            string path = "sync_dir/" + argument;
            remove(path.c_str());
        }
        else
        {
            cout << "Erro: O nome do arquivo é obrigatório.\n";
        }
    }
    else if (command == "list_server")
    {
        Packet packet(0, 1, MessageType::INFO, Status::SUCCESS, username.size(), username.c_str());
        sendPacket(clientSocket, packet);
    }
    else if (command == "list_client")
    {
        cout << listfFilesInfo("sync_dir") << endl;
    }
    else if (command == "get_sync_dir")
    {
        getSyncDir(clientSocket, username);
    }
    else if (command == "exit")
    {
        Packet packet(0, 1, MessageType::DISCONNECTION, Status::SUCCESS, 0, "");
        sendPacket(clientSocket, packet);
        exit(0);
    }
    else
    {
        cerr << "Error: Unknown command.\n";
    }
}

void Client::cli()
{

    std::string input;

    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin, input);

        processCommand(input);
    }
}