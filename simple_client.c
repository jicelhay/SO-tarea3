/**********************************************
 * IIC2333 - Cliente simple de servidor
 **********************************************/

// Incluimos lo necesario

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

/**
 * Funcion para errores
 */
void error(const char *msg)
{
    perror(msg);
    exit(0);
}


/**
 * Funcion principal
 * Se llama especificando el host y puerto
 */
int main(int argc, char *argv[])
{
    // Definimos las estructuras a ocupar
    int sockfd;
    int port_number;
    int n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // Controlamos el error que no se hayan especificado los datos necesario (phost y puerto)
    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"Usage %s hostname port\n", argv[0]);
       exit(0);
    }

    // Extraemos el puerto
    port_number = atoi(argv[2]);

    // Creamos el socket, controlando el error si es que no puede ser creado
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    // Obtenemos el servidor para ver si es valido
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    // Obtenemos los datos para conectarnos al servidor
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(port_number);

    // Intentamos conectarnos al servidor
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    //Solicitamos el mensaje a enviar
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);

    // Lo escribimos intentamos enviar
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");

    // Obtenemos la respuesta 
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");

    // Mostramos la respuesta en pantalla 
    printf("%s\n",buffer);

    // Cerramos el descriptor
    close(sockfd);
    return 0;
}
