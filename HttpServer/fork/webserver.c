#include "socket.h"

#define MAXLINE 128
#define SERV_PORT 80
#define BACKLOGSIZE 30

int main(int argc, char* argv[]) {

    int listenfd, connfd, n, pid, status;
    char method[MAXLINE], uri[MAXLINE], version[MAXLINE], buffer[MAXLINE];
    char filename[MAXLINE], str[INET_ADDRSTRLEN];
	struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len;
    
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    
    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    Listen(listenfd, BACKLOGSIZE);
	printf("\nReady For Accepting Connections ...\n");    

	struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
	sa.sa_handler = handler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    while(1){

        cliaddr_len = sizeof(cliaddr);
        connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
        
        pid = fork();
        if(pid < 0){
            printf("Error : Fail to fork\n");
	    	exit(1);
        }else if(pid == 0){
            while (1) {
                n = Read(connfd, buffer, MAXLINE);
                if (n == 0)
                    break;
                //printf("Received from %s at Port %d\nMessage is %s\n",inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),ntohs(cliaddr.sin_port),buffer);
                
                sscanf(buffer, "%s %s %s", method, uri, version);
                printf("method:%s	uri:%s	version:%s\n", method, uri, version);
                
                if(strcasecmp(method, "GET") == 0 && strstr(version, "HTTP")){
                    if(get_file(uri, filename))
                        get_response(connfd, filename);
                }else if(strcasecmp(method, "POST") == 0 && strstr(version, "HTTP")){
                    printf("This program is not support POST method\n");
                }
            }
            Close(connfd);
            exit(0);
        }else {
            Close(connfd);
            continue;
        }
    }
    shutdown(listenfd, SHUT_RDWR);
    return 0;
}
