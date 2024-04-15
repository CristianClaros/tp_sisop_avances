#include "conexion.h"

void crearServidor(t_log* logger, char* name_server, char* ip_server, char* puerto_server, int socket_server, void (*procesar_conexion)(t_procesar_conexion_args* args)){
    socket_server = iniciar_servidor(logger, name_server, ip_server, puerto_server);

    if (socket_server == -1) {
        log_error(logger, "FALLO AL CREAR EL SERVIDOR, CERRANDO %s", name_server);
    }

    server_escuchar(logger, name_server , socket_server,procesar_conexion);
//    while (server_escuchar(logger, name_server, socket_server));

}

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
    log_info(logger, "Escuchando en puerto: \n" /*ip_server, puerto_server, name_server*/);

    return socket_servidor;
}

int server_escuchar(t_log *logger, char* server_name, int socket_server, void (*procesar_conexion)(t_procesar_conexion_args* args)){
    int cliente_socket = esperar_cliente(logger, server_name, socket_server);

    t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
    args->log = logger;
    args->fd = cliente_socket;
    args->server_name = server_name;

    procesar_conexion(args);

//    if (cliente_socket != -1) {
//        pthread_t atenderProcesoNuevo;
//        t_procesar_conexion_args *args = malloc(sizeof(t_procesar_conexion_args));
//        args->log = logger;
//        args->fd = cliente_socket;
//        args->server_name = server_name;
//        pthread_create(&atenderProcesoNuevo, NULL,procesar_conexion,args);
//        pthread_detach(atenderProcesoNuevo);
//        return 1;
//    }
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
