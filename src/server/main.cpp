#include "Server.h"
#include <iostream>
#include <string.h>

using namespace std;

int main(int argc, char *argv[])
{   
    char* serverType = argv[1];

    if (strcmp(serverType, "p") == 0)
    {
        if(argc < 3){
        cerr << "Uso: " << argv[0] << "p porta" << endl;
        return 1;
        }

        int porta = stoi(argv[2]);
        Server server(porta);

        server.start();
    }
    else if(strcmp(serverType, "b") == 0)
    {
        if(argc < 5){
        cerr << "Uso: " << argv[0] << "b porta portaServidorPrincipal ipServidorPrincipal" << endl;
        return 1;
        }

        int porta = stoi(argv[2]);
        Server server(porta);
        
        string principalServerIp = argv[4];
        int principalServerPort = stoi(argv[3]);
        server.startBackup(principalServerIp, principalServerPort);
    }else{
        cerr << "Uso: " << argv[0] << "p porta ou b porta portaServidorPrincipal ipServidorPrincipal" << endl;
        return 1;
    }

    return 0;
}
