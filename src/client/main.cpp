#include "Notify.h"
#include "Client.h"
#include <iostream>
#include <string>
#include <thread>

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
    Notify notify;

    // Inicia a thread corretamente, passando o ponteiro de membro e o objeto notify
    thread watcherThread(&Notify::init, &notify);

    watcherThread.detach();  // Usa detach para rodar de forma independente

    client.setUsername(username);

    if (client.run(serverIP, serverPort))
    {
        client.sendMessage();
    }

    return 0;
}