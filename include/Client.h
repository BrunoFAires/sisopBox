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
        bool connectToServer(const std::string &serverIP, int serverPort);
        void createSyncDir();
        
    public:
        Client();
        ~Client();
        void setUsername(const std::string &user); 
        void sendMessage();
        bool run(const string &serverIP, int serverPort);
};

#endif