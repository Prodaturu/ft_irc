#include "../include/Client.hpp"

Client::Client(int fd) : _fd(fd), _authenticated(false), _registered(false) {}

Client::~Client() {}

int Client::getFd() const {
    return _fd;
}
const std::string& Client::getNickname() const {
    return _nickname;
}
const std::string& Client::getUsername() const {
    return _username;
}
const std::string& Client::getBuffer() const {
    return _buffer;
}
bool Client::isAuthenticated() const {
    return _authenticated;
}
bool Client::isRegistered() const {
    return _registered;
}

void Client::setNickname(const std::string& nickname) {
     _nickname = nickname;
}
void Client::setUsername(const std::string& username) {
    _username = username;
}
void Client::setRealname(const std::string& realname) {
    _realname = realname;
}
void Client::setAuthenticated(bool auth) {
    _authenticated = auth;
}
void Client::setRegistered(bool reg) {
    _registered = reg;
}
void Client::appendToBuffer(const std::string& data) {
    _buffer += data;
}
bool Client::hasCompleteLine() const{
    return (_buffer.find("\r\n") != std::string::npos ||
            _buffer.find("\n") != std::string::npos);
}
std::string Client::extractLine() {
    size_t pos = _buffer.find("\r\n");
    if (pos != std::string::npos){
        std::string line = _buffer.substr(0, pos);
        _buffer.erase(0, pos + 2);
        return line;
    }
    pos = _buffer.find("\n");
    if(pos != std::string::npos){
        std::string line = _buffer.substr(0, pos);
        _buffer.erase(0, pos + 1);
        return line;
    }
    return "";
}