#include "Client.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cerr << "Uso: " << argv[0] << " usuario ip porta" << endl;
        return 1;
    }

    string username = argv[1];
    string serverIP = argv[2];
    int serverPort = stoi(argv[3]);

    Client client;

    client.setUsername(username);

    if (client.connectToServer(serverIP, serverPort))
    {
        client.sendMessage();
    }

    return 0;
}
