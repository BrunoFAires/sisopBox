#include "Server.h"
#include <iostream>
#include <string.h>

using namespace std;

int main(int argc, char *argv[])
{

    if (argc < 4)
    {
        cerr << "Uso: " << argv[0] << " ip porta p|b porta ipServidorPrincipal" << endl;
        return 1;
    }

    string ip = argv[1];
    int porta = stoi(argv[2]);

    Server server(ip, porta);

    if (strcmp(argv[3], "p") == 0)
    {
        server.start();
    }
    else
    {
        string serverIp = argv[5];
        server.startBackup(serverIp, stoi(argv[4]));
    }

    return 0;
}
