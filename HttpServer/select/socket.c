#include "socket.h"
#define MAXLINE 1024

int Socket(int family,int type,int protocol) {
    int socketfd = socket(family, type, protocol);
    if(socketfd < 0) {
		printf("%s\n", "Error : Fail to connect socket\n");
        return -1;
    }
    return socketfd;
}

void Bind(int fd, const struct sockaddr *sa, socklen_t len) {
    if(bind(fd, sa, len) < 0 ) {
		printf("%s\n", "Error : Fail to bind socket\n");
        exit(1);
    }
}

void Listen(int fd,int backlog_size) {
    if(listen(fd, backlog_size) < 0){
        printf("%s\n", "Error : Fail to listen client\n");
		exit(1);
	}
}

int Accept(int fd,struct sockaddr *sa,socklen_t *len) {
    int clientfd = accept(fd, sa, len);
    if(clientfd < 0)
        printf("%s\n", "Error : Fail to accept client\n");
    return clientfd;
}

long Read(int fd, void *buf, size_t len) {
    
	char   *ptr;
	size_t  nleft;
    ssize_t nread;
    
    ptr = buf;
    nleft = len;
    while (nleft > 0) {
        if ( (nread = read(fd, ptr, nleft)) < 0) {
            if (errno == EINTR)
                nread = 0;
            else
                return -1;
        } else if (nread == 0)
            break;
        ptr += nread;
        nleft -= nread;
    }
    return len - nleft;
}

void Write(int fd, void *buf, size_t len) {
    if(write(fd, buf, len) == -1 )
        printf("%s\n", "Error : Fail to write\n");
}

void Close(int fd) {
    if(close(fd) == -1)
        printf("%s\n", "Error : Fail to close fd\n");
}

int get_file(char *uri,char *filename) {
    strcpy(filename, ".");
    strcat(filename, uri);
    if (uri[strlen(uri)-1] == '/')
        strcat(filename, "index.html");
    return 1;
}

void get_filetype(char *filename, char *filetype) {
    if (strstr(filename, ".html") || strstr(filename, ".htm") || strstr(filename, ".HTML") || strstr(filename, ".HTM") || strstr(filename, ".HTM"))
        strcpy(filetype, "text/html");
	else if (strstr(filename, ".css") || strstr(filename, ".CSS"))
        strcpy(filetype, "text/css");
	else if (strstr(filename, ".js"))
        strcpy(filetype, "text/javascript");
    else if (strstr(filename, ".gif"))
        strcpy(filetype, "image/gif");
    else if (strstr(filename, ".png"))
        strcpy(filetype, "image/png");
    else if (strstr(filename, ".jpg"))
        strcpy(filetype, "image/jpeg");
    else if (strstr(filename, ".jpeg"))
        strcpy(filetype, "image/jpeg");
    else if (strstr(filename, ".txt"))
        strcpy(filetype, "text/plain");
    else
        strcpy(filetype, "text/plain");
}

// For 404
void error_response(int connfd) {
    struct stat sbuf;
    int filefd;
    char *srcp;
    char response[MAXLINE];
    char filename[] = "./404.html";
    
    if(stat(filename,&sbuf) < 0) {
        printf("Error : Fail to find 404.html\n");
        return;
    }
    filefd = open(filename,O_RDONLY);
    
    sprintf(response, "HTTP/1.1 404 Not found\r\n");
    sprintf(response, "%sServer: Web Server\r\n",response);
    sprintf(response, "%sConnection: close\r\n",response);
    sprintf(response, "%sContent-length: %ld\r\n",response,sbuf.st_size);
    sprintf(response, "%sContent-type: text/html\r\n\r\n",response);
    Write(connfd, response, strlen(response));
    
    srcp = mmap(0, sbuf.st_size, PROT_READ, MAP_PRIVATE, filefd, 0);
    Close(filefd);

    Write(connfd, srcp, sbuf.st_size);
    munmap(srcp, sbuf.st_size);
}

void get_response(int connfd,char *filename) {
    struct stat sbuf;
    int filefd;
    char *srcp;
    char response[MAXLINE], filetype[MAXLINE];

    if(stat(filename,&sbuf) < 0) {
        error_response(connfd);
        return;
    }else {
        get_filetype(filename,filetype);
        filefd = open(filename,O_RDONLY);
            
        sprintf(response, "HTTP/1.0 200 OK\r\n");
        sprintf(response, "%sServer: Web Server\r\n",response);
        sprintf(response, "%sConnection: close\r\n",response);
        sprintf(response, "%sContent-length: %ld\r\n",response,sbuf.st_size);
        sprintf(response, "%sContent-type: %s\r\n\r\n",response,filetype);
        Write(connfd, response, strlen(response));
        printf("Response headers:\n");
        printf("%s\n",response);

        srcp = mmap(0, sbuf.st_size, PROT_READ, MAP_PRIVATE, filefd, 0);
        Close(filefd);

        Write(connfd, srcp, sbuf.st_size);
        munmap(srcp, sbuf.st_size);
    }
}
