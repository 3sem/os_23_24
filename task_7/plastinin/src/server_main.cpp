#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/server.h"

int main() {
    Server serv = {};
    server_ctor(&serv);

    fd_set rfds;
    FD_ZERO(&rfds);
    LOG1("Creating fifo\n");

    mkfifo(REGISTER_FIFO, 0666);
    serv.reg_fd = open(REGISTER_FIFO, O_RDWR | O_NONBLOCK);
    LOG1("Opened register.fifo\n");

    FD_SET(serv.reg_fd, &rfds);
    LOG1("Set rfds\n\n");

    while(!serv.off) {
        int ret = select(serv.reg_fd + 2, &rfds, NULL, NULL, NULL);

        if (ret) {
            LOG1("Got something\n");
            if (FD_ISSET(serv.reg_fd, &rfds)) {
                LOG1("Got new register request\n");
                handle_cmd(&serv);
            }
            else {
                for (int i = 0; i < serv.clients_num; i++) {
                    if (FD_ISSET(serv.clients[i].in_fd, &rfds)) {
                        LOG1("Got request from client %d\n" COMMA i);
                        handle_request(&serv, i);
                    }
                }
            }
        }

        initialize_rfds(&serv, &rfds);
    }

    server_dtor(&serv);

    return 0;
}
