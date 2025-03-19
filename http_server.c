#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdlib.h>
#include <err.h>
#include <errno.h>

#include "net_structs.h"

#define PORT "2001"
#define BACKLOG 10
#define MAX_BUFFER 1024

//int argc, char* argv[]
int main() {
    int sockfd, newfd;
    struct sockaddr_storage inc_ip;
    socklen_t addr_size;
    struct addrinfo hints, *res, *p;
    struct sockaddr_in *sock;
    struct sockaddr_in6 *sock6;
    char *ipver;

    char *msg = "hi from sock world";
    char *buff;
    int bytes_sent, bytes_rec;

    // if (argc < 2) {
    //     fprintf(stderr, "must provide a hostname");
    // };

    memset(&hints, 0, sizeof(hints)); // ensure hints is empty

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Check that getaddrinfo returned successfully
    if ((sockfd = getaddrinfo(NULL, PORT, &hints, &res) != 0)) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(sockfd));
        return 2;
    };

    // Check that linked list has valid entries
    // for (p = res; p =! NULL; p = p->ai_next) {
    //     if (p->ai_family == "AF_INET") {
    //         sock = p->ai_addr;
    //         // struct in_addr *address = &(p->ai_addr->sin_addr);
    //         ipver = &("IPv4");
    //         fprintf(stdout, "using IPv4");
    //     } else {
    //         sock6 = p->ai_addr;
    //         // struct in6_addr *address = &(p->ai_addr->sin_addr);
    //         ipver = &("IPv6");
    //         fprintf(stdout, "using IPv6");
    //     };
    // };

    // Create socket
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("webserver (socket)");
        return 1;
    };
    printf ("socket created succesfully");

    // Create socket bind parameters
    if (*ipver == "IPv4") {
        int b = bind(sockfd, sock, sizeof(sock->sin_addr));
        if (b == -1) {
            fprintf(stderr, " failed with error number %d", errno);
            return 1;
        };
    } else {
        int b = bind(sockfd, sock6, sizeof(sock6->sin6_addr));
        if (b == -1) {
            fprintf(stderr, " failed with error number %d", errno);
            return 1;
        };
    };

    listen(sockfd, BACKLOG);
    newfd = accept(sockfd, (struct sockaddr *)&inc_ip, &addr_size);
    bytes_sent = send(newfd, msg, strlen(msg), 0);
    bytes_rec = recv(newfd, buff, MAX_BUFFER, 0);

    fprintf(stdout, "%s", buff);

    // Release memory
    freeaddrinfo(res);

    return 0;
}