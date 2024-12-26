#include <netinet/in.h>
#include <optional>
#include <string>
#include <vector>
#include <thread>
#include <filesystem>

using namespace std;

class Server
{
    private:

        int serverSocket;
        struct sockaddr_in serverAddress;
        vector<thread> clientThreads;
        chrono::steady_clock::time_point lastHeartbeat;


        static void handle_client_activity(int socket_id, int secundary);
        static void backup_process(int socket_id, int secundary);
        static void heartbeatRequest();

    public:

        Server(string ip, int port);
        ~Server();
        void start();
        void startBackup(string &serverIp, int serverPort);
        void createSyncDir();
};