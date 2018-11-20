#include <stdio.h>   // fputs, 
#include <unistd.h>  // read(), write(), fork(),
#include <string.h>  // void memset( void * s , int c, size_t n );
#include <sys/types.h> 
 #include <sys/socket.h>  // socket(), 
#include <arpa/inet.h>  // htons(), htonl(), ntohs(), ntohl()
#include <netinet/in.h> // inet_addr(), inet_aton(), inet_ntoa() 还需要<sys/socket.h> <arpa/inet.h>
#include <errno.h>  // INTR 
#include <stdlib.h> // NULL, EXIT_FAILURE, EXIT_SUCCESS
                        //
#define ERR_EXIT(m) \
    do  {   \
                perror(m); \
            exit( EXIT_FAILURE); }    \
        while (0) 
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

void do_service (int conn)
{
        char recvbuf[1024];
        while (1)
        {
 	  memset (recvbuf, 0 , sizeof(recvbuf));
          int ret = readline (conn, recvbuf, 1024 );
     	  if (ret == -1)
		ERR_EXIT("readline");		
   	  if (ret == 0)
         {
         	    printf ("client closed\n");
       		     break;
       	 }

     	 fputs (recvbuf, stdout);
       	 writen (conn, recvbuf, strlen (recvbuf));
   	 }
}

int main (void)
{
    int listenfd;
    if (( listenfd = socket (AF_INET, SOCK_STREAM, 0)) < 0 )
        ERR_EXIT("socket");
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof (servaddr));


    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188);
/*  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); */
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1; 
    if ( setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0 )
        ERR_EXIT ("setsock");
    if ( bind (listenfd , (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT( " bind" );
    if ( listen (listenfd, SOMAXCONN) < 0 )
        ERR_EXIT( "listen") ;

    struct sockaddr_in peeraddr;
    socklen_t peerlen;
//    socklen_t * pt_len = &peerlen;

    int conn;
//    pid_t pid;
/*
    while (1) {
        if ((conn = accept ( listenfd, (struct sockaddr *) &peeraddr, pt_len)) < 0 )
            ERR_EXIT("accept") ;
        
        printf ("IP: %s, port: %d\n", inet_ntoa (peeraddr.sin_addr),
                                      ntohs(peeraddr.sin_port) );

        pid = fork();
        if (pid == -1)
            ERR_EXIT("fork");
        if (pid == 0 )
        {
            do_service(conn);
            close (listenfd);
            exit (EXIT_SUCCESS);
        }
        else close (conn);
    }
*/	int i;
    	int client[FD_SETSIZE];
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;

	int nready;
	int maxfd = listenfd;
	int maxi = 0;
	fd_set rset;
	fd_set allset;
	FD_ZERO(&rset);
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	while(1)
	{
		rset = allset;
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);
		if (nready == -1)
		{
			if (errno == EINTR)
				continue;
			ERR_EXIT("select");
		}
		if (nready == 0)
			continue;
		if (FD_ISSET(listenfd, &rset))
		{
			peerlen = sizeof(peeraddr);
			conn = accept(listenfd, (struct sockaddr*) &peeraddr, &peerlen);
			
			if (conn == -1)
				ERR_EXIT("accept");
			for (i = 0; i < FD_SETSIZE; i++)
			{
	  			if (client[i] < 0)
				{
					client[i] = conn;
					if (i > maxi)
						maxi = i;
					break;
				}
			}
			if (i == FD_SETSIZE)
			{
				fprintf(stderr, "to many clients");
				exit(EXIT_FAILURE);
			}
			printf("ip=%s port= %d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
			FD_SET (conn, &allset);
			if (conn > maxfd)
				maxfd = conn;
 			
			if (--nready <= 0)
				continue;
		}

		for (i = 0; i < maxi; i++)
		{
			conn = client[i];
			if (conn == -1)
				continue;
			if (FD_ISSET(conn, &rset))
			{
				char recvbuf[1024] = {0};
				int ret = readline(conn, recvbuf, 1024);
				if (ret == -1)
					ERR_EXIT("readline");
				if(ret == 0)
				{
					printf("client close\n");
					FD_CLR(conn, &allset);
					client[i] = -1;
 					
				}
				fputs (recvbuf, stdout);
				writen (conn, recvbuf, strlen(recvbuf));
				if (--nready <= 0)
					break;
			}
					

		}
	}
	
	



    close (listenfd);
    return 0;
    
}

