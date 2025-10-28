
#ifndef WEBSOCKET_ENDPOINT
#define WEBSOCKET_ENDPOINT

#include <iostream>
#include <jsoncpp/json/json.h>
#include "connection_metadata.hpp"

typedef websocketpp::client<websocketpp::config::asio_client> client;

class websocket_endpoint {
public:
    connection_metadata::ptr con;
    
    websocket_endpoint();
    ~websocket_endpoint();

    int connect(std::string const & uri, const int& id);
    void close(const int& id, const websocketpp::close::status::value& code);

    // Fonction pour s'authentifier à OBS WebSocket
    void authenticate(const std::string& password);

    std::string call(const int& requestId, const std::string& requestType, const std::string& values = "");

    void send(const int& requestId, const std::string& requestType, const std::string& values = "");
    //inline void send(const int& requestId, const char * const message) { send(std::string(message)); }
    //inline void send(const Json::Value& command) { send(Json::FastWriter().write(command)); }

private:
    client m_endpoint;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
};

#endif