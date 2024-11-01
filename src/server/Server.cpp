#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <format>

#include <Server.h>
#include <global_settings.h>

#define PORT 5002

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
    listen(serverSocket, 5);
    cout << "Servidor escutando na porta " << ntohs(serverAddress.sin_port) << endl;

    while (true)
    {
        int socket_id = accept(serverSocket, nullptr, nullptr);

        if (socket_id < 0)
        {
            cerr << "Erro ao aceitar conexão." << endl;
            continue;
        }

        clientThreads.push_back(std::thread(handle_client_activity, socket_id));
    }
}

void Server::handle_client_activity(int socket_id)
{
    char buffer[1024];

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = recv(socket_id, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0)
        {
            cout << "Conexão com o cliente encerrada ou erro." << endl;

            string client_name = global_settings::socket_id_dictionary.get(socket_id);

            bool success = global_settings::disconnect_client(socket_id, client_name);

            break;
        }
        else
        {
            string message(buffer);

            if (message.starts_with("startup: "))
            {
                size_t pos = message.find(' ');

                string client_name = (pos != std::string::npos) ? message.substr(pos + 1) : "";
             
                bool success = global_settings::connect_client(socket_id, client_name);

                string reply;

                if (success)
                {
                    reply = "startup";

                    send(socket_id, reply.c_str(), reply.size(), 0);
                }
                else
                {
                    reply = "startup";

                    send(socket_id, reply.c_str(), reply.size(), 0);
                }
            }
            else if (message.starts_with("exit: "))
            {
                size_t pos = message.find(' ');

                string client_name = (pos != std::string::npos) ? message.substr(pos + 1) : "";

                bool success = global_settings::disconnect_client(socket_id, client_name);
            }
            else
            {
                cout << buffer << endl;               
            }
        }
    }

    close(socket_id);
}