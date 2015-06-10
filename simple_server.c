/*********************************************/
/* IIC2333 Ejemplo de un servidor simple     */
/*********************************************/

// Incluimos lo necesario

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

/**
 * Funcion para desplegar los errores
 */
void error(const char *msg)
{
	perror(msg);
	exit(1);
}


//Creamos el socket parecido al chat
int create_tcp_socket();

//Se busca la IP del host
char *get_ip(char *host);

//Se arma el request HTTP
char *build_get_query(char *host, char *page);

// Un método que le dice al usuario como usar el programa
void usage();


// Se define un host por defecto-----
#define HOST "www.uc.cl"
#define PAGE "/"
#define PORT 80
#define USERAGENT "HTMLGET 1.0"
//---------------------------



//Le digo al usuario como usar mi programa


void usage()
{
    fprintf(stderr, "USO: htmlget host [recurso solicitado]\n\
            \thost: El host del sitio ejemplo: www.uc.cl\n\
            \trecurso solicitado: el recurso que quiero ejemplo:index.html, default: /\n");
}


// Creo el socket
int create_tcp_socket()
{
    int sock;
    //Acá se setea el socket y se buscan errores
    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        perror("No se puede crear  el socket");
        exit(0);
    }
    return sock;
}


//Obtendo la IP
char *get_ip(char *host)
{
    // hostent libería netdb
    struct hostent *hent;
    
    //El largo de la IP
    int iplen = 15; //XXX.XXX.XXX.XXX
    
    //Se crea un string del largo de la ip que va a ser lo que devolveremos
    char *ip2 = (char *)malloc(iplen+1);
    
    //Seteamos un trozo de memoria
    memset(ip2, 0, iplen+1);
    
    // Se crea el "objeto hostent hent"
    hent = gethostbyname(host);
    
    // magia netdb que setea la ip que obtuvo del host
    inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip2, iplen);
    
    return ip2;
}





//Armo la solicitud -Podría modificarse...
char *build_get_query(char *host, char *page)
{
    //El string de la query
    char *query;
    //La página solicitada
    char *getpage = page;
    
    //La solicitud GET, esto lo tienene que modificar porque tiene que llegar desde el cliente a su proxy
    char *tpl = "HEAD /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
    
    // Qué pasará si pongo HEAD???????!
    
    if(getpage[0] == '/'){
        getpage = getpage + 1;
        fprintf(stderr," \"/\",  %s %s\n", page, getpage);
    }
    
    //Creamos el string query vacío
    query = (char *)malloc(strlen(host)+strlen(getpage)+strlen(USERAGENT)+strlen(tpl)-5);
    
    //Se setea acá:
    sprintf(query, tpl, getpage, host, USERAGENT);
    return query;
}


/**
 * Funcion principal, se llama indicando el puerto de escucha
 */
