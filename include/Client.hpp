#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

typedef std::string string;

class Client {

    private:
        int fd;
        string buffer;
        string nickname;
        string username;
        string realname;
        bool authenticated;
        bool registered;

    public:
        Client(int fd);
        ~Client();

        //getters
        int getFd() const;
        const std::string& getNickname() const;
        const std::string& getUsername() const;
        const std::string& getBuffer() const;
        bool isAuthenticated() const;
        bool isRegistered() const;

        //setters
        void setNickname(const std::string& nickname);
        void setUsername(const std::string& username);
        void setRealname(const std::string& realname);
        void setAuthenticated(bool auth);
        void setRegistered(bool reg);

        //authentication
        bool authenticator(std::string line, Client* client, std::string password, int client_fd) const;
        bool checkNickname(std::vector<std::string> tokens, int client_fd) const;
        bool checkUsername(std::vector<std::string> tokens, int client_fd) const;
        bool checkPassword(std::vector<std::string> tokens, Client* client, std::string password) const;

        //buffer methods
        void appendToBuffer(const std::string& data);
        bool hasCompleteLine() const;
        std::string extractLine();
};

#endif
