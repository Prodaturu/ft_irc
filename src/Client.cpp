#include "../include/Client.hpp"

Client::Client(int fd) : fd(fd), buffer(""), nickname(""), username(""), realname(""), authenticated(false), registered(false) {}

Client::~Client() {}

int Client::getFd() const {
    return fd;
}
const string& Client::getNickname() const {
    return nickname;
}
const string& Client::getUsername() const {
    return username;
}
const string& Client::getBuffer() const {
    return buffer;
}
bool Client::isAuthenticated() const {
    return authenticated;
}
bool Client::isRegistered() const {
    return registered;
}

void Client::setNickname(const string& nickname) {
    this->nickname = nickname;
}
void Client::setUsername(const string& username) {
    this->username = username;
}
void Client::setRealname(const string& realname) {
    this->realname = realname;
}
void Client::setAuthenticated(bool auth) {
    this->authenticated = auth;
}
void Client::setRegistered(bool reg) {
    this->registered = reg;
}
void Client::appendToBuffer(const string& data) {
    this->buffer += data;
}
bool Client::hasCompleteLine() const{
    return (buffer.find("\r\n") != string::npos ||
            buffer.find("\n") != string::npos);
}
string Client::extractLine() {
    size_t pos = buffer.find("\r\n");
    if (pos != string::npos){
        string line = buffer.substr(0, pos);
        buffer.erase(0, pos + 2);
        return line;
    }
    pos = buffer.find("\n");
    if(pos != string::npos){
        string line = buffer.substr(0, pos);
        buffer.erase(0, pos + 1);
        return line;
    }
    return "";
}