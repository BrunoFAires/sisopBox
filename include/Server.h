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

        static void handle_client_activity(int socket_id, int secundary);
        static void backup_process(int socket_id, int secundary);

    public:

        Server(string ip, int port);
        ~Server();
        void start();
        void startBackup(string &serverIp, int serverPort);
        void createSyncDir();
};