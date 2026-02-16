#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
// Expanded stub for lwip/sockets.h for unit testing on PC
#define AF_INET 2
#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17
#define SOL_SOCKET 0xffff
#define SO_BROADCAST 0x0020

    typedef int lwip_socket_t;
    typedef int ssize_t;
    struct in_addr
    {
        unsigned long s_addr;
    };
    struct sockaddr_in
    {
        short sin_family;
        unsigned short sin_port;
        struct in_addr sin_addr;
        char sin_zero[8];
    };

    static inline int socket(int, int, int) { return 0; }
    static inline int setsockopt(int, int, int, const void *, int) { return 0; }
    static inline int sendto(int, const void *, int, int, const struct sockaddr *, int) { return 0; }
    static inline int closesocket(int) { return 0; }
    /* Remove stub for close(int) to avoid MSVC CRT linkage conflict */
    static inline int inet_pton(int, const char *, void *) { return 1; }
    static inline unsigned short htons(unsigned short x) { return x; }

#ifdef __cplusplus
}
#endif
