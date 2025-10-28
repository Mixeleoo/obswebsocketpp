
#include <iostream>

namespace obswebsocket {
class InvalidSourceName : public std::exception {
private:
    std::string message;
public:
    InvalidSourceName(const std::string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class ConnectionError : public std::exception {
private: std::string message;
public:
    ConnectionError(const std::string& msg) : message(msg) {}
    
    const char* what() const noexcept override {
        return message.c_str();
    }
};
}