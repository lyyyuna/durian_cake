#include <unistd.h>         /* fork */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>     /* sockaddr_in */
#include <string.h>         /* for memset */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define LISTENQ     1024
#define MAXLINE     4096

void str_echo(int sockfd);

int main()
{
    int         listenfd, connfd;
    pid_t       childpid;
    socklen_t   clilen;
    struct sockaddr_in      cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(9877);
    
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    while (1) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

        if ((childpid = fork()) == 0) {
            close(listenfd);
            str_echo(connfd);
            exit(0);
        }

        close(connfd);
    }
}


void 
str_echo(int sockfd)
{
    ssize_t     n;
    char        buf[MAXLINE];

again:
    while ((n = read(sockfd, buf, MAXLINE)) > 0) {
        write(sockfd, buf, n);    
        printf("%s", buf);
    }

    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0)
        ;
}
