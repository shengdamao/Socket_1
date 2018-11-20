
#include <stdio.h>   // fputs, 
#include <unistd.h>  // read(), write(), fork(),
#include <string.h>  // void memset( void * s , int c, size_t n );
#include <sys/types.h> 
#include <sys/socket.h>  // socket(), 
#include <arpa/inet.h>  // htons(), htonl(), ntohs(), ntohl()
#include <netinet/in.h> // inet_addr(), inet_aton(), inet_ntoa() 还需要<sys/socket.h> <arpa/inet.h>
#include <errno.h>  // INTR 
#include <stdlib.h> // NULL, EXIT_FAILURE, EXIT_SUCCESS
#include <signal.h> 

#define ERR_EXIT(m) \
    do  {   \
                perror(m); \
            exit( EXIT_FAILURE);   }  \
        while (0) 

ssize_t readn (int fd, void *buf, size_t count)
{
        size_t nleft = count;
        ssize_t nread;
        char * bufp = (char*)buf;
        while (nleft > 0)
        {
                if (( nread = read (fd, bufp, nleft)) < 0)
                {
                        if (errno == EINTR)
                                continue;
                        return -1;
                }
                else if (nread == 0)
                        return count - nleft;
                bufp += nread;
                nleft -= nread;
        }
        return count;
}


ssize_t writen (int fd, const void *buf, size_t count)
{
        size_t nleft = count;
        ssize_t nwritten;
        char * bufp = (char *)buf;
        while (nleft > 0)
        {
                if (( nwritten = write (fd , bufp, nleft )) < 0 )
                {
                        if (errno == EINTR)
                                continue;
                        return -1;
                }
                else if ( nwritten == 0)
                        continue;
                bufp += nwritten;
                nleft -= nwritten;
        }
        return count;
}


ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
        while (1)
        {
                int ret = recv(sockfd, buf, len , MSG_PEEK);
                if ( ret == -1 && errno == EINTR)
                        continue;
                return ret;
        }
}

ssize_t readline(int sockfd, void *buf, size_t maxline)
{
        int ret;
        int nread;
        char *bufp = buf;
        int nleft = maxline;
        while (1)
        {
                ret = recv_peek (sockfd, bufp, nleft);
                if ( ret < 0 )
                        return ret;
                else if (ret == 0)
                        return ret;
                nread = ret;
                int i;
                for ( i = 0; i < nread; i++)
                {
                        if (bufp[i] == '\n')
                        {
                                ret = readn(sockfd, bufp, i + 1);
                                if (ret != i + 1)
                                        exit(EXIT_FAILURE);
                                return ret;
                        }
                }
                if (nread > nleft)
                        exit(EXIT_FAILURE);
                nleft -= nread;
                ret = readn( sockfd, bufp, nread);
                if (ret != nread)
                bufp += nread;
        }
        return -1;
}
          
void echo_cli (int sock)
{
	fd_set rset;
	FD_ZERO(&rset);
	int maxfd;
	int nready;
	int fd_stdin = fileno (stdin);
	if (fd_stdin > sock) 
		maxfd = fd_stdin;
	else maxfd = sock;
	
	char sendbuf[1024] = {0};
	char recvbuf[1024] = {0};

	while (1) 
	{
		FD_SET (fd_stdin, &rset);
		FD_SET (sock, &rset);
		nready = select (maxfd + 1, &rset, NULL, NULL, NULL);
		if (nready == -1)
			ERR_EXIT("select");
		if (nready == 0)
			continue;
		if (FD_ISSET(sock, &rset))
		{
 			int ret = readline(sock, recvbuf, sizeof(recvbuf));	
			if (ret == -1)
				ERR_EXIT("readline");
			else if (ret == 0)
			{
				printf("server closed");
				break;
	
			}
			fputs(recvbuf, stdout);
			memset (recvbuf, 0 ,sizeof(recvbuf));

		}
		if ( FD_ISSET(fd_stdin, &rset))
		{		
			if (fgets(sendbuf, sizeof(sendbuf), stdin) == NULL)
				break;
			writen (sock, sendbuf, strlen (sendbuf));
			memset  (sendbuf, 0, sizeof(sendbuf));
		}

	}
}

int main (void)
{
    int sock;
    if (( sock = socket (AF_INET, SOCK_STREAM, 0)) < 0 )
        ERR_EXIT("socket");
    struct sockaddr_in servaddr;
    memset( &servaddr, 0, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
       
    if ( connect (sock , (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT( " connect" );
/*
    char sendbuf[1024] = {0};i
    char recvbuf[1024] = {0};

    while (fgets (sendbuf, sizeof(sendbuf), stdin ) != NULL )
    {
        writen  (sock,  sendbuf, strlen (sendbuf));
 	
	int ret =readline( sock, recvbuf, sizeof (recvbuf));
	if ( ret == -1 )
		ERR_EXIT("readline");
	else if (ret == 0)
	{
		printf ( "client close \n");
		break;	
	}

        fputs (recvbuf, stdout);
	memset ( sendbuf, 0, sizeof (sendbuf));
	memset ( recvbuf, 0, sizeof (recvbuf));
    } */
	
    echo_cli(sock);
    return 0;   
} 
