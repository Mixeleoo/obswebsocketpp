
#include "rotate.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>

int main(int argc, char *argv[]) {
    int opt;
    int speed = 1; // Valeur spéciale indiquant que l'option n'a pas été fournie
    int t = 5;
    char* source_name = nullptr;

    if(argc > 1){
        source_name = argv[1];
    } else {
        std::cerr << "Usage: " << argv[0] << "<source_name> [-s <vitesse>] [-t <secondes>]\n";
        return EXIT_FAILURE;
    }
    
    // Analyse des options
    while ((opt = getopt(argc, argv, "s:t:")) != -1) {
        switch (opt) {
            case 's':
                speed = atoi(optarg); // Convertit en entier
                break;

            case 't':
                t = atoi(optarg);
                break;

            case '?':
                std::cerr << "Usage: " << argv[0] << "<source_name> [-s <vitesse>] [-t <secondes>]\n";
                return EXIT_FAILURE;
        }
    }

    if(source_name == nullptr){
        std::cerr << "Nécessite une source" << std::endl;
        exit(EXIT_FAILURE);
    }

    obswebsocket::obsmanager obswebsocket;
    obswebsocket.connect("localhost", 4455, "test123");
    
    rotate(obswebsocket, speed, t, source_name);

    return EXIT_SUCCESS;
}
