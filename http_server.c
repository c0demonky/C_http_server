#include <sys/socket.h>
#include <sys/stdio.h>
#include <errno.h>

int main() {
    int s = socket(PF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        perror("webserver (socket)");
        return 1;
    };
    printf ("socket created succesfully");
    return 0;
}