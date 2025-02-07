
#include "obswebsocket.hpp"
#include <chrono>

using namespace std::chrono_literals;

// On initialise le compteur des instances
int obswebsocket::instance_counter = 0;

obswebsocket::obswebsocket()
{
    id = instance_counter;
    instance_counter += 1;
}

void obswebsocket::connect(const std::string_view& host, const int& port, const std::string_view& password)
{

    int id = endpoint.connect("ws://" + std::string(host) + ":" + std::to_string(port), obswebsocket::id); // input.substr(8)
    if (id != -1) {
        std::cout << "> Created connection with id " << id;
    } else {
        std::cerr << "Error connecting";
    }
    std::cout << std::endl;
    std::this_thread::sleep_for(1s);
    endpoint.authenticate(std::string(password));
}

int obswebsocket::get_scene_item_id(const std::string& scene_name, const std::string& source_name)
{
    //?sceneName 	String 	Name of the scene or group to search in
    //?sceneUuid 	String 	UUID of the scene or group to search in
    //sourceName 	String 	Name of the source to find 	None 	N/A
    std::string msg = endpoint.call(1, "GetSceneItemId", "\"sceneName\":\"" + scene_name + "\",\"sourceName\":\"" + source_name + "\"");

    Json::Value response;
    Json::Reader reader;
    if (reader.parse(msg, response)) {
        if (response["d"].isMember("responseData")) {
            return std::stoi(response["d"]["responseData"]["sceneItemId"].asString());
        } else {
            std::cout << "Y'a pas de nombre là" << std::endl;
            return -1;
        }
    } else {
        std::cout << "Erreur lors du parsage du reçu de message dans get_scene_item_id" << std::endl;
        return -1;
    }
}

std::string obswebsocket::get_scene_list()
{
    /*
    endpoint.send(id, "{\"d\":{\"requestId\":\"1\",\"requestType\":\"GetSceneList\"},\"op\":6}");
    std::cout << "Attente de la réponse..." << std::endl;
    std::this_thread::sleep_for(1s);
    std::cout << "Ici les scènes :\n" << Json::FastWriter().write(endpoint.con->get_message("scenes")) << std::endl;
    */
    return endpoint.call(1, "GetSceneList");
}

Json::Value obswebsocket::get_scene_item_transform(const std::string& scene_name, const std::string& source_name)
{
    /*
    ?sceneName 	String 	Name of the scene the item is in 	None 	Unknown
    ?sceneUuid 	String 	UUID of the scene the item is in 	None 	Unknown
    sceneItemId 	Number 	Numeric ID of the scene item 	>= 0 	N/A
    */

    Json::Value response;
    Json::Reader reader;
    reader.parse(endpoint.call(2, "GetSceneItemTransform", "\"sceneName\":\"" + scene_name + "\",\"sceneItemId\":" + std::to_string(get_scene_item_id(scene_name, source_name)) + ""), response);
    
    return response["d"]["responseData"]["sceneItemTransform"];
}

bool obswebsocket::get_scene_item_enabled(const std::string& scene_name, const std::string& source_name)
{
    /*
    ?sceneName 	String 	Name of the scene the item is in 	None 	Unknown
    ?sceneUuid 	String 	UUID of the scene the item is in 	None 	Unknown
    sceneItemId 	Number 	Numeric ID of the scene item 	>= 0 	N/A
    */

    Json::Value response;
    Json::Reader reader;

    reader.parse(endpoint.call(3, "GetSceneItemEnabled", "\"sceneName\":\"" + scene_name + "\",\"sceneItemId\":" + std::to_string(get_scene_item_id(scene_name, source_name)) + ""), response);
    return response["d"]["responseData"]["sceneItemEnabled"].asBool();
}

void obswebsocket::set_scene_item_transform(const std::string& scene_name, const std::string& source_name, const std::string& new_transform)
{
    /*
    ?sceneName 	String 	Name of the scene the item is in 	None 	Unknown
    ?sceneUuid 	String 	UUID of the scene the item is in 	None 	Unknown
    sceneItemId 	Number 	Numeric ID of the scene item 	>= 0 	N/A
    sceneItemTransform 	Object 	Object containing scene item transform info to update
    */
    endpoint.send(1, "SetSceneItemTransform", "\"sceneName\":\"" + scene_name + "\",\"sceneItemId\":" + std::to_string(get_scene_item_id(scene_name, source_name)) + ",\"sceneItemTransform\":{" + new_transform + "}");
}

void obswebsocket::set_scene_item_enabled(const std::string& scene_name, const std::string& source_name, const bool& scene_item_enabled)
{
    /*
    ?sceneName 	String 	Name of the scene the item is in 	None 	Unknown
    ?sceneUuid 	String 	UUID of the scene the item is in 	None 	Unknown
    sceneItemId 	Number 	Numeric ID of the scene item 	>= 0 	N/A
    sceneItemEnabled 	Boolean 	New enable state of the scene item 	None 	N/A
    */
    endpoint.send(1, "SetSceneItemEnabled", "\"sceneName\":\"" + scene_name + "\",\"sceneItemId\":" + std::to_string(get_scene_item_id(scene_name, source_name)) + ",\"sceneItemEnabled\":{" + (scene_item_enabled ? "true" : "false") + "}");
}
