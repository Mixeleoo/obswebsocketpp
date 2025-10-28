
#ifndef CONNECTION_METADATA
#define CONNECTION_METADATA

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <iostream>
#include <jsoncpp/json/json.h>

typedef websocketpp::client<websocketpp::config::asio_client> client;

class connection_metadata {
public:
    typedef websocketpp::lib::shared_ptr<connection_metadata> ptr;
    std::vector<std::string> m_messages;
 
    connection_metadata(int, websocketpp::connection_hdl, std::string);
    void on_open(client *, websocketpp::connection_hdl);
    void on_fail(client *, websocketpp::connection_hdl);
    void on_close(client *, websocketpp::connection_hdl);

    // Ah ouais ca recoit les messages et touut
    void on_message(websocketpp::connection_hdl, client::message_ptr);
    inline void add_request(const int& requestId, std::shared_ptr<std::promise<std::string>>& promise) { pending_requests[requestId] = std::move(promise); }

    inline void record_sent_message(std::string message) { m_messages.push_back(">> " + message); }
    std::string get_status() const { return m_status; }
    int get_id() const { return m_id; }

    friend std::ostream & operator<< (std::ostream & out, connection_metadata const & data);

    websocketpp::connection_hdl get_hdl(){
        return m_hdl;
    }

private:
    int m_id;
    websocketpp::connection_hdl m_hdl;
    std::string m_status;
    std::string m_uri;
    std::string m_server;
    std::string m_error_reason;

    std::map<int, std::shared_ptr<std::promise<std::string>>> pending_requests;
};

#endif
