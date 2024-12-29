#include <netinet/in.h>
#include <optional>
#include <string>
#include <vector>
#include <thread>
#include <filesystem>
#include "Packet.h"

using namespace std;

class Server
{
private:
    int serverSocket;
    struct sockaddr_in serverAddress;
    vector<thread> clientThreads;
    chrono::steady_clock::time_point lastHeartbeat;

    static void handle_client_activity(int socket_id);
    static void backup_process(int socket_id, int secundary);
    static void heartbeatRequest();
    void processHeartbeat(Packet receivedPacket, int socket_id);
    void processPacket(Packet receivedPacket, int socketId);
    void backupReceivePacket(int socket_id);
    void checkLastHeartbeat(int socket_id);

public:
    Server(string ip, int port);
    ~Server();
    void start();
    void startBackup(string &serverIp, string &principalServerIp, int principalServerPort);
    void createSyncDir();
};