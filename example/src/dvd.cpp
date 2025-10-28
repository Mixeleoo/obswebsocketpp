
#include "dvd.hpp"
#include <SFML/Audio.hpp>

int dvd(obswebsocket::obsmanager& obswebsocket, const char* source_name, const int& speed, const int& t, const int& brainrot){
    int value_X_added = speed;
    int value_Y_added = speed;

    Json::Value param_transform = obswebsocket.get_scene_item_transform("Stream", source_name);

    auto p = obswebsocket.computeTransformedBounds(param_transform);

    int x = p.first.x;
    int y = p.first.y;
    int width = p.second.x - p.first.x;
    int height = p.second.y - p.first.y;

    auto start = std::chrono::steady_clock::now();  // Temps de départ
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);

    sf::SoundBuffer buffers[2];
    if (!buffers[0].loadFromFile("trucs/ballerina.mp3"))
        return -1;

    if (!buffers[1].loadFromFile("trucs/tungtung.mp3"))
        return -1;

    sf::Sound sounds[2];
    sounds[0].setBuffer(buffers[0]);
    sounds[1].setBuffer(buffers[1]);

    while (elapsed.count() < t) {
        now = std::chrono::steady_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);

        using namespace std::chrono_literals;

        x += value_X_added;
        y += value_Y_added;

        if(x >= 1920 - width){
            value_X_added = -speed;
            if(brainrot != -1){
                sounds[brainrot].play();
            }

        } else if(x <= 0){
            value_X_added = speed;
            if(brainrot != -1){
                sounds[brainrot].play();
            }
        }

        if(y >= 1080 - height){
            value_Y_added = -speed;
            if(brainrot != -1){
                sounds[brainrot].play();
            }

        } else if(y <= 0){
            value_Y_added = speed;
            if(brainrot != -1){
                sounds[brainrot].play();
            }
        }

        std::this_thread::sleep_for(10ms);

        obswebsocket.set_scene_item_transform("Stream", source_name, "\"positionX\":" + std::to_string(x) + ",\"positionY\":" + std::to_string(y));
    }

    return EXIT_SUCCESS;
}
