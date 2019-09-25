#include "socket.h"

#define MAXLINE 128
#define SERV_PORT 80
#define BACKLOGSIZE 30

int main(int argc, char* argv[]) {
    
    int listenfd, connfd, pid, i, n;
    char method[MAXLINE], uri[MAXLINE], version[MAXLINE], buffer[MAXLINE];
    char str[INET_ADDRSTRLEN], filename[MAXLINE];
	struct sockaddr_in servaddr,cliaddr;
    socklen_t cliaddr_len;
	int maxi, maxfd, sockfd, nready, client[FD_SETSIZE];
    fd_set rset, allset;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    
    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    Listen(listenfd, BACKLOGSIZE);
    printf("\nReady For Accepting Connections ...\n");
    
    maxfd = listenfd;			/* initialize */
    maxi = -1;					/* index into client[] array */
    for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;			/* -1 indicates available entry */
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    while (1) {
		
		rset = allset;			/* structure assignment */
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset)) {	/* new client connection */

			cliaddr_len = sizeof(cliaddr);
			connfd = Accept(listenfd, (struct sockaddr *) &cliaddr, &cliaddr_len);
#ifdef	NOTDEF
			printf("new client: %s, port %d\n",
					Inet_ntop(AF_INET, &cliaddr.sin_addr, 4, NULL),
					ntohs(cliaddr.sin_port));
#endif
			for (i = 0; i < FD_SETSIZE; i++)
				if (client[i] < 0) {
					client[i] = connfd;	/* save descriptor */
					break;
				}

			if (i == FD_SETSIZE){
				//err_quit("too many clients");
				printf("too many clients");
				continue;
			}

			FD_SET(connfd, &allset);	/* add new descriptor to set */
			if (connfd > maxfd)
				maxfd = connfd;			/* for select */
			if (i > maxi)
				maxi = i;				/* max index in client[] array */

			if (--nready <= 0)
				continue;				/* no more readable descriptors */
		}

		for (i = 0; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset)) {
				/*if ( (n = Read(sockfd, buffer, MAXLINE)) == 0) {
					Close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				} else
					Writen(sockfd, buffer, n);*/
				while (1) {
					n = Read(sockfd, buffer, MAXLINE);
					if (n == 0)
						break;
					//printf("Received from %s at Port %d\nMessage is %s\n",inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),ntohs(cliaddr.sin_port),buffer);
				
					sscanf(buffer, "%s %s %s", method, uri, version);
					printf("method:%s	uri:%s	version:%s\n", method, uri, version);
			
					if(strcasecmp(method, "GET") == 0 && strstr(version, "HTTP")) {
						if(get_file(uri, filename)) 
							get_response(sockfd, filename);
					}else if(strcasecmp(method, "POST") == 0 && strstr(version, "HTTP")) {
						printf("This program is not support POST method\n");
					}
				}
			
				Close(sockfd);
				FD_CLR(sockfd, &allset);
				client[i] = -1;

				if (--nready <= 0)
						break;				/* no more readable descriptors */
			}
		}
    }
    shutdown(listenfd, SHUT_RDWR);
    return 0;
}

