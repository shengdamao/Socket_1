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


int main (void)
{
    
    int sock;
    if (( sock = socket (AF_INET, SOCK_STREAM, 0)) < 0 )
        ERR_EXIT("socket");
    struct sockaddr_in servaddr;
    memset( &servaddr, 0, size0f (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
       
    if ( connect (sock , (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT( " connect" );
     

    pid = fork();
    if (pid == -1)
            ERR_EXIT("fork");
    if (pid == 0 )
    {
            
                char recvbuf[1024];
                while (1)
                {
                memset (recvbuf, 0 , sizeof(recvbuf));
                int ret = read (sock, recvbuf, sizeof(recvbuf));
                if (ret == 0)  
                {
                  printf ("peer closed\n");
                  break;
                }
                else if (ret == -1)
                 ERR_EXIT("read");
                else 
                    fputs (recvbuf, stdout);
                }
                close (sock);
                exit(EXIT_SUCCESS);
    }
    else
    {              
            char sendbuf[1024];
            while (fgets(sendbuf, sizeof (sendbuf), stdin) != NULL)
            {
                write (sock, sendbuf, strlen(sendbuf));
                memset (sendbuf, 0, sizeof(sendbuf));            
            }
            close (sock);
            exit(EXIT_SUCCESS);
    }   
            
 

       
        
    return 0;
    
}


