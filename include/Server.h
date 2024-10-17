#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <string>
#include <vector>
#include <thread>

class Server
{
private:
    int serverSocket;
    struct sockaddr_in serverAddress;
    std::vector<std::thread> clientThreads;

    static void handleClient(int clientSocket);

public:
    Server();
    ~Server();
    void start();
};

#endif