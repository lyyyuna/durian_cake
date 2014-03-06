#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <arpa/inet.h>  /* inet_pton */

#define LISTENQ     1024
#define MAXLINE     4096

void str_cli(FILE *fp, int sockfd);

int main(int argc, char **argv)
{
    int                 sockfd;
    struct sockaddr_in  servaddr;

    if (argc != 2) {
        printf("2 arguments");
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9877);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    str_cli(stdin, sockfd);

    exit(0);
}

ssize_t
writen(int fd, const void *vptr, size_t n)
{
    size_t      nleft;
    ssize_t     nwritten;
    const char  *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;
            else 
                return -1;
        }

        nleft -= nwritten;
        ptr   += nwritten;
    }

    return n;
}

static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

static ssize_t
my_read(int fd, char *ptr)
{
    if (read_cnt <= 0) {
again:
        if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
            if (errno == EINTR)
                goto again;
            return -1;
        } else if (read_cnt == 0) 
            return 0;

        read_ptr = read_buf;
    }

    read_cnt --;
    *ptr = *read_ptr++;
    return 1;
}

ssize_t
readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char    c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        if ((rc = my_read(fd, &c)) == 1) {
            *ptr++ = c;
            if (c == '\n')
                break;
        } else if (rc == 0) {
            *ptr = 0;
            return n-1;
        } else
            return -1;
    }

    *ptr = 0;
    return n;
}


void
str_cli(FILE *fp, int sockfd)
{
    FILE *pFile;
    char sendline[MAXLINE], recvline[MAXLINE];

    pFile = fopen("1.txt", "r");
    // while (fgets(sendline, MAXLINE, pFile) != NULL) {
    while (fgets(sendline, MAXLINE, fp) != NULL) {
        writen(sockfd, sendline, strlen(sendline));
        if (readline(sockfd, recvline, MAXLINE) == 0) {
            exit(0);
        }
        printf("%s", recvline);        
    }
}
