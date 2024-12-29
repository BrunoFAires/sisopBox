#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <filesystem>

#include <Server.h>
#include <Service.h>
#include <Util.h>
#include <Packet.h>
#include <global_settings.h>
#include <arpa/inet.h>

#define DIR_NAME "dir"
#define PORT 5000

using namespace std;

void createDir(const char *dirName)
{
    if (!std::filesystem::exists(dirName))
        std::filesystem::create_directory(dirName);
}

Server::Server(string ip, int port)
{
    lerArquivo();
    createDir(DIR_NAME);
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0)
    {
        cerr << "Erro ao criar o socket do servidor." << endl;
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = inet_addr(ip.c_str());
    lastHeartbeat = chrono::steady_clock::now();

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

    std::thread heartbeatThread(heartbeatRequest);
    heartbeatThread.detach();

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

void Server::backupReceivePacket(int socket_id)
{
    while (true)
    {
        Packet receivedPacket = receivePacket(socket_id);
        processHeartbeat(receivedPacket, socket_id);
        processPacket(receivedPacket, socket_id);
    }
}

void Server::checkLastHeartbeat(int socket_id)
{
    while (true)
    {
        if (lastHeartbeat + chrono::seconds(12) < chrono::steady_clock::now())
        {
            string ip = inet_ntoa(serverAddress.sin_addr);
            string porta = to_string(ntohs(serverAddress.sin_port));
            string origem = ip + ":" + porta;

            cout << "Servidor primário desconectado, iniciar eleição" << endl;
            string destino = buscarDestino(origem);
            cout << "Enviar mensagem para o vizinho: " << destino << endl;
            startElection(destino);
            break;
        }
    }
}

void Server::startBackup(string &serverIp, string &principalServerIp, int principalServerPort)
{

    int newClientSocket = socket(AF_INET, SOCK_STREAM, 0);

    printf("%s, %d\n", principalServerIp.c_str(), principalServerPort);

    struct sockaddr_in newServerAddress;
    newServerAddress.sin_family = AF_INET;
    newServerAddress.sin_port = htons(principalServerPort);

    if (inet_pton(AF_INET, principalServerIp.c_str(), &newServerAddress.sin_addr) <= 0)
    {
        cerr << "Endereço IP inválido." << endl;
        throw runtime_error("Endereço IP inválido.");
    }

    if (connect(newClientSocket, (struct sockaddr *)&newServerAddress, sizeof(newServerAddress)) < 0)
    {
        cerr << "Erro ao conectar ao servidor." << endl;
        throw invalid_argument("Erro ao conectar ao servidor.");
    }

    Packet packet(1, 1, MessageType::CONNECTION_SERVER, Status::SUCCESS, serverIp.size(), serverIp.c_str());
    sendPacket(newClientSocket, packet);

    Packet receivedPacket = receivePacket(newClientSocket);

    if (!receivedPacket.isStatusError())
    {
        createDir("secundario");
        std::thread client_activity([this, newClientSocket]()
                                    { this->backupReceivePacket(newClientSocket); });
        std::thread checkHeartbeat([this, newClientSocket]()
                                   { this->checkLastHeartbeat(newClientSocket); });
        client_activity.join();
        checkHeartbeat.join();

        serverAddress2.sin_family = AF_INET;
        serverAddress2.sin_port = htons(666);
        serverAddress2.sin_addr.s_addr = serverAddress.sin_addr.s_addr;

        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

        if (listen(serverSocket, 5) < 0)
        {
            cerr << "Erro ao escutar na porta." << endl;
            exit(EXIT_FAILURE);
        }
        cout << "Servidor escutando na porta " << ntohs(serverAddress2.sin_port) << endl;

        while (true)
        {
            int socket_id = accept(serverSocket, nullptr, nullptr);
            if (socket_id < 0)
            {
                cerr << "Erro ao aceitar conexão." << endl;
                continue;
            }
            cout << "Conexão aceita" << endl;
        }
    }
    else
    {
        cout << "Erro ao conectar ao servidor principal" << endl;
    }
}

void Server::startElection(string destination)
{

    string ipVizinho = destination.substr(0, destination.find(':'));
    string portaVizinho = destination.substr(destination.find(':') + 1, destination.size());
    cout << ipVizinho << endl;
    int newClientSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in newServerAddress;
    newServerAddress.sin_family = AF_INET;
    newServerAddress.sin_port = htons(stoi(portaVizinho));

    cout << "enviado 1" << endl;
    if (inet_pton(AF_INET, ipVizinho.c_str(), &newServerAddress.sin_addr) <= 0)
    {
        cerr << "Endereço IP inválido." << endl;
        throw runtime_error("Endereço IP inválido.");
    }

    cout << "enviado2" << endl;
    int a = connect(newClientSocket, (struct sockaddr *)&newServerAddress, sizeof(newServerAddress));
    cout << "a: " << a << endl;
    if (a < 0)
    {
        cout << "erro" << endl;
        cerr << "Erro ao conectar ao servidor." << endl;
        throw invalid_argument("Erro ao conectar ao servidor.");
    }

    cout << "enviado 3" << endl;
    Packet packet(1, 1, MessageType::HEARTBEAT, Status::SUCCESS, ipVizinho.size(), ipVizinho.c_str());
    cout << "enviado" << endl;
    sendPacket(newClientSocket, packet);
}

void sendClientInfo(int socketId, string info1, string info2, MessageType messageType)
{
    string message = info1 + ":" + info2;
    Packet packet(1, 1, messageType, Status::SUCCESS, message.size(), message.c_str());
    sendPacket(socketId, packet);
}

void Server::heartbeatRequest()
{
    while (true)
    {
        vector<string> secundaryIps = global_settings::servers.keys();
        for (string secundaryIp : secundaryIps)
        {
            cout << "ip Secundario " << secundaryIp << endl;
            int secondarySocketId = global_settings::servers.get(secundaryIp);
            cout << "Heartbeat enviado " << to_string(secondarySocketId) << endl;
            Packet packet(1, 1, MessageType::HEARTBEAT, Status::SUCCESS, 0, "");
            try
            {
                sendPacket(secondarySocketId, packet);
            }
            catch (const std::exception &e)
            {
                global_settings::servers.remove(secundaryIp);
                cout << "Servidor secundário desconectado" << endl;
            }
        }
        this_thread::sleep_for(chrono::seconds(5));
    }
}

void Server::processHeartbeat(Packet receivedPacket, int socket_id)
{
    if (receivedPacket.isHeartbeatPacket())
    {
        lastHeartbeat = chrono::steady_clock::now();
        cout << "Heartbeat recebido " << to_string(socket_id) << endl;
    }
    else
    {
        cout << "Outro pacote " << endl;
    }
}

void Server::processPacket(Packet receivedPacket, int socket_id)
{
    if (receivedPacket.isDataPacket())
    {
        receiveFile(receivedPacket, socket_id, nullopt, "secundario");
    }
    else if (receivedPacket.isDeletePacket())
    {

        remove(receivedPacket.getMessage());
    }
}

void Server::handle_client_activity(int socket_id)
{
    while (true)
    {

        Packet receivedPacket = receivePacket(socket_id);

        if (receivedPacket.isConnectionPacket())
        {
            string fullMessage = receivedPacket.getMessage();
            string username = fullMessage.substr(0, fullMessage.find(':'));
            string clientIp = fullMessage.substr(fullMessage.find(':'), fullMessage.size());

            bool success = global_settings::connect_client(socket_id, username, clientIp);
            std::string message = success ? "Conexão bem-sucedida." : "Erro ao conectar.";

            if (success)
            {
                Packet replyPacket(1, 1, MessageType::CONNECTION, Status::SUCCESS, message.size(), message.c_str());
                string userDirFolderName = string(DIR_NAME) + "/" + username;
                createDir(userDirFolderName.c_str());
                sendPacket(socket_id, replyPacket);

                sendClientInfo(socket_id, username, to_string(socket_id), MessageType::CLIENT);
                sendClientInfo(socket_id, to_string(socket_id), username, MessageType::SOCKET);
                sendClientInfo(socket_id, to_string(socket_id), clientIp, MessageType::IP);
            }
            else
            {
                Packet replyPacket(1, 1, MessageType::DISCONNECTION, Status::ERROR, 0, "");
                sendPacket(socket_id, replyPacket);
                break;
            }
        }
        else if (receivedPacket.isDisconnectionPacket())
        {
            global_settings::disconnect_client(socket_id, global_settings::socket_id_dictionary.get(socket_id));
            vector<string> secundaryIps = global_settings::servers.keys();
            for (string secundaryIp : secundaryIps)
            {
                int secondarySocketId = global_settings::servers.get(secundaryIp);
                sendPacket(secondarySocketId, receivedPacket);
            }
            break;
        }
        else if (receivedPacket.isDataPacket())
        {
            string username = global_settings::socket_id_dictionary.get(socket_id);
            receiveFile(receivedPacket, socket_id, username, "dir");
            auto syncDeviceSocket = global_settings::socket_id_dictionary.findFirstDifferentValue(username, socket_id);
            if (syncDeviceSocket)
            {
                string filename = receivedPacket.getMessage();
                string dirName = "dir/" + username;
                sendFile(*syncDeviceSocket, dirName, filename, true, false);
            }

            // Tornar isso uma função, por hora replicar nos demais ifs
            // pensando bem tem muita coisa pra refatorar nesse método.

            vector<string> secundaryIps = global_settings::servers.keys();
            for (string secundaryIp : secundaryIps)
            {
                int secundarySocketId = global_settings::servers.get(secundaryIp);
                string filename = receivedPacket.getMessage();
                string dirName = "dir/" + username;
                string message = username + "/" + filename;
                // sendFile(secundarySocketId, dirName, message, false, false);
            }
        }
        else if (receivedPacket.isDeletePacket())
        {

            string username = global_settings::socket_id_dictionary.get(socket_id);
            auto syncDeviceSocket = global_settings::socket_id_dictionary.findFirstDifferentValue(username, socket_id);
            string path = "dir/" + username + "/" + receivedPacket.getMessage();
            remove(path.c_str());
            if (syncDeviceSocket)
            {
                sendPacket(*syncDeviceSocket, receivedPacket);
            }

            vector<string> secundaryIps = global_settings::servers.keys();
            for (string secundaryIp : secundaryIps)
            {
                int secundarySocketId = global_settings::servers.get(secundaryIp);
                string message = "secundario/" + username + "/" + receivedPacket.getMessage();
                Packet packet(1, 1, MessageType::DELETE, Status::SUCCESS, message.size(), message.c_str());
                sendPacket(secundarySocketId, packet);
            }
        }
        else if (receivedPacket.isFetchPacket())
        {
            string username = global_settings::socket_id_dictionary.get(socket_id);
            syncFiles(socket_id, "dir", username);
        }
        else if (receivedPacket.isDownloadPacket())
        {
            string username = global_settings::socket_id_dictionary.get(socket_id);
            string filename = receivedPacket.getMessage();
            string dirName = "dir/" + username;
            sendFile(socket_id, dirName, filename, false, true);
        }
        else if (receivedPacket.isInfoPacket())
        {
            string username = receivedPacket.getMessage();
            string dirName = "dir/" + username;
            string result = listfFilesInfo(dirName);
            Packet replyPacket(1, 1, MessageType::INFO, Status::SUCCESS, result.size(), result.c_str());
            sendPacket(socket_id, replyPacket);
        }
        else if (receivedPacket.isConnectionServer())
        {
            string serverIp = receivedPacket.getMessage();
            cout << "ip " << serverIp << endl;
            bool success = global_settings::connect_server(socket_id, serverIp);
            std::string message = success ? "Conexão bem-sucedida do servidor secundário" : "Erro ao conectar.";
            cout << message << endl;
            if (success)
            {
                Packet replyPacket(1, 1, MessageType::CONNECTION_SERVER, Status::SUCCESS, message.size(), message.c_str());
                sendPacket(socket_id, replyPacket);
            }
            else
            {
                Packet replyPacket(1, 1, MessageType::CONNECTION_SERVER, Status::ERROR, 0, "");
                sendPacket(socket_id, replyPacket);
                break;
            }

            vector<string> clientUserNames = global_settings::client_name_dictionary.keys();
            for (string clientUsername : clientUserNames)
            {
                sendClientInfo(socket_id, clientUsername, to_string(global_settings::client_name_dictionary.get(clientUsername)), MessageType::CLIENT);
            }

            vector<int> sockets = global_settings::socket_id_dictionary.keys();
            for (int socket : sockets)
            {
                sendClientInfo(socket_id, to_string(socket), global_settings::socket_id_dictionary.get(socket), MessageType::SOCKET);
                sendClientInfo(socket_id, to_string(socket), global_settings::client_ip.get(socket), MessageType::IP);
            }
        }
        else if (receivedPacket.isClientPacket())
        {
            string fullMessage = receivedPacket.getMessage();
            string username = fullMessage.substr(0, fullMessage.find(':'));
            string qtd_str = fullMessage.substr(fullMessage.find(':') + 1, fullMessage.size());
            // cout << "Recebido username: " << fullMessage << endl;
            int qtd = stoi(qtd_str);
            global_settings::client_name_dictionary.insert_or_update(username, qtd);
            string dirName = "secundario/" + username;
            createDir(dirName.c_str());
        }
        else if (receivedPacket.isSocketPacket())
        {
            string fullMessage = receivedPacket.getMessage();
            string socketId = fullMessage.substr(0, fullMessage.find(':'));
            string username = fullMessage.substr(fullMessage.find(':') + 1, fullMessage.size());

            // cout << "Recebido socket: " << fullMessage << endl;
            int socketIdInt = std::stoi(socketId);
            global_settings::socket_id_dictionary.insert_or_update(socketIdInt, username);
        }
        else if (receivedPacket.isIpPacket())
        {
            string fullMessage = receivedPacket.getMessage();
            string socketId = fullMessage.substr(0, fullMessage.find(':'));
            ;
            string ip = fullMessage.substr(fullMessage.find(':') + 1, fullMessage.size());

            // cout << "Recebido Ip: " << fullMessage << endl;

            int socketIdInt = std::stoi(socketId);
            global_settings::client_ip.insert_or_update(socketIdInt, ip);
        }
    }
    close(socket_id);
}