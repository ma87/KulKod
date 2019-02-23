
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <byteswap.h>

char * encrypt(const char * message, int * size_output)
{
    *size_output = 4 + strlen(message);
    char * crypted_message = malloc(*size_output);
    int size_crypted = __bswap_32 (strlen(message));

    memcpy(crypted_message, &size_crypted, sizeof(int));

    int key = 171;
    int i;
    for (i = 4 ; i < 4 + strlen(message) ; i++)
    {
        int c = (int)message[i - 4];
        int a = key ^ c;
        key = a;
        crypted_message[i] = (char)a;
    }
    return crypted_message;
}

void decrypt(char * buffer, int size_buffer)
{
    int key = 171;
    int i;
    for (i = 0 ; i <  size_buffer ; i++)
    {
        int c = (int)buffer[i];
        int a = key ^ c;
        key = (int)buffer[i];
        buffer[i] = (char)a;
    }
}

void *server(void * data)
{
    printf("server thread\n");
    int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        printf("server: ERROR opening socket\n");

    int yes=1;

     if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
       perror("setsockopt");
       exit(1);
     }
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = 8008;
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              printf("server: ERROR on binding\n");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd,
                 (struct sockaddr *) &cli_addr,
                 &clilen);
     if (newsockfd < 0)
          printf("server: ERROR on accept\n");
     bzero(buffer,256);
     n = read(newsockfd,buffer,255);
     printf("server received message: %s\n", buffer);
     n = write(sockfd,buffer,strlen(buffer));
     if (n < 0)
          printf("server: ERROR writing socket\n");
    close(newsockfd);
    close(sockfd);
}

char * client(char * buffer, int size_buffer)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    //char *buffer = (char *)data;

    portno = 9999;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        printf("client: ERROR opening socket\n");
    server = gethostbyname("192.168.1.248");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        printf("client: ERROR connecting\n");

    //printf("encrypt buffer %s\n", buffer);

    n = write(sockfd,buffer,size_buffer);
    if (n < 0)
         printf("client: ERROR writing socket\n");
    //bzero(buffer,256);

    char output_buffer[255];
    n = read(sockfd,output_buffer,255);
    if (n < 0)
         printf("client: ERROR reading socket\n");
    printf("tplink returned %d bytes\n", n);

    int size_answer;
    memcpy(&size_answer, output_buffer, sizeof(int));
    size_answer = __bswap_32 (size_answer);

    printf("size_answer = %d\n", size_answer);
    char * answer = (char *)malloc(size_answer);

    memcpy(answer, output_buffer + 4, size_answer);
    decrypt(answer, size_answer);

    close(sockfd);
    return answer;
}

int main(int argc, char *argv[])
{
    pthread_t server_thread;
    pthread_t client_thread;
    /*
    int ret = pthread_create(&server_thread, NULL, server, NULL);
    ret = pthread_create(&client_thread, NULL, client, NULL);

    sleep(0.05); // time for server to initialize socket

    pthread_join(client_thread, NULL);
    pthread_join(server_thread, NULL);
    */

    const char * test_crypt = "\{\"emeter\":\{\"get_realtime\":\{}}}";
    //int size_input = strlen(test_crypt);
    //memcpy(buffer, &size_input, sizeof(int));
    //printf("test_crypt = %s\n", test_crypt);
    //strcpy(buffer + 4, test_crypt);
    //printf("buffer = %s\n", buffer);
    //buffer[strlen(test_crypt) + 1 + 4] = '\0';
    //printf("buffer = %s\n", buffer);

    int size_crypted;
    char * buffer = encrypt(test_crypt, &size_crypted);
    char * answer = client(buffer, size_crypted);
    printf("tplink answer: %s\n", answer);
    /*int ret = pthread_create(&client_thread, NULL, client, (void *)buffer);

    pthread_join(client_thread, NULL);

    int size_return = 0;
    memcpy(&size_return, buffer, sizeof(int));
    decrypt(buffer + 4, size_return);
    //encrypt(buffer, strlen(buffer));
    printf("%s\n", buffer);*/
    return 0;
}
