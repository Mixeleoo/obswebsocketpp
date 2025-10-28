
#include "obswebsocket.hpp"
#include <chrono>
#include "exception.hpp"
#include <cmath>
#include <string.h>

using namespace std::chrono_literals;
using namespace obswebsocket;

// On initialise le compteur des instances
int obsmanager::instance_counter = 0;

static int find_item(char const * const list[], char const * const item_name){
    int i = 0;
    while(i < 32){
        if(list[i] != NULL && !strcmp(list[i], item_name)) return i;
        i++;
    }
    return -1;
}

obsmanager::obsmanager(){
    id = instance_counter;
    instance_counter += 1;

    // On met tout le cache à 0
    for(int i = 0; i < 32; i++) _itemIdCache[i] = NULL;
}

obsmanager::~obsmanager(){
    for(int i = 0; i < 32; i++) free(_itemIdCache[i]);
}

void obsmanager::connect(const std::string_view& host, const int& port, const std::string_view& password)
{
    int id = endpoint.connect("ws://" + std::string(host) + ":" + std::to_string(port), obsmanager::id); // input.substr(8)
    if (id != -1) {
        std::cout << "> Created connection with id " << id;
    } else { throw ConnectionError("Error connecting"); return; }

    std::cout << std::endl;
    std::this_thread::sleep_for(2s);
    endpoint.authenticate(std::string(password));
}

int obsmanager::get_scene_item_id(const std::string& scene_name, const std::string& source_name)
{
    //?sceneName 	String 	Name of the scene or group to search in
    //?sceneUuid 	String 	UUID of the scene or group to search in
    //sourceName 	String 	Name of the source to find 	None 	N/A
    int res = find_item(_itemIdCache, source_name.c_str());
    if(res >= 0){
        return res;
                
    } else {
        std::string msg = endpoint.call(1, "GetSceneItemId", "\"sceneName\":\"" + scene_name + "\",\"sourceName\":\"" + source_name + "\"");

        Json::Value response;
        Json::Reader reader;
        if (reader.parse(msg, response)) {
            if (response["d"].isMember("responseData")) {
                int id = std::stoi(response["d"]["responseData"]["sceneItemId"].asString());
                _itemIdCache[id] = (char*)malloc(sizeof(char) * source_name.size());
                strcpy(_itemIdCache[id], source_name.c_str());
                return id;
            } else {
                std::cout << "Y'a pas de nombre là" << std::endl;
                return -1;
            }
        } else {
            std::cout << "Erreur lors du parsage du reçu de message dans get_scene_item_id" << std::endl;
            return -1;
        }
    }
}

std::string obsmanager::get_scene_list()
{
    /*
    endpoint.send(id, "{\"d\":{\"requestId\":\"1\",\"requestType\":\"GetSceneList\"},\"op\":6}");
    std::cout << "Attente de la réponse..." << std::endl;
    std::this_thread::sleep_for(1s);
    std::cout << "Ici les scènes :\n" << Json::FastWriter().write(endpoint.con->get_message("scenes")) << std::endl;
    */
    return endpoint.call(1, "GetSceneList");
}

std::vector<std::string> obsmanager::get_scene_item_list(const std::string& scene_name)
{
    /*
    "sceneItems":[
    {"inputKind":"image_source","isGroup":null,"sceneItemBlendMode":"OBS_BLEND_NORMAL","sceneItemEnabled":true,"sceneItemId":3,"sceneItemIndex":0,
    "sceneItemLocked":false,"sceneItemTransform":{blabla...},"sourceName":"Image",
    "sourceType":"OBS_SOURCE_TYPE_INPUT","sourceUuid":"8230fca3-584b-40c0-b4ff-631cf2d0f061"}]
    */
    Json::Value response;
    Json::Reader reader;

    reader.parse(endpoint.call(4, "GetSceneItemList", "\"sceneName\":\"" + scene_name + "\""), response);
    std::vector<std::string> d;

    for(const Json::Value& item: response["d"]["responseData"]["sceneItems"]){
        d.push_back(item["sourceName"].asString());
    }
    
    return d;
}

