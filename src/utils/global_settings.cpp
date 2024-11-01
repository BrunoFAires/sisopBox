#include <string>
#include <format>
#include <iostream>

#include <global_settings.h>

using namespace std;

concurrent_dictionary<string, int> global_settings::client_name_dictionary;
concurrent_dictionary<int, string> global_settings::socket_id_dictionary;

bool global_settings::connect_client(int socket_id, string client_name)
{
    int socket_ids = 1;   
    bool success = false;
    string terminal_output;

    bool client_name_exists = client_name_dictionary.contains(client_name);

    if (client_name_exists)
    {
        socket_ids = client_name_dictionary.get(client_name);

        if (socket_ids != 2)
        {
            socket_ids++;

            terminal_output = format("Success: connected client; Client: {}; Connection: #{}", client_name, socket_ids);

            success = true;
        }
        else
        {
            terminal_output = format("Failure: client has two connections already; Client: {}; Connection: #{}", client_name, socket_ids);
        }               
    }
    else
    {
        terminal_output = format("Success: connected client; Client: {}; Connection: #{}", client_name, socket_ids);

        success = true;
    }

    if (success == true)
    {
        client_name_dictionary.insert_or_update(client_name, socket_ids);
        socket_id_dictionary.insert_or_update(socket_id, client_name);
    }

    cout << terminal_output << endl;

    return success;      
}

bool global_settings::disconnect_client(int socket_id, string client_name)
{
    bool success = false;
    string terminal_output;

    bool client_name_exists = client_name_dictionary.contains(client_name);
    bool socket_id_exists = socket_id_dictionary.contains(socket_id);

    if (client_name_exists && socket_id_exists)
    {
        int socket_ids = client_name_dictionary.get(client_name);
        socket_ids--;
        client_name_dictionary.insert_or_update(client_name, socket_ids);

        socket_id_dictionary.remove(socket_id);

        success = true;

        terminal_output = format("Success: disconnected client; Client: {}; Socket Id: #{}", client_name, socket_id);
    }
    else
    {
        terminal_output = format("Failure: unexpected error on disconnecting client; Client: {}; Socket Id: {}", client_name, socket_id);
    }

    cout << terminal_output << endl;

    return success; 
}