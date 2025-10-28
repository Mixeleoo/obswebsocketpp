
#include "calling.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
#include <SFML/Audio.hpp>

int main(int argc, char* argv[]) {
    int opt;
    int speed = 1; // Valeur spéciale indiquant que l'option n'a pas été fournie
    int t = 5;
    
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
                std::cerr << "Usage: " << argv[0] << "[-s <vitesse>] [-t <secondes>]\n";
                return EXIT_FAILURE;
        }
    }

    obswebsocket::obsmanager obswebsocket;
    obswebsocket.connect("localhost", 4455, "test123");

    calling(obswebsocket, speed, t);

    return EXIT_SUCCESS;
}
