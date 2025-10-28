
#include "bordel.hpp"
#include <experimental/random>

using namespace std::experimental;

void bordel(obswebsocket::obsmanager& obswebsocket, const int& t, const int& interval){
    std::vector<std::string> scene_item_list = obswebsocket.get_scene_item_list("Scene");

    SceneItemTransform transform;
    std::string source_name;
    
    auto start = std::chrono::steady_clock::now();  // Temps de départ
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);

    while (elapsed.count() < t) {
        source_name = scene_item_list[randint(0UL, scene_item_list.size() - 1)];
        SceneItemTransform transform;
        
        int choice = randint(0, 3);  
        switch (choice) {
            case 0: // rotate
                transform.rotation.set(randint(0, 360));
                break;
            case 1: // scale
                transform.scaleX.set(randint(0, 2) ? randint(50, 100) / 100 : randint(100, 200) / 100);
                transform.scaleY.set(randint(0, 2) ? randint(50, 100) / 100 : randint(100, 200) / 100);
                break;
            case 2: // position
                transform.positionX.set(randint(-600, 1320));
                transform.positionY.set(randint(-600, 740));
                break;
        }
            
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        obswebsocket.set_scene_item_transform("Scene", source_name, transform);

        elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start);
    }
}