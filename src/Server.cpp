#include "../include/Server.hpp"
#include "../include/Channel.hpp"
#include <errno.h>
#include <stdexcept>

Server::Server(int port, const std::string& password): _server_fd(-1), _port(port), _password(password)
{
	setupSocket();
}

Server::~Server()
{
	for (size_t i = 0; i < _clients.size(); i++)
		delete _clients[i];
	for (size_t i = 0; i < _channels.size(); i++)
		delete _channels[i];
	if (_server_fd != -1)
		close(_server_fd);
}

void Server::setupSocket()
{
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd < 0)
		throw std::runtime_error("Error creating socket");

	int opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		close(_server_fd);
		throw std::runtime_error("Failed to set socket options");
	}

	struct sockaddr_in server_addr = {};
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(_port); 

	if (bind(_server_fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		close(_server_fd);
		throw std::runtime_error("Error binding socket");
	}

	if (listen(_server_fd, 5) < 0)
	{
		close(_server_fd);
		throw std::runtime_error("Error listening on socket");
	}
	
	std::cout << "Server listening on port " << _port << std::endl;
}


/**e
 * @brief Start server event loop and process incoming connections & client events.
 *
 * @details
 * Initializes a pollfd for listening server socket and
 * appends it to the_poll_fds vector, then enters a blocking, infinite loop
 * that dispatches events returned by poll().
 *
 * Behavior on each poll wake-up:
 *  - If the listening socket (stored at _poll_fds[0]) has POLLIN set, a new
 *    connection is accepted via acceptNewClient().
 *  - For each client socket (indices >= 1) the revents field is inspected:
 *      - POLLERR or POLLNVAL: logs an error and removes the client via removeClient().
 *      - POLLHUP: if POLLIN is also present, handleClientData(fd) is called to
 *        consume pending data; then logs the hangup and removes the client.
 *      - POLLIN: calls handleClientData(fd) to read/process incoming client data.
 *
 * The loop ignores interrupted system calls (EINTR) and continues polling.
 * If poll() returns an error for any other reason, the function throws
 * std::runtime_error("Poll error").
 *
 * @pre _server_fd must be a valid, bound, and listening socket.
 * @post The server enters a continuous event loop servicing connections;
 *       under normal operation this function does not return.
 *
 * @note
 *  - This method mutates the member _poll_fds and relies on helpers:
 *    acceptNewClient(), handleClientData(int), and removeClient(int).
 *  - When removeClient() shrinks _poll_fds, the loop index is adjusted (decremented)
 *    to avoid skipping subsequent entries.
 *  - Diagnostic messages are printed to std::cout.
 *
 * @throws std::runtime_error if poll() fails for reasons other than EINTR.
 *
 * @see acceptNewClient(), handleClientData(int), removeClient(int)
 */

void Server::start()
{
	struct pollfd       server_poll;

	server_poll.fd = _server_fd; // Server socket
	server_poll.events = POLLIN; // Ready to accept new connections
	server_poll.revents = 0;// Initialize revents to 0
	_poll_fds.push_back(server_poll);
	
	std::cout << "Server started successfully!" << std::endl;

	// infinite event loop so the server runs continuously
	while ("martin is bulgarian and a beach. Run this server")
	{
		// store the poll results indicating which fds are ready
		int poll_count = poll(&_poll_fds[0], _poll_fds.size(), -1);
		if (poll_count < 0)
		{
			if(errno == EINTR)
				continue;
			throw std::runtime_error("Poll error");
		}
		// Check server socket for new connections
		if (_poll_fds[0].revents & POLLIN)
			acceptNewClient();

		// Check client sockets for data  
		for (size_t i = 1; i < _poll_fds.size(); i++)
		{
			int revents = _poll_fds[i].revents;
			//checking for errors or hangup
			if (revents & (POLLERR | POLLNVAL))
			{
				std::cout << "[ERROR] Poll error on FD=" << _poll_fds[i].fd << std::endl;
				removeClient(_poll_fds[i].fd);
				i--;
				continue;
			}
			if (revents & POLLHUP)
			{
				if (revents & POLLIN)
					handleClientData(_poll_fds[i].fd);
				std::cout << "[HANGUP] Client hung up: FD=" << _poll_fds[i].fd << std::endl;
				removeClient(_poll_fds[i].fd);
				i--;
				continue;
			}
			if (revents & POLLIN)
				handleClientData(_poll_fds[i].fd);
		}
	}
}

