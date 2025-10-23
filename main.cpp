#include <iostream>
#include <string>
#include <cstdlib>
#include <csignal>
#include "include/Server.hpp"

// Global flag for signal handling
volatile sig_atomic_t g_shutdown = 0;

// Signal handler function
void signalHandler(int signum)
{
	std::cout << "\n[SIGNAL] Received signal " << signum;
	if (signum == SIGINT)
		std::cout << " (SIGINT - Ctrl+C)";
	else if (signum == SIGTERM)
		std::cout << " (SIGTERM)";
	else if (signum == SIGQUIT)
		std::cout << " (SIGQUIT - Ctrl+\\)";
	std::cout << " - Shutting down server gracefully..." << std::endl;
	
	// Set shutdown flag (server will detect this and exit cleanly)
	g_shutdown = 1;
}

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
	
	// Create & start the server
	Server server(port,password);
	
	// Setup signal handlers
	signal(SIGINT, signalHandler);   // Ctrl+C
	signal(SIGTERM, signalHandler);  // Termination request
	signal(SIGQUIT, signalHandler);  // Ctrl+\ (quit)
	
	std::cout << "[INFO] Signal handlers installed (Ctrl+C to exit gracefully)" << std::endl;
	
	try 
	{
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
