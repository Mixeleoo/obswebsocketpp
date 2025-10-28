#include <linux/uinput.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "press_key_input.hpp"

bool press_key_uinput(int keycode) {
    // Ouvre le device uinput
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("open /dev/uinput");
        return false;
    }

    // Active les événements clavier
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, keycode);

    // Configure le périphérique virtuel
    struct uinput_setup usetup{};
    strcpy(usetup.name, "Virtual Keyboard");
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor  = 0x1234;
    usetup.id.product = 0x5678;
    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);

    // Petite pause pour s'assurer que le device est prêt
    usleep(100000);

    // Fonction d’envoi d’événement
    auto emit = [&](int type, int code, int val) {
        struct input_event ie{};
        ie.type = type;
        ie.code = code;
        ie.value = val;
        ie.time.tv_sec = 0;
        ie.time.tv_usec = 0;
        write(fd, &ie, sizeof(ie));
    };

    // Appui
    emit(EV_KEY, keycode, 1);
    emit(EV_SYN, SYN_REPORT, 0);
    usleep(20000);

    // Relâchement
    emit(EV_KEY, keycode, 0);
    emit(EV_SYN, SYN_REPORT, 0);

    // Pause avant de supprimer le device
    usleep(50000);
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);

    return true;
}
