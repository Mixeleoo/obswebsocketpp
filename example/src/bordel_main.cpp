
#include "obswebsocket.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
#include "bordel.hpp"

int main(int argc, char *argv[]) {
    int opt;
    int interval = 10; // Valeur spéciale indiquant que l'option n'a pas été fournie
    int t = 5;

    // Analyse des options
    while ((opt = getopt(argc, argv, "i:t:")) != -1) {
        switch (opt) {
            case 'i':
                interval = atoi(optarg); // Convertit en entier
                break;

            case 't':
                t = atoi(optarg);
                break;

            case '?':
                std::cerr << "Usage: " << argv[0] << " [-i <interval (ms)>] [-t <secondes>]\n";
                return EXIT_FAILURE;
        }
    }
    
    std::cout << "Intervalle : " << interval << std::endl;

    obswebsocket::obsmanager obswebsocket;
    obswebsocket.connect("localhost", 4455, "test123");

    bordel(obswebsocket, t, interval);

    return EXIT_SUCCESS;
}
