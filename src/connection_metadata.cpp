
#include "connection_metadata.hpp"

connection_metadata::connection_metadata(int id, websocketpp::connection_hdl hdl, std::string uri)
      : m_id(id)
      , m_hdl(hdl)
      , m_status("Connecting")
      , m_uri(uri)
      , m_server("N/A")
    {}

void connection_metadata::on_open(client * c, websocketpp::connection_hdl hdl) {
    m_status = "Open";
 
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    m_server = con->get_response_header("Server");
}

void connection_metadata::on_fail(client * c, websocketpp::connection_hdl hdl) {
    m_status = "Failed";
 
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    m_server = con->get_response_header("Server");
    m_error_reason = con->get_ec().message();
}

void connection_metadata::on_close(client * c, websocketpp::connection_hdl hdl) {
    m_status = "Closed";
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    std::stringstream s;
    s << "close code: " << con->get_remote_close_code() << " (" 
    << websocketpp::close::status::get_string(con->get_remote_close_code()) 
    << "), close reason: " << con->get_remote_close_reason();
    m_error_reason = s.str();
}

// Ah ouais ca recoit les messages et touut
void connection_metadata::on_message(websocketpp::connection_hdl hdl, client::message_ptr msg)
{
    std::string message;

    if (msg->get_opcode() == websocketpp::frame::opcode::text) message = msg->get_payload();
    else message = websocketpp::utility::to_hex(msg->get_payload());
    
    m_messages.push_back(message);
    std::cout << "Message reçu : " << message << std::endl;

    // Parse JSON pour extraire le requestId
    Json::Value response;
    Json::Reader reader;
    if (reader.parse(msg->get_payload(), response) && response["d"].isMember("requestId")) {
        int requestId = std::stoi(response["d"]["requestId"].asString());
        // Vérifie si on attend une réponse pour ce requestId
        auto it = pending_requests.find(requestId);
        if (it != pending_requests.end()) {
            // Remplit la promesse avec la réponse OBS
            it->second->set_value(msg->get_payload());
            pending_requests.erase(it);  // Nettoie la requête
        }
    }
}

Json::Value connection_metadata::get_message(const std::string& field){
    std::vector<std::string>::reverse_iterator it;

    // On cherche la dernière instance du field recherché
    for(it = m_messages.rbegin(); it != m_messages.rend() && it->find(field) == std::string::npos; ++it);

    // Si y'a pas de dernière instance, ça veut dire que l'itération a atteint la fin
    if(it == m_messages.rend()){
        std::cerr << field << " non trouvé" << std::endl;
        return -1;
    }

    Json::Value jsonData;
    Json::CharReaderBuilder reader;
    std::string errs;

    std::istringstream iss(*it);
    if (!Json::parseFromStream(reader, iss, &jsonData, &errs)) {
        std::cerr << "Erreur de parsing JSON: " << errs << std::endl;
        return 1;
    }

    return jsonData["d"]["responseData"][field];
}

std::ostream & operator<< (std::ostream & out, connection_metadata const & data) {
    out << "> URI: " << data.m_uri << "\n"
        << "> Status: " << data.m_status << "\n"
        << "> Remote Server: " << (data.m_server.empty() ? "None Specified" : data.m_server) << "\n"
        << "> Error/close reason: " << (data.m_error_reason.empty() ? "N/A" : data.m_error_reason) << "\n"
        << "> Messages Processed: (" << data.m_messages.size() << ") \n";
 
    std::vector<std::string>::const_iterator it;
    for (it = data.m_messages.begin(); it != data.m_messages.end(); ++it) {
        out << *it << "\n";
    }
 
    return out;
}