void Server::acceptNewClient()
{
	int client_fd = accept(_server_fd, NULL, NULL);
	if (client_fd < 0)
	{
		std::cerr << "Error accepting connection" << std::endl;
		return;
	}

	// Set non-blocking
	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "Error setting non-blocking mode" << std::endl;
		close(client_fd);
		return ;
	}

	Client* new_client = new Client(client_fd);
	_clients.push_back(new_client);

	// Add to poll
	struct pollfd client_poll;
	client_poll.fd = client_fd;
	client_poll.events = POLLIN;
	client_poll.revents = 0;
	_poll_fds.push_back(client_poll);
	
	std::cout << "[CONNECT] New client connected: FD=" << client_fd << std::endl;
	
	// send welcome message
	std::string welcome_msg = "Welcome to IRC Server!\r\n";
	send(client_fd, welcome_msg.c_str(), welcome_msg.length(), 0);
}

void Server::handleClientData(int client_fd)
{
	char buffer[512];
	ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	
	if (bytes_received < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;

		std::cerr << "[ERROR] recv() failed for client " << client_fd << std::endl;
		removeClient(client_fd);
		return ;
	}

	if (bytes_received == 0)
	{
		std::cout << "[DISCONNECT] Client " << client_fd << "closed connection" << std::endl;
		removeClient(client_fd);
		return ;
	}
	
	buffer[bytes_received] = '\0';
	Client* client = getClientByFd(client_fd);
	if (!client)
		return;
	client->appendToBuffer(std::string(buffer, bytes_received));
	while (client->hasCompleteLine()) {
		std::string line = client->extractLine();
		if (line.empty())
			continue;
		
		std::cout << "[RECV] FD=" << client_fd << ": \"" << line << "\"" << std::endl;
		
		// Process command through authenticator
		authenticator(line, client, client_fd);
		
		// Check if client just completed registration
		if (client->isAuthenticated() && !client->getNickname().empty() && !client->getUsername().empty())
		{
			// Send welcome messages if not yet registered
			if (!client->isRegistered())
			{
				client->setRegistered(true);
				std::string nick = client->getNickname();
				
				// 001 RPL_WELCOME
				std::string welcome = ":localhost 001 " + nick + " :Welcome to the IRC Network " + nick + "!~" + client->getUsername() + "@localhost\r\n";
				send(client_fd, welcome.c_str(), welcome.length(), 0);
				
				// 002 RPL_YOURHOST
				std::string yourhost = ":localhost 002 " + nick + " :Your host is localhost, running version 1.0\r\n";
				send(client_fd, yourhost.c_str(), yourhost.length(), 0);
				
				// 003 RPL_CREATED
				std::string created = ":localhost 003 " + nick + " :This server was created sometime\r\n";
				send(client_fd, created.c_str(), created.length(), 0);
				
				// 004 RPL_MYINFO
				std::string myinfo = ":localhost 004 " + nick + " localhost 1.0 o o\r\n";
				send(client_fd, myinfo.c_str(), myinfo.length(), 0);
				
				std::cout << "[REGISTERED] " << nick << " successfully registered" << std::endl;
			}
			
			// Execute other commands after registration
			execCommand(line, client);
		}
	}
}

void Server::removeClient(int client_fd)
{
	std::cout << "[DISCONNECT] Removing client: FD=" << client_fd << std::endl;

	// Remove from clients vector
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i]->getFd() == client_fd)
		{
			delete _clients[i];
			_clients.erase(_clients.begin() + i);
			break;
		}
	}

	// Remove from poll_fds vector
	for (size_t i = 0; i < _poll_fds.size(); i++)
	{
		if (_poll_fds[i].fd == client_fd)
		{
			_poll_fds.erase(_poll_fds.begin() + i);
			break;
		}
	}

	close(client_fd);
}

Client* Server::getClientByFd(int client_fd)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i]->getFd() == client_fd)
			return _clients[i];
	}
	return NULL;
}

const std::string& Server::getPassword() const
{
	return _password;
}