Json::Value obsmanager::get_scene_item_transform(const std::string& scene_name, const std::string& source_name)
{
    /*
    ?sceneName 	String 	Name of the scene the item is in 	None 	Unknown
    ?sceneUuid 	String 	UUID of the scene the item is in 	None 	Unknown
    sceneItemId 	Number 	Numeric ID of the scene item 	>= 0 	N/A
    */

    Json::Value response;
    Json::Reader reader;
    int id = get_scene_item_id(scene_name, source_name);
    if(id == -1) throw InvalidSourceName(source_name + " does not exist.");

    reader.parse(endpoint.call(2, "GetSceneItemTransform", "\"sceneName\":\"" + scene_name + "\",\"sceneItemId\":" + std::to_string(id) + ""), response);
    
    return response["d"]["responseData"]["sceneItemTransform"];
}

std::pair<Point, Point> obsmanager::computeTransformedBounds(const Json::Value& param_transform)
{
    constexpr double DEG2RAD = M_PI / 180.0;

    double x = param_transform["positionX"].asDouble();
    double y = param_transform["positionY"].asDouble();
    int width = param_transform["sourceWidth"].asInt();
    int height = param_transform["sourceHeight"].asInt();

    double scaleX = param_transform["scaleX"].asDouble();
    double scaleY = param_transform["scaleY"].asDouble();
    double rotation = param_transform["rotation"].asDouble(); // en degrés

    // Taille réelle après scale
    double scaledWidth = width * scaleX;
    double scaledHeight = height * scaleY;

    // Conversion en radians
    double angle = rotation * DEG2RAD;
    double cosA = std::cos(angle);
    double sinA = std::sin(angle);

    // Décalage par rapport au point pivot (haut-gauche)
    double dx = scaledWidth;
    double dy = scaledHeight;

    // Appliquer la rotation autour de (x, y)
    double rotatedX2 = x + (dx * cosA - dy * sinA);
    double rotatedY2 = y + (dx * sinA + dy * cosA);

    return { {x, y}, {rotatedX2, rotatedY2} };
}

void obsmanager::set_text_settings(const std::string& source_name, const TextSettings& new_settings)
{
    std::string settings_str = new_settings.to_str();
    if(settings_str.empty()) return;

    endpoint.send(1, "SetInputSettings", "\"inputName\":\"" + source_name + "\",\"inputSettings\":{" + settings_str.substr(1) + "}");
}

bool obsmanager::get_scene_item_enabled(const std::string& scene_name, const std::string& source_name)
{
    /*
    ?sceneName 	String 	Name of the scene the item is in 	None 	Unknown
    ?sceneUuid 	String 	UUID of the scene the item is in 	None 	Unknown
    sceneItemId 	Number 	Numeric ID of the scene item 	>= 0 	N/A
    */

    Json::Value response;
    Json::Reader reader;
    int id = get_scene_item_id(scene_name, source_name);
    if(id == -1) throw InvalidSourceName(source_name + " does not exist.");

    reader.parse(endpoint.call(3, "GetSceneItemEnabled", "\"sceneName\":\"" + scene_name + "\",\"sceneItemId\":" + std::to_string(id) + ""), response);
    return response["d"]["responseData"]["sceneItemEnabled"].asBool();
}

void obsmanager::set_scene_item_transform(const std::string& scene_name, const std::string& source_name, const std::string& new_transform)
{
    /*
    ?sceneName 	String 	Name of the scene the item is in 	None 	Unknown
    ?sceneUuid 	String 	UUID of the scene the item is in 	None 	Unknown
    sceneItemId 	Number 	Numeric ID of the scene item 	>= 0 	N/A
    sceneItemTransform 	Object 	Object containing scene item transform info to update
    */
    int id = get_scene_item_id(scene_name, source_name);
    if(id == -1) throw InvalidSourceName(source_name + " does not exist.");

    endpoint.send(1, "SetSceneItemTransform", "\"sceneName\":\"" + scene_name + "\",\"sceneItemId\":" + std::to_string(id) + ",\"sceneItemTransform\":{" + new_transform + "}");
}

int obsmanager::get_input_audio_balance(const std::string& input_name)
{
    Json::Value response;
    Json::Reader reader;
    reader.parse(endpoint.call(3, "GetSceneItemEnabled", "\"input_name\":\"" + input_name), response);
    return response["d"]["responseData"]["inputAudioBalance"].asInt();
}

void obsmanager::set_input_audio_balance(const std::string& input_name, const int& input_audio_balance)
{

}