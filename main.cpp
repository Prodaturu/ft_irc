#include <iostream>
#include <string>
#include <cstdlib>
#include "include/Server.hpp"

int main (int ac, char **av)
{
    // Argument check
    if (ac != 3)
    {
        std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl; 
        return 1;
    } 

    // Validate port number
    int port = std::atoi(av[1]);
    if (port < 1024 || port > 65535)
    {
        std::cerr << "Error: Invalid port number" << std::endl;
        return 1;
    }

    // Validate password
    std::string password = av[2];
    if (password.empty())
    {
        std::cerr << "Error: Password cannot be empty" << std::endl;
        return 1;
    }

    /*----------------Setup Server----------------*/
    
    // Create and start the server
    Server server(port,password);
    try 
    {
        // Signal handling (pending implementation)
        signal(SIGINT, Server::SignalHandler);
        signal(SIGQUIT, Server::SignalHandler);
        // [] (int signum)
        // {
        //     std::cout << "\nSignal " << signum << " received, shutting down server." << std::endl;
        //     std::exit(0);
        // }
        // Start the server
        server.start();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "Server Closed!" << std::endl;
    return 0;
}
