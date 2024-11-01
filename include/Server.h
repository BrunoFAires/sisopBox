#include <netinet/in.h>
#include <optional>
#include <string>
#include <vector>
#include <thread>

using namespace std;

class Server
{
    private:

        int serverSocket;
        struct sockaddr_in serverAddress;
        vector<thread> clientThreads;

        static void handle_client_activity(int socket_id);

    public:

        Server();
        ~Server();
        void start();
};