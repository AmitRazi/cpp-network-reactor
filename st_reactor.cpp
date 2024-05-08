//
// Created by 97250 on 22/05/2023.
//

#include "st_reactor.hpp"
#include <iostream>
#include <poll.h>
#include <vector>
#include <array>
#include <fcntl.h>
#include <algorithm>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


void trim(char *str){
    int len = strlen(str);
    for(int i = 0 ; i < len ; i++){
        if(str[i] == '\r'){
            str[i] = '\0';
            break;
        }
    }
}
void removeFd(void *reactor, int fd) {
    Reactor *castReactor = static_cast<Reactor *>(reactor);

    auto it = std::remove_if(castReactor->fds.begin(),castReactor->fds.end(),[&](const pollfd &pfd){
        return pfd.fd == fd;
    });

    castReactor->fds.erase(it, castReactor->fds.end());

    castReactor->handlers.erase(fd);
    castReactor->clients -= 1;
}

int createListeningSocket() {
    int fd;
    int yes = 1;
    int rv;

    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        perror("getaddrinfo");
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (fd < 0) {
            continue;
        }

        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(fd, p->ai_addr, p->ai_addrlen) < 0) {
            close(fd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        return 1;
    }

    if (listen(fd, 10) == -1) {
        close(fd);
        return -1;
    }

    return fd;
}
void stopReactor(void *arg){
    Reactor *castReactor = static_cast<Reactor *>(arg);
    castReactor->stop = 1;
}
void addFd(void *reactor, int fd, handler_t handler) {
    Reactor *castReactor = static_cast<Reactor *>(reactor);
    pollfd newFd;
    memset(&newFd, 0, sizeof(pollfd));
    newFd.fd = fd;
    newFd.events = POLLIN;
    castReactor->fds.emplace_back(newFd);
    castReactor->handlers[fd] = handler;
    castReactor->clients += 1;
}
void recvHandler(void *reactor, int fd) {
    Reactor *castReactor = static_cast<Reactor *>(reactor);
    char buf[1024];
    int nbytes = recv(fd, buf, sizeof(buf), 0);

    if (nbytes <= 0) {
        if (nbytes == 0) {
            close(fd);
            removeFd(reactor, fd);
        } else {
            perror("recv");
        }
    } else {
        buf[nbytes-1] = '\0';
        trim(buf);
        if(strcmp(buf,"quit_s") == 0){
            stopReactor(reactor);
            return;
        }
        char message[1400];
        sprintf(message,"Message recived from client %d: %s\n",fd-3,buf);
        printf("%s",message);
        for (int i = 1; i < castReactor->clients; i++) {
            int dstFd = castReactor->fds[i].fd;
            if (dstFd != fd) {
                if (send(dstFd, message, strlen(message)+1, 0) == -1) {
                    perror("send");
                }
            }
        }
    }
}
void connectionHandler(void *reactor, int fd) {
    std::cout<<"Accepting a new client...\n";
    int newFd;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;

    addrlen = sizeof(remoteaddr);
    newFd = accept(fd, (struct sockaddr *) &remoteaddr, &addrlen);

    if (newFd == -1) {
        perror("accept");
        return;
    }

    addFd(reactor, newFd, recvHandler);
}


void *reactorLoop(void *arg) {
    Reactor *castReactor = static_cast<Reactor *>(arg);

    int listenFd = createListeningSocket();
    if (listenFd < 0) {
        fprintf(stderr, "Failed to create a listener socket\n");
        exit(1);
    }

    addFd(arg, listenFd, connectionHandler);
    std::cout<<"The reactor is now running...\n";
    while(!castReactor->stop) {
        int pollCount = poll(castReactor->fds.data(), castReactor->fds.size(), -1);

        if (pollCount == -1) {
            perror("poll");
            exit(1);
        }
        for (int i = 0; i < castReactor->clients; i++) {
            if (castReactor->fds[i].revents & POLLIN) {
                castReactor->handlers[castReactor->fds[i].fd](arg,castReactor->fds[i].fd);
            }
            castReactor->fds[i].revents = 0;
        }
    }
    std::cout<<"Stopping the reactor...\n";

    delete castReactor;
    return 0;
}




void waitFor(void *reactor) {
    Reactor *castReactor = static_cast<Reactor *>(reactor);
    pthread_join(castReactor->thread, NULL);
}

void *createReactor() {
    return new Reactor();
}

void startReactor(void *reactor) {
    std::cout<<"Starting the reactor...\n";
    Reactor *castReactor = static_cast<Reactor *>(reactor);
    pthread_create(&(castReactor->thread), NULL, reactorLoop, reactor);
    waitFor(reactor);
}

