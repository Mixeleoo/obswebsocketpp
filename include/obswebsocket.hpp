
#ifndef OBSWEBSOCKET
#define OBSWEBSOCKET

#include <iostream>
#include <map>
#include <future>
#include "websocket_endpoint.hpp"

// TODO: Je pourrais rassembler toutes les requêtes qui visent une source précise 

class obswebsocket{
public:
    obswebsocket();
    void connect(const std::string_view& host, const int& port, const std::string_view& password);

    inline void show() const { std::cout << *endpoint.con << std::endl; }
    inline void close() { endpoint.close(0, websocketpp::close::status::normal); }

    /**
     * @brief Récupérer la liste des scènes
     * @return std::string
    */
    int get_scene_item_id(const std::string& scene_name, const std::string& source_name);
    std::string get_scene_list();
    Json::Value get_scene_item_transform(const std::string& scene_name, const std::string& source_name);
    bool get_scene_item_enabled(const std::string& scene_name, const std::string& source_name);

    /**
     * @brief Changer les propriétées d'une source. Champs possibles :
        'alignment', 'boundsAlignment', 'boundsHeight', 'boundsType', 'boundsWidth', 'cropBottom', 'cropLeft', 'cropRight', 'cropTop',
        'height', 'positionX', 'positionY', 'rotation', 'scaleX', 'scaleY', 'sourceHeight', 'sourceWidth', 'width'
     * @param scene_name Nom de la scène dans laquelle la source est
     * @param source_name Nom de la source
     * @param new_transform Dictionnaire json avec les différents champs et leur nouvelle valeur
     * @return void
    */
    void set_scene_item_transform(const std::string& scene_name, const std::string& source_name, const std::string& new_transform);
    inline void set_scene_item_transform(const std::string& scene_name, const std::string& source_name, const Json::Value& new_transform)
    {
        set_scene_item_transform(scene_name, source_name, Json::FastWriter().write(new_transform));
    }
    inline void set_scene_item_transform(const std::string& scene_name, const std::string& source_name, const char * const new_transform)
    {
        set_scene_item_transform(scene_name, source_name, std::string(new_transform));
    }
    void set_scene_item_enabled(const std::string& scene_name, const std::string& source_name, const bool& scene_item_enabled);

private:
    websocket_endpoint endpoint;
    static int instance_counter;
    int id;
};

#endif
