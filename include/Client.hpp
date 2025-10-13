#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {

    private:
        int _fd;
        std::string _buffer;
        std::string _nickname;
        std::string _username;
        std::string _realname;
        bool _authenticated;
        bool _registered;

    public:
        Client(int fd);
        ~Client();

        //getters
        int getFd() const;
        const std::string& getNickname() const;
        const std::string& getUsername() const;
        bool isAuthenticated() const;
        bool isRegistered() const;

        //setters
        void setNickname(const std::string& nickname);
        void setUsername(const std::string& username);
        void setRealname(const std::string& realname);
        void setAuthenticated(bool auth);
        void setRegistered(bool reg);

        //buffer methods
        void appendToBuffer(const std::string& data);
        bool hasCompleteLine() const;
        std::string extractLine();
};

#endif
