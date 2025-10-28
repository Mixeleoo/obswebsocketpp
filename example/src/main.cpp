
#include "obswebsocket.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <experimental/random>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <sys/wait.h>
#include "calling.hpp"
#include "rotate.hpp"
#include "bordel.hpp"
#include "dvd.hpp"
#include <linux/input-event-codes.h>
#include "press_key_input.hpp"

using namespace std::experimental;

/*
int main(int argc, char *argv[]) {
    int opt;
    int speed = 1; // Valeur spéciale indiquant que l'option n'a pas été fournie
    int t = 5;
    char * source_name = nullptr;

    // Analyse des options
    while ((opt = getopt(argc, argv, "S:s:t:")) != -1) {
        switch (opt) {
            case 'S':
                source_name = optarg;
                break;

            case 's':
                speed = atoi(optarg); // Convertit en entier
                break;

            case 't':
                t = atoi(optarg);
                break;

            case '?':
                std::cerr << "Usage: " << argv[0] << " [-s <vitesse>] [-t <secondes>] [-S >source_name>]\n";
                return EXIT_FAILURE;
        }
    }

    if(source_name == nullptr){
        std::cerr << "Nécessite une source" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    std::cout << "Vitesse : " << speed << std::endl;

    obswebsocket::obsmanager obswebsocket;

    obswebsocket.connect("localhost", 4455, "test123");

    int value_X_added = speed;
    int value_Y_added = speed;

    Json::Value* param_transform = new Json::Value;
    *param_transform = obswebsocket.get_scene_item_transform("Scene", source_name);

    int x = (*param_transform)["positionX"].asInt();
    int y = (*param_transform)["positionY"].asInt();
    int width = (*param_transform)["sourceWidth"].asInt();
    int height = (*param_transform)["sourceHeight"].asInt();

    delete param_transform;
    
    auto start = std::chrono::steady_clock::now();  // Temps de départ
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);

    while (elapsed.count() < t) {
        now = std::chrono::steady_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);

        using namespace std::chrono_literals;

        std::this_thread::sleep_for(10ms);

        obswebsocket.set_scene_item_transform("Scene", source_name, "\"positionX\":" + std::to_string(x) + ",\"positionY\":" + std::to_string(y));
    }

    return EXIT_SUCCESS;
}
*/

#define MALUST 4
#define MALUSTEMPS 5

int main() {
    obswebsocket::obsmanager obswebsocket;
    obswebsocket.connect("localhost", 4455, "test123");
    char const* maluss[] = {
        // "Joues avec tes pieds\\nmon cochon !",
        "Tu dois appeler John Pork",
        // "100ms de lag",
        // "Commandes inversées",
        "Le jeu tourne",
        "Des touches sont\\nappuyées aléatoirement",
        "Le jeu est un dvd logo",
        "Petit écran"
    };

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("trucs/peghit_low.ogg"))
        return -1;

    sf::Sound sound;
    sound.setBuffer(buffer);
    std::string new_text;
    std::string temp_text;

    TextSettings t;
    t.position.set(960.f, 950.f);

    for (int delay = 50; delay <= randint(600, 900); delay *= 1.2) {
        temp_text = maluss[std::experimental::randint(0, MALUST)];
        while(new_text == temp_text) temp_text = maluss[std::experimental::randint(0, MALUST)];
        new_text = temp_text;
        t.set_content(new_text);
        obswebsocket.set_text_settings("Texte", t);
        sound.setPitch(1.f + delay / 800.f);
        sound.play();
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }

    // Dernière attente 1s puis musique finale
    std::this_thread::sleep_for(std::chrono::milliseconds(900));

    temp_text = maluss[std::experimental::randint(0, MALUST)];
    while(new_text == temp_text) temp_text = maluss[std::experimental::randint(0, MALUST)];
    new_text = temp_text;
    t.set_content(new_text);

    obswebsocket.set_text_settings("Texte", t);

    if (!buffer.loadFromFile("trucs/explode.ogg"))
        return -1;

    sound.stBuffer(buffer);
    sound.setPitch(1.f);
    sound.play();

    std::chrono::_V2::steady_clock::time_point start;
    std::chrono::seconds elapsed;

    switch(new_text[0]){
        // Le jeu tourne
        case 'L':
        switch(new_text[7]){
            case 't':
                rotate(obswebsocket, 1, MALUSTEMPS, "écran");
                obswebsocket.set_scene_item_transform("Stream", "écran", "\"positionX\":0,\"positionY\":0,\"rotation\":0,\"scaleX\":" + std::to_string(1618.0 / 1920) + ",\"scaleY\":" + std::to_string(911.0 / 1080));
                break;
            case 'e':
                dvd(obswebsocket, "écran", 1, MALUSTEMPS);
                break;
        }
            break;
        
        // John Pork
        case 'T':
            // Fin du son de l'explosion
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));

            if (!buffer.loadFromFile("trucs/iphone_original.mp3"))
                return -1;

            sound.setBuffer(buffer);
            sound.play();

            obswebsocket.set_scene_item_enabled("Stream", "johnpork", true);
            calling(obswebsocket, 2, MALUSTEMPS);
            obswebsocket.set_scene_item_enabled("Stream", "johnpork", false);
            break;

        // Touches appuyées aléatoirement
        case 'D':
            start = std::chrono::steady_clock::now();  // Temps de départ
            elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start);

            while (elapsed.count() < MALUSTEMPS) {
                press_key_uinput(((int[]){KEY_W, KEY_A, KEY_S, KEY_D})[randint(0, 3)]);

                std::this_thread::sleep_for(std::chrono::milliseconds(randint(200, 2000)));
                elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start);
            }
            break;
        case 'P':
            obswebsocket.set_scene_item_transform("Stream", "écran", "\"positionX\":960,\"positionY\":540,\"scaleX\":0.3,\"scaleY\":0.3");
            std::this_thread::sleep_for(std::chrono::seconds(MALUSTEMPS));
            obswebsocket.set_scene_item_transform("Stream", "écran", "\"positionX\":0,\"positionY\":0,\"rotation\":0,\"scaleX\":" + std::to_string(1618.0 / 1920) + ",\"scaleY\":" + std::to_string(911.0 / 1080));
            break;

        default:
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}

// bbbbqccq
