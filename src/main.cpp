


#include <sstream>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <poll.h>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <unistd.h> 
#include <limits.h> 
#include <algorithm> 
#include <string>
#include <sstream>
#include <vector>

// struct pollfd fds[1024];

/**
 * @file main.cpp
 * @brief Simple server implementation using sockets and the `poll` system call.
 *
 * creates a server that listens for incoming client connections on port 6662.
 * It uses the `poll` system call to handle multiple client connections concurrently.
 * The server sends a welcome message to new clients and responds to specific messages.
 *
 * @details
 * - The server uses the `socket`, `bind`, and `listen` system calls to set up the server socket.
 * - The `poll` system call is used to monitor multiple file descriptors for events.
 * - When a new client connects, the server accepts the connection, sets the client socket to non-blocking mode,
 *   and adds it to the list of monitored file descriptors.
 * - The server responds to incoming messages from clients. For example, if a client sends "hello\n",
 *   the server responds with "dont say hello, say salam alaikum\r\n".
 *
 * @note
 * - server runs indefinitely in a loop until manually terminated.
 * - server does not handle advanced error recovery or cleanup for simplicity.
 * - Assumes max of 1024 file descriptors for the `poll` array.
 */
int main()
{
    int client_fd;
	int nfds = 0;
	int server_fd;
	struct pollfd fds[1024];

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
    {
        std::cerr << "Error creating socket\n";
        return 1;
    }

	sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(6662);

	if (bind(server_fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "Error binding socket\n";
        return 1;
    }

	if (listen(server_fd, 5) < 0)
    {
        std::cerr << "Error listening on socket\n";
        return 1;
    }
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    nfds++;
    while (true)
    {
        int poll_count = poll(fds, nfds, -1);
        if (poll_count < 0)
        {
            std::cerr << "Poll error\n";
            return 0;
        }
        /* code */
        for (int i = 0; i < nfds; ++i)
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == server_fd)
                {
                    client_fd = accept(server_fd, nullptr, nullptr);
                    if (client_fd < 0)
                    {
                        std::cerr << "Error accepting connection\n";
                        continue;
                    }

                    fcntl(client_fd, F_SETFL, O_NONBLOCK);
                    fds[nfds].fd = client_fd;
                    fds[nfds].events = POLLIN;
                    nfds++;
                    //creatUser(clientfd);
                    std::string welcome_msg = "hello new client \r\n";
                    send(client_fd, welcome_msg.c_str(), strlen(welcome_msg.c_str()), 0);
                }
                else
                {
                    client_fd = fds[i].fd;
                    char buffer[1024];
                    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
                    buffer[bytes_received] = '\0';
                    
                    std::cout << buffer << std::endl;
                    if (strcmp(buffer,"hello\n") == 0 )
                    {
                        std::string welcome_msg = "dont say hello, say wassup!\r\n";
                        send(client_fd, welcome_msg.c_str(), strlen(welcome_msg.c_str()), 0);
                        
                    }
                    
                }

            }
        }
    




    }

    // std::cout << "Server listening\n";

	return 0;
}