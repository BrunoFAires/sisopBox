#ifndef CLIENT_H
#define CLIENT_H
#include <netinet/in.h>
#include <string>

using namespace std;

class Client
{
private:
    string username;
    int clientSocket;
    struct sockaddr_in serverAddress;

public:
    Client();
    ~Client();
    bool connectToServer(const std::string &serverIP, int serverPort);
    void setUsername(const std::string &user); 
    void sendMessage();
};

#endif