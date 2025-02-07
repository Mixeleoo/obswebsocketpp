
//#include <websocketpp/common/thread.hpp>
//#include <websocketpp/common/memory.hpp>
 
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <stdexcept>
#include <chrono>

#include "obswebsocket.hpp"
 
// ws://localhost:4455

int main() {
    obswebsocket obswebsocket;
    bool done = false;
    std::string input;

    obswebsocket.connect("localhost", 4455, "test123");
 
    while (!done) {
        std::cout << "Enter Command: ";
        std::getline(std::cin, input);
 
        if (input == "quit") {
            done = true;
            
        } else if (input == "help") {
            std::cout
                << "\nCommand List:\n"
                << "show <connection id>\n"
                << "help: Display this help text\n"
                << "quit: Exit the program\n"
                << std::endl;

        } else if (input.substr(0, 2) == "sl"){
            obswebsocket.get_scene_list();

        } else if (input.substr(0, 2) == "te"){
            obswebsocket.get_scene_item_enabled("Scene", "Image");

        } else if(input.substr(0, 3) == "dvd"){
            int speed = std::stoi(input.substr(4));
            int value_X_added = speed;
            int value_Y_added = speed;

            Json::Value* param_transform = new Json::Value;
            *param_transform = obswebsocket.get_scene_item_transform("Scene", "Image");

            int x = (*param_transform)["positionX"].asInt();
            int y = (*param_transform)["positionY"].asInt();
            int width = (*param_transform)["sourceWidth"].asInt();
            int height = (*param_transform)["sourceHeight"].asInt();

            delete param_transform;
            
            while(true){
                using namespace std::chrono_literals;

                x += value_X_added;
                y += value_Y_added;

                if(x >= 1920 - width) value_X_added = -speed;
                else if(x <= 0) value_X_added = speed;

                if(y >= 1080 - height) value_Y_added = -speed;
                else if(y <= 0) value_Y_added = speed;

                std::this_thread::sleep_for(10ms);

                obswebsocket.set_scene_item_transform("Scene",  "Image", "\"positionX\":" + std::to_string(x) + ",\"positionY\":" + std::to_string(y));
            }
        } else if (input.substr(0, 2) == "ge"){
            obswebsocket.get_scene_item_transform("Scene", "Image");
        
        } else if (input.substr(0, 4) == "show"){
            obswebsocket.show();
        
        } else if (input.substr(0,5) == "close") {
            obswebsocket.close();
            
        } else {
            std::cout << "> Unrecognized Command" << std::endl;
        }
    }
 
    return 0;
}
