//
// Created by 97250 on 22/05/2023.
//

#ifndef OS_4_ST_REACTOR_HPP
#define OS_4_ST_REACTOR_HPP

#include <vector>
#include <array>
#include <poll.h>
#include <unordered_map>
#include <pthread.h>

#define PORT "9034"   // Port we're listening on

typedef void (*handler_t)(void *, int);

typedef struct {
    std::vector<pollfd> fds;
    std::unordered_map<int,handler_t> handlers;
    size_t clients = 0;
    pthread_t thread;
    int stop = 0;
} Reactor;


void trim(char *str);
void removeFd(void *reactor, int fd);
void stopReactor(void *arg);
void addFd(void *reactor, int fd, handler_t handler);
void recvHandler(void *reactor, int fd);
void connectionHandler(void *reactor, int fd);
void *reactorLoop(void *arg);
void waitFor(void *reactor);
void *createReactor();
void startReactor(void *reactor);


#endif //OS_4_ST_REACTOR_HPP

