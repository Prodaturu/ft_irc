#include <iostream>
#include <string>
#include <cstdlib>
#include "include/Server.hpp"


int main (int ac, char **av)
{
    if (ac != 3) {
        std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl; 
        return 1;
    } 
    int port = std::atoi(av[1]);
    if (port <= 0 || port > 65535) {
        std::cerr << "Error: Invalid port number" << std::endl;
        return 1;
    }
    std::string password = av[2];
    if (password.empty()) {
        std::cerr << "Error: Password cannot be empty" << std::endl;
        return 1;
    }
    try {
        Server server(port,password);
        server.start();
    }
    catch (const std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}