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

    try
    {
        Client client = client.run(username, serverIP, serverPort);
        Notify notify(&client);
        thread watcherThread1(&Notify::init, &notify);
        thread watcherThread2(&Client::sync, &client);
        thread watcherThread3(&Client::handle_client_command, &client);
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
