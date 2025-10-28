
#ifndef OBSWEBSOCKET
#define OBSWEBSOCKET

#include <iostream>
#include <map>
#include <future>
#include <optional>
#include "websocket_endpoint.hpp"
#include "scene_item_transform.hpp"
#include "text_settings.hpp"

// TODO: Je pourrais rassembler toutes les requêtes qui visent une source précise 
#include <iostream>
#include <string>

struct Point {
    double x;
    double y;
};

namespace obswebsocket {

class obsmanager {
private:
    /// Ce cache va permir d'éviter de demander à chaque fois l'id d'un item pour intéragir avec celui-ci
    char* _itemIdCache[32];
public:
    obsmanager();
    ~obsmanager();
    void connect(const std::string_view& host, const int& port, const std::string_view& password);

    inline void show() const { std::cout << *endpoint.con << std::endl; }
    inline void close() { endpoint.close(0, websocketpp::close::status::normal); }

    int get_input_audio_balance(const std::string& input_name);

    /**
     * 
     */
    void set_input_audio_balance(const std::string& input_name, const int& input_audio_balance);

    /**
     * @brief Récupère l'id d'une source d'une scène.
     * @param scene_name Nom de la scène
     * @param source_name Nom de la source
     * @return L'id de la scène si trouvée -1 sinon
     */
    int get_scene_item_id(const std::string& scene_name, const std::string& source_name);

    /**
     * @brief Récupérer la liste des scènes
     * @return std::string
    */
    std::string get_scene_list();
    Json::Value get_scene_item_transform(const std::string& scene_name, const std::string& source_name);
    std::pair<Point, Point> computeTransformedBounds(const Json::Value& param_transform);
    bool get_scene_item_enabled(const std::string& scene_name, const std::string& source_name);
    std::vector<std::string> get_scene_item_list(const std::string& scene_name);
    inline void set_text_content(const std::string& source_name, const std::string& new_text)
    {
        endpoint.send(1, "SetInputSettings", "\"inputName\":\"" + source_name + "\",\"inputSettings\":{\"text\":\"" + new_text + "\"}");
    }
    void set_text_settings(const std::string& source_name, const TextSettings& new_settings);

    /**
     * @brief Changer les propriétées d'une source via un dictionnaire. Champs possibles :
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
        std::string nt = Json::FastWriter().write(new_transform);
        set_scene_item_transform(scene_name, source_name, nt.substr(1, nt.size() - 3));
    }
    inline void set_scene_item_transform(const std::string& scene_name, const std::string& source_name, const char * const new_transform)
    {
        set_scene_item_transform(scene_name, source_name, std::string(new_transform));
    }
    /**
     * @brief Changer les propriétées d'une source.
     * Nécessite d'initialiser un SceneItemTransform pour les paramètres.
     * @param scene_name Nom de la scène
     * @param source_name Nom de la source
     * @return void
     */
    inline void set_scene_item_transform(
        const std::string& scene_name,
        const std::string& source_name,
        const SceneItemTransform& transform
    )
    {
        set_scene_item_transform(scene_name, source_name, "\"sceneName\":\"" + scene_name + "\"" + transform.to_str());
    }

    /**
     * @brief Afficher ou cacher une source d'une scène
     * @param scene_name Nom de la scène
     * @param source_name Nom de la source, une recherche de l'id se fera avec le nom de la source, sinon utiliser la méthode avec l'id de la source directement
     */
    inline void set_scene_item_enabled(const std::string& scene_name, const std::string& source_name, const bool& scene_item_enabled)
    { set_scene_item_enabled(scene_name, get_scene_item_id(scene_name, source_name), scene_item_enabled); }

    /**
     * @brief Afficher ou cacher une source d'une scène
     * @param scene_name Nom de la scène
     * @param source_name Nom de la source
     */
    inline void set_scene_item_enabled(const std::string& scene_name, const int& source_id, const bool& scene_item_enabled)
    {
        endpoint.send(1, "SetSceneItemEnabled", "\"sceneName\":\"" + scene_name + "\",\"sceneItemId\":" + std::to_string(source_id) + ",\"sceneItemEnabled\":" + (scene_item_enabled ? "true" : "false") + "");
    }

private:
    websocket_endpoint endpoint;
    static int instance_counter;
    int id;
};
}

#endif
