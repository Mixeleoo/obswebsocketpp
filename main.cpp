#include "external/websocketpp/config/asio_no_tls_client.hpp"
#include "external/websocketpp/client.hpp"
#include <iostream>
#include <string>
#include <jsoncpp/json/json.h> // Pour manipuler les messages JSON (utilise la bibliothèque JsonCpp)

typedef websocketpp::client<websocketpp::config::asio_client> client;

void on_message(websocketpp::connection_hdl hdl, client::message_ptr msg) {
    std::cout << "Réponse du serveur : " << msg->get_payload() << std::endl;
}

int main() {
    // Configuration
    std::string uri = "ws://localhost:4455"; // URL WebSocket
    std::string password = "kwiIpQG3x8RMTfVW";

    // Créer le client WebSocket
    client c;

    try {
        c.init_asio();

        // Ajouter des handlers
        c.set_message_handler(&on_message);

        // Se connecter au serveur
        websocketpp::lib::error_code ec;
        auto con = c.get_connection(uri, ec);

        if (ec) {
            std::cerr << "Erreur de connexion : " << ec.message() << std::endl;
            return -1;
        }

        c.connect(con);

        // Lancer l'événement Asio (non bloquant)
        std::thread t([&c]() { c.run(); });

        // Envoyer un message d'authentification
        Json::Value auth;
        auth["op"] = 1; // Message d'authentification
        auth["d"]["rpcVersion"] = 1; // Version de l'API WebSocket
        auth["d"]["authentication"] = password;

        std::string message = Json::FastWriter().write(auth);
        c.send(con->get_handle(), message, websocketpp::frame::opcode::text);

        // Envoyer une commande (exemple : récupérer la liste des scènes)
        Json::Value command;
        command["op"] = 6; // Identifiant de la commande
        command["d"]["requestType"] = "GetSceneList";
        command["d"]["requestId"] = "1";

        message = Json::FastWriter().write(command);
        c.send(con->get_handle(), message, websocketpp::frame::opcode::text);

        // Attendre la réponse
        t.join();

    } catch (const std::exception& e) {
        std::cerr << "Exception capturée : " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Exception inconnue capturée !" << std::endl;
    }

    return 0;
}
