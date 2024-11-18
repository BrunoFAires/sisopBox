#ifndef CLIENT_H
#define CLIENT_H
#include <netinet/in.h>
#include <string>
#include <atomic>

using namespace std;

class Client
{
private:
    string username;
    int clientSocket;
    struct sockaddr_in serverAddress;
    std::atomic<bool> stopRequested{false};
    Client connectToServer(const string &username, const string &serverIP, int serverPort);
    void createSyncDir();
    void createClientDownloadDir();
    void processCommand(const string commandLine);
    void signalHandler(int signal);

public:
    Client();
    Client(string username, int clientSocket, sockaddr_in serverAddress);
    ~Client();
    void setUsername(const std::string &user);
    string getUsername();
    int getSocketId();
    void sendMessage();
    Client run(const string &username, const string &serverIP, int serverPort);
    void sync();
    void cli();
};

#endif