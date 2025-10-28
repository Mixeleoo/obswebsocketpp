
#include "calling.hpp"
void calling(obswebsocket::obsmanager& obswebsocket, const int& speed, const int& t){
    Json::Value transform = obswebsocket.get_scene_item_transform("Stream", "johnpork");

    auto start = std::chrono::steady_clock::now();  // Temps de départ
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start);

    int x         = transform["positionX"].asInt();
    int width     = transform["sourceWidth"].asInt();
    double scaleX = transform["scaleX"].asDouble();

    while (elapsed.count() < t) {

        using namespace std::chrono_literals;

        x -= speed;
        if(x < -(width * scaleX)) x = 1920;

        obswebsocket.set_scene_item_transform(
            "Stream",
            "johnpork",
            "\"positionX\":" + std::to_string(x)
        );

        std::this_thread::sleep_for(10ms);
        elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start);
    }
}