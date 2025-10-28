
#include "dvd.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>

int main(int argc, char *argv[]) {
    int opt;
    int speed = 1; // Valeur spéciale indiquant que l'option n'a pas été fournie
    int t = 5;
    char* source_name = nullptr;
    int brainrot = -1;

    // Analyse des options
    while ((opt = getopt(argc, argv, "S:s:t:b")) != -1) {
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

            case 'b':
                //ballerina: 0, tungtung: 1
                brainrot = source_name[0] == 't' ? 1 : 0;
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

    return dvd(obswebsocket, source_name, speed, t, brainrot);
}
