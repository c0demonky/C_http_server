#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdlib.h>
#include <err.h>
#include <errno.h>

/* holder for output of getaddrinfo() information*/
struct addrinfo {
    int                 ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
    int                 ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
    int                 ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
    int                 ai_protocol;  // use 0 for "any"
    size_t              ai_addrlen;   // size of ai_addr in bytes
    struct sockaddr_in  *ai_addr;      // struct sockaddr_in or _in6
    char                *ai_canonname; // full canonical hostname

    struct addrinfo *ai_next;      // linked list, next node

};

/* Internt address */
struct sockaddr_in {
    short int          sin_family;  // Address family, AF_INET
    unsigned short int sin_port;    // Port number
    struct in_addr     sin_addr;    // Internet address
    unsigned char      sin_zero[8]; // Same size as struct sockaddr
};

/* Socket address */
// struct sockaddr {
//     unsigned short    sa_family;    // address family, AF_xxx
//     char              sa_data[14];  // 14 bytes of protocol address
// };

/* IP address */
struct in_addr {
    uint32_t s_addr; // that's a 32-bit int (4 bytes)
};

int main() {
    int status;
    struct addrinfo hints, *res;
    struct sockaddr_in socket_address;

    memset(&hints, 0, sizeof(hints)); // ensure hints is empty

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, "2000", &hints, &res) != 0)) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 2;
    };

    // create socket
    int s = socket(PF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        perror("webserver (socket)");
        return 1;
    };
    printf ("socket created succesfully");
    return 0;

    // create socket bind parameters

    // free memory
    freeaddrinfo(res);
}