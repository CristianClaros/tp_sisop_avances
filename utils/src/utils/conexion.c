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
    log_info(logger, "Escuchando en puerto: %s, (%s), %s\n", ip_server, puerto_server, name_server);

    return socket_servidor;
}

int server_escuchar(t_log *logger, char* server_name, int socket_server, void (*procesar_conexion)(t_procesar_conexion_args* args)){
    int cliente_socket = esperar_cliente(logger, server_name, socket_server);

    t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
    args->log = logger;
    args->fd = cliente_socket;
    args->server_name = server_name;

    procesar_conexion(args);
//Aqui hay que implementar hilos
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

//Switch de espera del servidor
void procesar_conexion(t_procesar_conexion_args* void_args){

    t_log* logger = void_args->log;
    int cliente_socket = void_args->fd;
    char *server_name = void_args->server_name;

    op_code cop;
    while (cliente_socket != -1) {

        if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_info(logger, "DISCONNECT!");
            return;
        }

        switch (cop) {

        	//---------------------------------------- BASICAS -----------------------
    		case HANDSHAKE:
    			//Es el saludo del modulo para saber si se conecto correctamente
    			return;
            //---------------------------------------- KERNEL -----------------------
    			// hay varias acciones que deben esperar del la consola
        	case INICIAR_PROCESO:
        		//Recibe la orden de la consola para iniciar proceso
        		return;
        	case FINALIZAR_PROCESO:
        		//Recibe la orden de la consola para finalizar proceso
        		return;
        	case INICIAR_PLANIFICACION:
        		//Recibe la orden de la consola para iniciar planificacion
        		return;
        	case DETENER_PLANIFICACION:
        		//Recibe la orden de la consola para detener planificacion
        		return;
        	case EJECUTAR_SCRIP_OPERACIONES:
        		//Recibe la orden de la consola para ejecutar scrips
        		return;
        	case MODIFICAR_GRADO_MULTIPROGRAMACION:
        		//Recibe la orden de la consola para modificar grado de programacion
        		return;
        	case LISTAR_PROCESOS_ESTADO:
        		//Recibe la orden de la consola para lsitar procesos
        	case IDENTIFICAR_INTERFAZ:
        		//Identifica la interfaz
        		return;
        	case RECIBIR_PROCESO_DESALOJADO:
        		//Recibe PCB desde el CPU
        		return;
        	case FINALIZAR_PROCESO_SOLICITUD:
        		//Recibe PCB para finalizar por solicitud de planificacion
        		return;
        	case FINALIZAR_PROCESO_ERROR:
        		//Recibe PCB para finalizar por error de memoria
        		return;
        	case FINALIZAR_PROCESO_CONSOLA:
        		//Recibe PCB para finalizar por solicitud de consola
        		return;
        	case SOLICITUD_RECURSO:
        		//El cpu solicita recursos de i/o
        		return;
        	case LIBERAR_RECURSO:
        		//El cpu solicita liberar recurso de i/o
        		return;
        	case SLEEP_INTERFAZ:
        		//El cpu prdena al kernel SLEEP en interfaz
        		return;
        	case STDIN_INTERFAZ:
        		//El cpu ordena al kernel que tome datos de entrada
        		return;
        	case STDOUT_INTERFAZ:
        		//El cpu ordena al kernel que envie datos de salida
        		return;
        	case CREATE_FS_INTERFAZ:
        		//El cpu ordena al kernel crear un archivo de fs por interfaz
        		return;
        	case DELETE_FS_INTERFAZ:
        		//El cpu ordena al kernel borrar un archivo de fs por interfaz
        		return;
        	case TRUNCATE_FS_INTERFAZ:
        		//El cpu ordena al kernel modificar un archivo de fs por interfaz
        		return;
        	case READ_FS_INTERFAZ:
        		//El cpu ordena al kernel leer un archivo de fs por interfaz
        		return;
        	case WRITE_FS_INTERFAZ:
        		//El cpu ordena al kernel escribir un archivo de fs por interfaz
        		return;

            //---------------------------------------- CPU -----------------------
        	case EJECUTAR_PROCESO:
        		//Recibe PCB para ejecutar
        		return;
        	case RECIBIR_DIRECCION_FISICA:
        		//Recibe la direccion logica transformada a fisica por la memoria
        		return;
        	case RECIBIR_INSTRUCCION:
        		//Recibe la instruccion a ejecutar
        		return;
        	//----------------------------------------MEMORIA----------------------
        	case CREAR_PROCESO:
        		//Crear la tabla de recursos necesarios para que el proceso este en memoria
        		return;
        	case LIBERAR_PROCESO:
        		//Libera los recursos creados para el proceso en la memoria
        		return;
        	case TRADUCIR_DIRECCION_LOGICA:
        		//Traduce las direccion logica a fisica que solicita el CPU
        		return;
        	case PEDIR_INSTRUCCION:
        		//Recibe la orden de enviar la instruccion a ejcutar que solicto el CPU
        		return;
        	case AJUSTAR_TAMANIO_PROCESO:
        		//Modifica el tamaño de un proceso solicitado por CPU
        		return;
        	case OBTENER_FRAME:
        		//Espera solicut de FRAME por el CPU
        		return;
        	case LEER_DIRECCION_FISICA:
        		//Lee una direccion fisica pedida por el CPU
        		return;
        	//----------------------------------------INTERFAZ----------------------
        	case IO_GEN_SLEEP:
        		//Recibe orden del Kernel para SLEEP
        		return;
        	case IO_STDIN_READ:
        		//Recibe orden del Kernel para leer por interfaz
        		return;
        	case IO_STDOUT_WRITE:
        		//Recibe orden del Kernel para mostrar por interfaz
        		return;
        	case IO_FS_CREATE:
        		//Recibe orden del Kernel para crear un FS
        		return;
        	case IO_FS_DELETE:
        		//Recibe orden del Kernel para borra un FS
        		return;
        	case IO_FS_TRUNCATE:
        		//Recibe orden del Kernel para truncar un FS
        		return;
        	case IO_FS_WRITE:
        		///Recibe orden del Kernel para escribir en un FS
        		return;
        	case IO_FS_READ:
        		///Recibe orden del Kernel para leer en un FS
        		return;
        	// --------------------- DESCONEXION -------------------------
            case DESCONEXION:
            	//ERROR DE DESCONEXION
                log_error(logger, "Cliente desconectado de %s...", server_name);
                return;
            default:
                log_error(logger, "Algo anduvo mal en el server de %s", server_name);
                log_info(logger, "Cop: %d", cop);
                return;
        }
    }

    log_warning(logger, "El cliente se desconecto de %s server", server_name);
    return;
}
