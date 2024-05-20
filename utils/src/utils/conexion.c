#include "conexion.h"

//Switch de espera del servidor

// INICIA SERVER ESCUCHANDO EN IP:PUERTO
int iniciar_servidor(t_log* logger, const char* name_server, char* ip_server, char* puerto_server){
    int socket_servidor;
    struct addrinfo hints, *servinfo;

    // Inicializando hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Recibe los addrinfo
    getaddrinfo(ip_server, puerto_server, &hints, &servinfo);

    //Creo un socket servidor
    socket_servidor = 0;

    socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    if(socket_servidor == -1){
    	perror("NO PUEDE CREARSE EL SERVIDOR!!!\n");
    }

    if(bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) == -1){
    	perror("ERROR EN EL BIND!!!\n");
    }

    //Asociamos y escuhamos atraves del puerto
    if(listen(socket_servidor,SOMAXCONN) == -1){
    	perror("ERROR EN EL LISTEN!!!\n");
    }

    freeaddrinfo(servinfo);

    // Aviso al logger
    log_info(logger, "Escuchando IP: %s, en puerto: (%s), %s\n", ip_server, puerto_server, name_server);

    return socket_servidor;
}

int server_escuchar(t_log *logger, char *server_name, int server_socket, void* procesar_conexion(void* args)) {
    int cliente_socket = esperar_cliente(logger, server_name, server_socket);

    t_procesar_conexion_args *args = malloc(sizeof(t_procesar_conexion_args));
    args->log = logger;
    args->fd = cliente_socket;
    args->server_name = server_name;

    if (cliente_socket != -1) {
    	if(strcmp(server_name,"SERVER_CPU") == 0 || strcmp(server_name,"SERVER_INTERFAZ") == 0){
    		procesar_conexion(args);
    	}else{
    		pthread_t atenderProcesoNuevo;
    		pthread_create(&atenderProcesoNuevo, NULL, procesar_conexion, args);
    		pthread_detach(atenderProcesoNuevo);
    	}
        return 1;
    }
    //implementar hilos
    return 0;
}

// ESPERAR CONEXION DE CLIENTE EN UN SERVER ABIERTO
int esperar_cliente(t_log* logger, const char* name, int socket_servidor){
    int socket_cliente;

    if((socket_cliente = accept(socket_servidor, NULL, NULL)) == -1){
    	perror("ERROR EN EL ACCEPT!!!\n");
    	return -1;
    }

    log_info(logger, "Cliente conectado (a %s)\n", name);

    return socket_cliente;
}

//Clientes
int crear_conexion(t_log* logger, const char* server_name, char* ip, char* puerto){
    struct addrinfo hints, *servinfo;

    // Init de hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Recibe addrinfo
    getaddrinfo(ip, puerto, &hints, &servinfo);

    // Crea un socket con la informacion recibida (del primero, suficiente)
    int socket_cliente = 0;

    // Fallo en crear el socket
    if((socket_cliente = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
        log_info(logger, "ERROR AL CREAR EL SOCKET CLIENTE!!!\n");
        return -1;
    }

    // Error conectando
    if(connect(socket_cliente, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        log_error(logger, "Error al conectar (a %s)\n", server_name);
        return 0;
    } else
        log_info(logger, "Cliente conectado en %s:%s (a %s)\n", ip, puerto, server_name);

    freeaddrinfo(servinfo);

    return socket_cliente;
}

//Funciones para recibir mensaje de modulos conectados
int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	printf("SE CONECTO UN MODULO DE  %s\n", buffer);
	free(buffer);
}

//Envio de msj para decir que modulo es
void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void enviar_mensaje(char* mensaje, int socket_cliente, int codigo_protocolo){
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = codigo_protocolo;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(int codigo_protocolo)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo_protocolo;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

