#include <stdio.h>   // fputs, 
#include <unistd.h>  // read(), write(), fork(),
#include <string.h>  // void memset( void * s , int c, size_t n );
#include <sys/types.h>   // socket(), bind(), listen(),
                        //
#include <sys/socket.h>  // socket(), 

#include <arpa/inet.h>  // htons(), htonl(), ntohs(), ntohl()

#include <netinet/in.h> // inet_addr(), inet_aton(), inet_ntoa() 还需要<sys/socket.h> <arpa/inet.h>

#include <errno.h>  // INTR 
#include <stdlib.h> // NULL, EXIT_FAILURE, EXIT_SUCCESS

#define ERR_EXIT(m) \
    do  {   \
                perror(m); \
            exit( EXIT_FAILURE);\    
        }  \
        while (0) 

void do_service (int conn)
{
    char recvbuf[1024];
    while 1 {
        memset (recvbuf, 0 , sizeof(recvbuf));
        int ret = read (conn, recvbuf, sizeof(recvbuf));
        if (ret == 0)
        {
            printf ("client closed\n");
            break;
        }
        else if (ret == -1)
            ERR_EXIT("read");
        fputs (recvbuf, stdout);
        write (conn, recvbuf, sizeof (recvbuf));
    }
}

int main (void)
{
    int listenfd;
    if (( listenfd = socket (AF_INET, SOCK_STREAM, 0)) < 0 )
        ERR_EXIT("socket");
    struct sockaddr_in servaddr;
    memset( &servaddr, 0, size0f (servaddr));
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
    socklen_t peerlen = sizeof ( peeraddr );
    socklen_t * pt_len = &peerlen;

    int conn;
    pid_t pid;

    while (1) {
        if ((conn = accept ( listenfd, (struct sockaddr *) &peeraddr, pt_len)) < 0 )
            ERR_EXIT("accept") ;
        
        printf ("IP: %s, port: %d\n", inet_ntoa (peeraddr.sin_addr),
                                      ntohs(peeraddr.sinport) );

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
    close (listenfd);
    return 0;
    
}








