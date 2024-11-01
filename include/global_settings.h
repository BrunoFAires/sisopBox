#include <string>
#include <format>
#include <iostream>

#include <concurrent_dictionary.h>

class global_settings
{
    public:

        static concurrent_dictionary<string, int> client_name_dictionary;
        static concurrent_dictionary<int, string> socket_id_dictionary;

        static bool connect_client(int socket_id, string client_name);
        static bool disconnect_client(int socket_id, string client_name);
};