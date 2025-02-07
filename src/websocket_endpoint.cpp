
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <jsoncpp/json/json.h>
#include <chrono>

#include <websocketpp/base64/base64.hpp>

#include "websocket_endpoint.hpp"

std::string sha256_base64(const std::string& data) {
    EVP_MD_CTX* context = EVP_MD_CTX_new();
    const EVP_MD* sha256 = EVP_sha256();
    if (context == nullptr || sha256 == nullptr) {
        std::cerr << "Failed to create EVP_MD_CTX or get SHA256" << std::endl;
        return "";
    }

    if (EVP_DigestInit_ex(context, sha256, nullptr) != 1) {
        EVP_MD_CTX_free(context);
        std::cerr << "Failed to initialize digest" << std::endl;
        return "";
    }

    if (EVP_DigestUpdate(context, data.c_str(), data.size()) != 1) {
        EVP_MD_CTX_free(context);
        std::cerr << "Failed to update digest" << std::endl;
        return "";
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len = 0;
    if (EVP_DigestFinal_ex(context, hash, &hash_len) != 1) {
        EVP_MD_CTX_free(context);
        std::cerr << "Failed to finalize digest" << std::endl;
        return "";
    }

    EVP_MD_CTX_free(context);

    // Conversion de hash en base64 ici (tu peux utiliser une bibliothèque comme OpenSSL base64 ou d'autres)
    return websocketpp::base64_encode(hash, hash_len);
}

websocket_endpoint::websocket_endpoint(){
    m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
    m_endpoint.clear_error_channels(websocketpp::log::elevel::all);
 
    m_endpoint.init_asio();
    m_endpoint.start_perpetual();
 
    m_thread.reset(new websocketpp::lib::thread(&client::run, &m_endpoint));
}

int websocket_endpoint::connect(std::string const & uri, const int& id) {
    websocketpp::lib::error_code ec;
 
    client::connection_ptr con = m_endpoint.get_connection(uri, ec);

    if (ec) {
        std::cout << "> Connect initialization error: " << ec.message() << std::endl;
        return -1;
    }
    connection_metadata::ptr metadata_ptr(new connection_metadata(id, con->get_handle(), uri));
 
    con->set_open_handler(websocketpp::lib::bind(
        &connection_metadata::on_open,
        metadata_ptr,
        &m_endpoint,
        websocketpp::lib::placeholders::_1
    ));
    con->set_fail_handler(websocketpp::lib::bind(
        &connection_metadata::on_fail,
        metadata_ptr,
        &m_endpoint,
        websocketpp::lib::placeholders::_1
    ));
    con->set_close_handler(websocketpp::lib::bind(
        &connection_metadata::on_close,
        metadata_ptr,
        &m_endpoint,
        websocketpp::lib::placeholders::_1
    ));
    con->set_message_handler(websocketpp::lib::bind(
        &connection_metadata::on_message,
        metadata_ptr,
        websocketpp::lib::placeholders::_1,
        websocketpp::lib::placeholders::_2
    ));
    
    websocket_endpoint::con = metadata_ptr;
    m_endpoint.connect(con);
 
    return id;
}

void websocket_endpoint::close(int id, websocketpp::close::status::value code) {
    websocketpp::lib::error_code ec;

    if(con == nullptr) std::cout << "> Erreur : connexion invalide (nullptr)" << std::endl;
    else {
        m_endpoint.close(con->get_hdl(), code, "", ec);
        if (ec) {
            std::cout << "> Error initiating close: " << ec.message() << std::endl;
        }
    }
}

// Fonction pour s'authentifier à OBS WebSocket
void websocket_endpoint::authenticate(const std::string& password) {

    if(con == nullptr){
        std::cout << "> Erreur : connexion invalide (nullptr)" << std::endl;
        return;
    }

    if (con->m_messages.empty()) {
        std::cout << "> No authentication data received!" << std::endl;
        return;
    }

    // Récupérer le dernier message contenant le challenge
    Json::Value response;
    Json::Reader reader;
    if (!reader.parse(con->m_messages.back(), response)) {
        std::cout << "> Failed to parse JSON!" << std::endl;
        return;
    }

    if (!response["d"].isMember("authentication")) {
        std::cout << "> No authentication required." << std::endl;
        return;
    }

    std::string challenge = response["d"]["authentication"]["challenge"].asString();
    std::string salt = response["d"]["authentication"]["salt"].asString();

    // Hasher le mot de passe
    std::string secret = sha256_base64(password + salt);
    std::string authResponse = sha256_base64(secret + challenge);

    // Envoyer la réponse d'authentification
    Json::Value authRequest;
    authRequest["op"] = 1;
    authRequest["d"]["rpcVersion"] = 1;
    authRequest["d"]["authentication"] = authResponse;

    std::string authMessage = Json::FastWriter().write(authRequest);
    m_endpoint.send(con->get_hdl(), authMessage, websocketpp::frame::opcode::text);
    con->record_sent_message(authMessage);
}

websocket_endpoint::~websocket_endpoint() {
    m_endpoint.stop_perpetual();

    if(con == nullptr) std::cout << "> Erreur : connexion invalide (nullptr)" << std::endl;
    else if (con->get_status() == "Open") {
        std::cout << "> Closing connection " << con->get_id() << std::endl;
            
        websocketpp::lib::error_code ec;
        m_endpoint.close(con->get_hdl(), websocketpp::close::status::going_away, "", ec);
        if (ec) {
            std::cout << "> Error closing connection " << con->get_id() << ": "  
                << ec.message() << std::endl;
        }
    }

    m_thread->join();
}

std::string websocket_endpoint::call(const int& requestId, const std::string& requestType, const std::string& values) {
    auto promise = std::make_shared<std::promise<std::string>>();
    std::future<std::string> future = promise->get_future();

    // Stocke la promesse associée à ce requestId
    con->add_request(requestId, promise);

    // Envoie la requête WebSocket
    send(requestId, requestType, values);

    // Attend la réponse (bloque jusqu'à ce que `on_message()` remplisse la promesse)
    return future.get();
}

void websocket_endpoint::send(const int& requestId, const std::string& requestType, const std::string& values)
{
    using namespace std::chrono_literals;
    websocketpp::lib::error_code ec;

    std::string message = "{\"d\":{\"requestId\":\"" + std::to_string(requestId) + "\",\"requestType\":\"" + std::string(requestType) + "\"" + (values != "" ? (",\"requestData\":{" + std::string(values)) + "}" : "") + "},\"op\":6}";
    std::cout << "Envoi du message suivant :\n" << message << std::endl;
      
    m_endpoint.send(con->get_hdl(), message, websocketpp::frame::opcode::text, ec);
    if (ec) {
        std::cout << "> Error sending message: " << ec.message() << std::endl;
        return;
    }
        
    con->record_sent_message(message);
}
