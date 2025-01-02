#include "Notify.h"
#include "Client.h"
#include <iostream>
#include <string>
#include <thread>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        cerr << "Uso: " << argv[0] << " usuario ip ipServidor portaServidor" << endl;
        return 1;
    }

    string username = argv[1];
    string clientIp = argv[2];
    string serverIP = argv[3];
    int serverPort = stoi(argv[4]);

    try
    {
        Client client = client.run(username, clientIp, serverIP, serverPort);
        Notify notify(&client);


        thread watcherThread1(&Notify::init, &notify);
        thread watcherThread3(&Client::cli, &client);
        thread watcherThread2(&Client::sync, &client);
        
        watcherThread1.join();
        watcherThread2.join();
        watcherThread3.join();
    }
    catch (runtime_error &e)
    {
        cout << e.what();
    }

    return 0;
}
