#include "conexion.h"

//Switch de espera del servidor
void* procesar_conexion(void* void_args){

    t_procesar_conexion_args *args = (t_procesar_conexion_args *) void_args;
    t_log *logger = args->log;
    int cliente_socket = args->fd;
    char *server_name = args->server_name;
    free(args);

    op_code cop;
    while (cliente_socket != -1) {

        if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_info(logger, "DISCONNECT!");
            return NULL;
        }
        switch (cop) {

        	//---------------------------------------- BASICAS -----------------------
    		case HANDSHAKE:
    			//Es el saludo del modulo para saber si se conecto correctamente
    			recibir_mensaje(cliente_socket);
    			break;
            //---------------------------------------- KERNEL -----------------------
    			// hay varias acciones que deben esperar del la consola
        	case INICIAR_PROCESO:
        		//Recibe la orden de la consola para iniciar proceso
        		recibir_mensaje(cliente_socket);
        		printf("INICIO_PROCESO\n");
        		break;
        	case FINALIZAR_PROCESO:
        		//Recibe la orden de la consola para finalizar proceso
        		break;
        	case INICIAR_PLANIFICACION:
        		//Recibe la orden de la consola para iniciar planificacion
        		break;
        	case DETENER_PLANIFICACION:
        		//Recibe la orden de la consola para detener planificacion
        		break;
        	case EJECUTAR_SCRIP_OPERACIONES:
        		//Recibe la orden de la consola para ejecutar scrips
        		break;
        	case MODIFICAR_GRADO_MULTIPROGRAMACION:
        		//Recibe la orden de la consola para modificar grado de programacion
        		break;
        	case LISTAR_PROCESOS_ESTADO:
        		//Recibe la orden de la consola para lsitar procesos
        		break;
        	case IDENTIFICAR_INTERFAZ:
        		//Identifica la interfaz
        		break;
        	case RECIBIR_PROCESO_DESALOJADO:
        		//Recibe PCB desde el CPU
        		break;
        	case FINALIZAR_PROCESO_SOLICITUD:
        		//Recibe PCB para finalizar por solicitud de planificacion
        		break;
        	case FINALIZAR_PROCESO_ERROR:
        		//Recibe PCB para finalizar por error de memoria
        		break;
        	case FINALIZAR_PROCESO_CONSOLA:
        		//Recibe PCB para finalizar por solicitud de consola
        		break;
        	case SOLICITUD_RECURSO:
        		//El cpu solicita recursos de i/o
        		break;
        	case LIBERAR_RECURSO:
        		//El cpu solicita liberar recurso de i/o
        		break;
        	case SLEEP_INTERFAZ:
        		//El cpu prdena al kernel SLEEP en interfaz
        		break;
        	case STDIN_INTERFAZ:
        		//El cpu ordena al kernel que tome datos de entrada
        		break;
        	case STDOUT_INTERFAZ:
        		//El cpu ordena al kernel que envie datos de salida
        		break;
        	case CREATE_FS_INTERFAZ:
        		//El cpu ordena al kernel crear un archivo de fs por interfaz
        		break;
        	case DELETE_FS_INTERFAZ:
        		//El cpu ordena al kernel borrar un archivo de fs por interfaz
        		break;
        	case TRUNCATE_FS_INTERFAZ:
        		//El cpu ordena al kernel modificar un archivo de fs por interfaz
        		break;
        	case READ_FS_INTERFAZ:
        		//El cpu ordena al kernel leer un archivo de fs por interfaz
        		break;
        	case WRITE_FS_INTERFAZ:
        		//El cpu ordena al kernel escribir un archivo de fs por interfaz
        		break;
            //---------------------------------------- CPU -----------------------
        	case EJECUTAR_PROCESO:
        		//Recibe PCB para ejecutar
        		break;
        	case RECIBIR_DIRECCION_FISICA:
        		//Recibe la direccion logica transformada a fisica por la memoria
        		break;
        	case RECIBIR_INSTRUCCION:
        		//Recibe la instruccion a ejecutar
        		break;
        	//----------------------------------------MEMORIA----------------------
        	case CREAR_PROCESO:
        		//Crear la tabla de recursos necesarios para que el proceso este en memoria
        		recibir_mensaje(cliente_socket);
        		break;
        	case LIBERAR_PROCESO:
        		//Libera los recursos creados para el proceso en la memoria
        		break;
        	case TRADUCIR_DIRECCION_LOGICA:
        		//Traduce las direccion logica a fisica que solicita el CPU
        		break;
        	case PEDIR_INSTRUCCION:
        		//Recibe la orden de enviar la instruccion a ejcutar que solicto el CPU
        		break;
        	case AJUSTAR_TAMANIO_PROCESO:
        		//Modifica el tamaño de un proceso solicitado por CPU
        		break;
        	case OBTENER_FRAME:
        		//Espera solicut de FRAME por el CPU
        		break;
        	case LEER_DIRECCION_FISICA:
        		//Lee una direccion fisica pedida por el CPU
        		break;
        	//----------------------------------------INTERFAZ----------------------
        	case IO_GEN_SLEEP:
        		//Recibe orden del Kernel para SLEEP
        		break;
        	case IO_STDIN_READ:
        		//Recibe orden del Kernel para leer por interfaz
        		break;
        	case IO_STDOUT_WRITE:
        		//Recibe orden del Kernel para mostrar por interfaz
        		break;
        	case IO_FS_CREATE:
        		//Recibe orden del Kernel para crear un FS
        		break;
        	case IO_FS_DELETE:
        		//Recibe orden del Kernel para borra un FS
        		break;
        	case IO_FS_TRUNCATE:
        		//Recibe orden del Kernel para truncar un FS
        		break;
        	case IO_FS_WRITE:
        		///Recibe orden del Kernel para escribir en un FS
        		break;
        	case IO_FS_READ:
        		///Recibe orden del Kernel para leer en un FS
        		break;
        	// --------------------- DESCONEXION -------------------------
            case DESCONEXION:
            	//ERROR DE DESCONEXION
                log_error(logger, "Cliente desconectado de %s...", server_name);
                break;
            default:
                log_error(logger, "Algo anduvo mal en el server de %s", server_name);
                log_info(logger, "Cop: %d", cop);
                break;
        }
    }

    log_warning(logger, "El cliente se desconecto de %s server", server_name);
    return NULL;
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
    log_info(logger, "Escuchando IP: %s, en puerto: (%s), %s\n", ip_server, puerto_server, name_server);

    return socket_servidor;
}

int server_escuchar(t_log *logger, char *server_name, int server_socket) {
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

