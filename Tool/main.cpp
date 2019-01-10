#include "tool.h"
#include <assert.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>

using namespace NET;

static bool stop = false;

static void handle_term(int sig){
    stop = true;
}


int main(int argc, char const *argv[])
{
    if(isBig()){
        printf("big endian\n");
    }else{
        printf("small endian\n");
    }

    signal(SIGTERM,handle_term);

    int sock = tcp_socket();

    int ret = socketBind(sock,"192.168.0.105",12345);
    listen(sock,5);

    while(!stop){
        sleep(1);
        printf(".\n");
    }

    close(sock);
    return 0;
}