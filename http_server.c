#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <strings.h>

#define PORT "3481"
#define BACKLOG 10
#define MAX_BUFFER 1024

void sigchld_handler(int s) {
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

void *get_addr_in(struct sockaddr* sa) {
    if (sa->sa_family == "AF_INET") {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    } else {
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }
}

int main(int argc, char* argv[]) {
    int sockfd, newfd, sin_size;
    struct sockaddr_storage inc_ip;
    struct addrinfo hints, *res, *p;
    struct sigaction sa;
    int result;
    char *s, *str;

    // if (argc < 2) {
    //     fprintf(stderr, "must provide a hostname");
    //     exit(1);
    // }

    memset(&hints, 0, sizeof(hints)); // ensure hints is empty
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Check that getaddrinfo returned successfully
    if ((result = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(result));
        return EXIT_FAILURE;
    }

    // Check that linked list has valid entries
    for (p = res; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket error");
            continue;
        };
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == 0) {
            break;
        };
        close(sockfd);
    }

    if (p == NULL) {
        fprintf(stderr, "no address binded successfully");
        return EXIT_FAILURE;
    }
    freeaddrinfo(res); // finished with structure

    fprintf(stderr,"%d\n", p->ai_family);

    str = inet_ntop(p->ai_family, p->ai_addr, &str, sizeof str);
    fprintf(stderr, "socket created on %s\n", str);

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

     
    fprintf(stderr, "accepting connections \n");
    while (1) {
        sin_size = sizeof inc_ip;
        if ((newfd = accept(sockfd, (struct sockaddr *)&inc_ip, (unsigned int*)&sin_size)) ==  -1) {
            perror("accept error");
            exit(1);
        }
        fprintf(stderr, "inside while loop");
        inet_ntop(inc_ip.ss_family, get_addr_in((struct sockaddr *)&inc_ip), s, sizeof s);
        fprintf(stderr, "connection was made from %s\n", s);
        if (!fork()) { // if child process returned
            close(sockfd); //close old socket
            fprintf(stderr, "old socket closed\n");
            if (send(newfd, "hello\n", 7, 0) == -1) {
                perror("send");
                close(newfd);
                exit(1);
            } else {
                close(newfd);
                fprintf(stderr, "connecting socket closed\n");
                break;
            }
        }
    }
    printf("end\n");
    return 0;
}