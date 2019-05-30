#include <sys/time.h>
#include <stdio.h>

int main(int argc,char *argv[]){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    printf("tv sec [%lu] , tv usec [%lu]\n",tv.tv_sec,tv.tv_usec);
    return 0;
}