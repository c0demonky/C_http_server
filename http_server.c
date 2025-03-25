#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <errno.h>

// #include "netstructs.h"

#define PORT "30013"
#define BACKLOG 10
#define MAX_BUFFER 1024

void sigchld_handler(int s) {
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

void *get_addr_in(struct sockaddr* sa) {
    if (strcomp(sa->sa_family, "AF_INET") == 1) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    } else {
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }
}

int main(int argc, char* argv[]) {
    int sockfd, newfd;
    struct sockaddr_storage inc_ip;
    socklen_t addr_size;
    struct addrinfo hints, *res, *p;
    struct sigaction sa;
    int result;
    char *ipver, *s;

    if (argc < 2) {
        fprintf(stderr, "must provide a hostname");
        exit(1);
    }

    memset(&hints, 0, sizeof(hints)); // ensure hints is empty
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Check that getaddrinfo returned successfully
    if ((result = getaddrinfo(NULL, PORT, &hints, &res) != 0)) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(result));
        return 2;
    }

    // Check that linked list has valid entries
    for (p = res; p =! NULL; p = p->ai_next) {
        if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
            perror("socket error");
            continue;
        };
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("bind error");
            continue;
        };
        break;
    }

    freeaddrinfo(res); // finished with structure

    if (p == NULL) {
        fprintf(stderr, "socket failed to bind");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1){
        perror("listen error");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    while (1) {
        int sin_size = sizeof(inc_ip);
        if (newfd = accept(sockfd, (struct sockaddr *)&inc_ip, &sin_size) ==  -1) {
            perror("accept error");
            continue;
        }
        inet_ntop(inc_ip.ss_family, get_addr_in((struct sockaddr*)&inc_ip), s, sizeof(s));
        printf("connection was made from %s/n", s);
        if (!fork()) { // if child process returned
            close(sockfd); //close old socket
            if (send(newfd, "hello", 5, 0) == -1) {
                perror("send");
                close(newfd);
                exit(1);
            }
        } 
        close(newfd);
    }
    return 0;
}