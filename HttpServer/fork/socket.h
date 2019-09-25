#ifndef socket_h
#define socket_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <ctype.h>

void handler();
int Socket(int family,int type,int protocol);
void Bind(int fd, const struct sockaddr *sa, socklen_t len);
void Listen(int fd,int backlog_size);
int Accept(int fd,struct sockaddr *sa,socklen_t *len);
long Read(int fd, void *buf, size_t len);
void Write(int fd, void *buf, size_t len);
void Close(int fd);
int get_file(char *uri,char *filename);
void get_response(int connfd,char *filename);

#endif