int main(int argc, char *argv[])
{
	
	// Definimos estructuras necesarias
    //necesarioas para el request
    struct sockaddr_in *remote;
    int sock;
    int tmpres;
    char *ip2;
    char *get2;
    char buf[BUFSIZ+1];
    char *host;
    char *page;

	//Descriptor del servidor
	int server_socket_descriptor;
	//Descriptor del cliente cliente
	int client_socket_descriptor;
	//Puerto:
	int port_number;
	//ALGO
	socklen_t client_lenght;
	//Un buffer para enviar info
	char buffer[256];
	//Estructura del servidor
	struct sockaddr_in server_addr;
	//Estructura cliente
	struct sockaddr_in  client_addr;

	//Esto sirve para leer el futuro mesaje del cliente
	int n;

	// Controlamos el error de no proveer el puerto para el servidor
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	// Creamos el socket del servidor
	server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	/*

    domain. Se podrá establecer como AF_INET (para usar los protocolos de Internet), o como AF_UNIX (si se desea crear sockets para la comunicación interna del sistema). Éstas son las más usadas, pero no las únicas. Existen muchas más, aunque no se nombrarán aquí.

    type. Aquí se debe especificar la clase de socket que queremos usar (de Flujos o de Datagramas). Las variables que deben aparecer son SOCK_STREAM o SOCK_DGRAM según querramos usar sockets de Flujo o de Datagramas, respectivamente.

    protocol. Aquí, simplemente se puede establecer el protocolo a 0. 

	*/
	
	// Controlamos el error de que no haya podido ser creado
	if (server_socket_descriptor < 0) 
		error("ERROR opening socket");


	// Especificamos los detalles para el servidor
	//La  función  bzero() pone a cero los primeros 'sizeof(server_addr)' bytes del área de bytes que comienza en &server_addr.
	bzero((char *) &server_addr, sizeof(server_addr));

	//Tomamos el puerto dado por el segundo argumento del programa
	port_number = atoi(argv[1]);

	// Se setean los valores del struct
	 server_addr.sin_family = AF_INET;
	 // Que ips vamos a escuchar, si queremos una especifica podemos poner: sin.sin_addr.s_addr = inet_addr("xxx.xxx.xxx.xxx");
	 server_addr.sin_addr.s_addr = INADDR_ANY;

	 // htons() transforma la orden en bytes para redes.
	 server_addr.sin_port = htons(port_number);

	// Registramos el socket
	if (bind(server_socket_descriptor, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) 
			 error("ERROR on binding");

	// Comenzamos a escuchar por el socket, 5 es la cantidad de procesos que pueden estar esperando mientra se establece la conexion.		
	listen(server_socket_descriptor,5);
	printf("-- Server listening --\n");	

	// Obtenemos el largo del mensaje
	client_lenght = sizeof(client_addr);

	

	

	// Aceptamos la conexion de algun cliente
	client_socket_descriptor = accept(server_socket_descriptor, 
				(struct sockaddr *) &client_addr, 
				&client_lenght);


	struct sockaddr_in* try = (struct sockaddr_in*)&client_addr;

	//int ipAddr = try->sin_addr.s_addr;
	//printf("%d",ipAddr);
	
	char* ip=  (char *) inet_ntoa(client_addr.sin_addr);
	printf("IP de cliente: %s \n",ip);



	// Si no se puede obtener el descriptor, enviamos un error
	if (client_socket_descriptor < 0) 
		 error("ERROR on accept");

	// Comenzamos la lectura del mensaje	
	bzero(buffer,256);
	n = read(client_socket_descriptor,buffer,255);

	// Controlamos el error de lectura del socket
	if (n < 0) 
		error("ERROR reading from socket");
	
	// Impimimos el mensaje enviado
	
	//ver si contiene get

	char* get;
	char* get_aux;
	get= strtok_r(buffer, " ",&get_aux);
	char* IP;
	IP=strtok(get_aux,"\n");	
	


	if(strcmp(get, "GET") == 0){
		printf("Request va a ser procesada\n");
				   }
	else	{n = write(client_socket_descriptor,"no GET",18);
			if (n < 0) {
				error("ERROR writing to socket");
				}
		error("Tu request tiene que ser del tipo GET");
					}




	//ver si el ip está en el archivo de permitidos

		int c;
	FILE *file;
	file = fopen("ips", "r");
 	char line[50];
	bool permited=false;
	char* aux_line;

	if (file) {
    	  while (fgets(line, sizeof(line), file)) {

       	aux_line=strtok(line,"\n");

	if(strcmp(ip, aux_line) == 0){
		permited=true;
				   }
    }
		  }

	if(permited){

	printf("IP permitido, se realizara petición\n");

	//ver si el archivo está en cache
	char *fname = malloc (strlen ("cache\\") + strlen(IP));
    	char *fnameaux= IP;
	strcpy (fname, "cache/");
	strcat (fname,IP);

	 FILE *f;
	f= fopen(fname, "r");
  	  if (f)
   	 {
		//en este caso existe, hay que leerlo y retornar el contenido
		printf ("Archivo está en cache\n");
		
		char * buffer = 0;
		long length;

		//guardamos todo el archivo en un string, lo enviamos.
		  fseek (f, 0, SEEK_END);
  		length = ftell (f);
  		fseek (f, 0, SEEK_SET);
  		buffer = malloc (length);
 		 if (buffer)
 		 {
    			fread (buffer, 1, length, f);
  		}
  		fclose (f);

	printf ("Texto a enviar:\n%s",buffer);
		
	n = write(client_socket_descriptor,buffer,length);

	if (n < 0) 
		error("ERROR writing to socket");
       		 
       	}
	else{ 
		
		
		printf( "no se encontro el archivo: %s hay que hacer http request\n",fname);

		//en este caso no existe cache y tenemos que hacer request, ---> INSERTAR REQUEST AQUI.
        
        //if(argc == 1)
        //{
        //  usage();
        //exit(0);
        //}
        //char *variable1="./htmlget ";
        //host = (char *)malloc(strlen(variable1)+strlen(fname));
        
        host=fnameaux;
        
        if(argc > 2)
        {
            page = argv[2];
        }
        else
        {
            page = PAGE;
        }
        printf("logramos pasar. ahora vamos a crear el tcpsocket\n");
        sock = create_tcp_socket();
        printf("vamos a ver la ip del servidor\n");
        ip2 = get_ip(host);
        printf("logramos conseguir la ip\n");
        fprintf(stderr, "La IP del host es: %s\n", ip2);
        
        remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
        
        
        remote->sin_family = AF_INET;
        tmpres = inet_pton(AF_INET, ip2, (void *)(&(remote->sin_addr.s_addr)));
        
        
        //Según el valor del tmpres le digo al usuario que es lo que pasa ------------
        if( tmpres < 0)
        {
            perror("No se puede setear remote->sin_addr.s_addr");
            exit(1);
        }else if(tmpres == 0)
        {
            fprintf(stderr, "%s No es un dirección válida\n", ip2);
            exit(1);
        }
        //----------------------------
        
        
        // Lo mismo con el puerto
        remote->sin_port = htons(PORT);
        
        if(connect(sock, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0){
            perror("No se pudo conectar");
            exit(0);
        }
        
        //Acá le decimos al usuario cuál es su solicitud con el método build query
        get2 = build_get_query(host, page);
        fprintf(stderr,
                "Query is:\n<<SU REQUEST TIENE ESTOS DATOS>>\n%s<<FIN>>\n", get2);
        
        
        
        
        
        //Enviamos la solicitud al servidor:
        
        //Comenzamos a mandar el string de la query
        int sent = 0;
        
        while(sent < strlen(get2))
        {
            
            
            tmpres = send(sock, get2+sent, strlen(get2)-sent, 0);
            
            
            //Nuevamente revisamos algún error:
            if(tmpres == -1)
                
            {
                perror("No se puede mandar la solicitud");
                exit(1);
            }
            sent += tmpres;
        }
    
        
        memset(buf, 0, sizeof(buf));
        int htmlstart = 0;
        char * htmlcontent;
        char output[10000];
        strcpy(output,"");
        
        //importante
        
        
        while((tmpres = recv(sock, buf, BUFSIZ, 0)) > 0)
        {
            if(htmlstart == 0)
            {
                //Si lo ocupan así obtendrán solo el archivo
                //htmlcontent = strstr(buf, "\r\n\r\n");
                
                //Si lo ocupan así tendrán en HEAD+ El resto del archivo
                htmlcontent = strstr(buf, "");
                strcat(output,buf);
                
                if(htmlcontent != NULL)
                {
                    htmlstart = 1;
                    htmlcontent += 4;
                }
            }
            else
            {
                htmlcontent = buf;
                strcat (output,buf);
            }
            
            if(htmlstart)
            {
                //  printf(buf);
                fprintf(stdout, htmlcontent);
            }
            
            memset(buf, 0, tmpres);
        }
        
        
        if(tmpres < 0)
        {
            perror("Error al recibir el contenido");
        }
        
        //Se libera el socket (librería sockets)
        free(get2);
        free(remote);
        free(ip2);
        close(sock);
        
        
        
        
        

        
        
        
       
		//creamos el archivo con fname y contenido de request.
		FILE *f_request = fopen(fname, "wb");
		if (f_request == NULL)
		{
    			error("ERROR creando archivo");
		}	
		
		printf("se escribira en nuevo archivo\n");
		 char *text = output;
		fprintf(f_request, text);
		n = write(client_socket_descriptor,text,strlen(text));

	}
   
	
	}
	else{
	printf("IP NO permitido\n");
	n = write(client_socket_descriptor,"IP no fue aceptado",18);

	}


	// Si no se pudo escribir, enviamos un error
	if (n < 0) 
		error("ERROR writing to socket");

	// Cerramos los descriptores
	close(client_socket_descriptor);
	close(server_socket_descriptor);

	return 0; 
}
