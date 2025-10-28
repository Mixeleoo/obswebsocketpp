
#include "rotate.hpp"
void rotate(obswebsocket::obsmanager& obswebsocket, const int& speed, const int& t, char const* const source_name){
    auto start = std::chrono::steady_clock::now();  // Temps de départ
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start);

    Json::Value transform = obswebsocket.get_scene_item_transform("Stream", source_name);

    int rotation  = 0;
    double x      = transform["positionX"].asDouble();
    double y      = transform["positionY"].asDouble();
    double width  = transform["sourceWidth"].asDouble()  * transform["scaleX"].asDouble();
    double height = transform["sourceHeight"].asDouble() * transform["scaleY"].asDouble();

    while (elapsed.count() < t) {
        using namespace std::chrono_literals;

        // Pendant le premier quart de rotation, on augmentera x jusqu'à width
        // Pendant le deuxième quart, on augmentera y jusqu'à height
        // Pendant le troisième quart, on diminuera x jusqu'à 0
        // Pendant le quatrième quart, on diminuera y jusqu'à 0
        rotation += speed;
        if(rotation < 90){
            x += (width / (90 / speed));
        } else if(rotation < 180){
            y += (height / (90 / speed));
        } else if(rotation < 270){
            x -= (width / (90 / speed));
        } else {
            y -= (height / (90 / speed));
        }
        rotation %= 360;

        obswebsocket.set_scene_item_transform(
            "Stream",
            source_name,
            "\"rotation\":" + std::to_string(rotation) +
            ",\"positionX\":" + std::to_string(x) +
            ",\"positionY\":" + std::to_string(y)
        );

        std::this_thread::sleep_for(10ms);
        elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start);
    }
}