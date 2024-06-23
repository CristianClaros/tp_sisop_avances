#include "conexionEntradaSalida.h"

#define IP_INTERFAZ "127.0.0.1"
#define PUERTO_INTERFAZ "8008"
#define NAME_SERVER "SERVER INTERFAZ"

int socket_interfaz;
int socket_memoria;
int socket_kernel;
t_config_entradasalida* datos_entradaSalida;

int iniciar_interfaz(t_config_entradasalida* entradasalida_datos, t_log* logger_entradasalida){
	datos_entradaSalida = entradasalida_datos;

	socket_interfaz = iniciar_servidor(logger_entradasalida, NAME_SERVER, IP_INTERFAZ, PUERTO_INTERFAZ);

	socket_memoria = crear_conexion(logger_entradasalida, NAME_SERVER, entradasalida_datos->IP_MEMORIA, entradasalida_datos->PUERTO_MEMORIA);
	enviar_mensaje(entradasalida_datos->TIPO_INTERFAZ, socket_memoria, HANDSHAKE);

	socket_kernel = crear_conexion(logger_entradasalida, NAME_SERVER, entradasalida_datos->IP_KERNEL, entradasalida_datos->PUERTO_KERNEL);

	//Esto le envia a kernel los datos para guardar su interfaz
	t_paquete* paquete = crear_paquete(IDENTIFICAR_INTERFAZ);
	agregar_a_paquete(paquete, entradasalida_datos->NOMBRE, sizeof(entradasalida_datos->NOMBRE)+1);
	agregar_a_paquete(paquete, entradasalida_datos->TIPO_INTERFAZ, strlen(entradasalida_datos->TIPO_INTERFAZ)+1);
	enviar_paquete(paquete, socket_kernel);
	eliminar_paquete(paquete);

//	while(server_escuchar(logger_entradasalida, NAME_SERVER, socket_interfaz, procesar_conexion_entradasalida));

    while (1){
        op_code cop;
        void* buffer;
        uint32_t size;
        int desplazamiento;

        if (recv(socket_kernel, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_info(logger_entradasalida, "DISCONNECT!");
            return -1;
        }
        switch (cop) {
        	//---------------------------------------- BASICAS -----------------------
    		case HANDSHAKE:
    			//Es el saludo del modulo para saber si se conecto correctamente
    			recibir_mensaje(socket_kernel);
    			break;
        	//----------------------------------------INTERFAZ----------------------
            case IO_GEN_SLEEP:
            	//Recibe orden del Kernel para SLEEP
            	buffer = recibir_buffer(&size, socket_kernel);
            	desplazamiento = 0;
            	int pid_generica = recibir_int(buffer, &desplazamiento);
            	int valor = recibir_int(buffer, &desplazamiento);

            	ejecutar_interfaz_generica(valor, pid_generica, logger_entradasalida);
            	//Aca deberia enviar algo al Kernel??????

            	break;
            case IO_STDIN_READ:
            	//Recibe orden del Kernel para leer por interfaz
            	buffer = recibir_buffer(&size, socket_kernel);
            	//En estas funciones vamos a necesitar saber que tipo se manda uint_8, etc
            	desplazamiento = 0;
            	int pid_teclado = recibir_int(buffer, &desplazamiento);
            	int registro_teclado = recibir_int(buffer, &desplazamiento);
            	int tamanio_teclado = recibir_int(buffer, &desplazamiento);

            	ejecutar_interfaz_teclado(pid_teclado, registro_teclado, tamanio_teclado, logger_entradasalida);
            	break;
            case IO_STDOUT_WRITE:
            	//Recibe orden del Kernel para mostrar por interfaz
            	buffer = recibir_buffer(&size, socket_kernel);
            	desplazamiento = 0;
            	//En estas funciones vamos a necesitar saber que tipo se manda uint_8, etc
            	int pid_monitor = recibir_int(buffer, &desplazamiento);
            	int registro_monitor = recibir_int(buffer, &desplazamiento);
            	int tamanio_monitor = recibir_int(buffer, &desplazamiento);

            	ejecutar_interfaz_monitor(pid_monitor, registro_monitor, tamanio_monitor,logger_entradasalida);
            	break;
            //Funciones de DIAL_FS
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
        	// --------------------- DESCONEXION ------------------------
        	default:
        		break;
        }
    }

	return 1;
}

void ejecutar_interfaz_generica(int valor, int pid, t_log* logger){
	int tiempo_espera = valor * ((datos_entradaSalida->TIEMPO_UNIDAD_TRABAJO)*1000);
	log_info(logger, "PID: <%i> - Operacion: <%s>", pid, datos_entradaSalida->TIPO_INTERFAZ); //Operacion deberia tomarse parametro
	usleep(tiempo_espera);
}

void ejecutar_interfaz_teclado(int pid, int registro, int tamanio, t_log* logger){
	char* teclado;
	printf("Ingrese un texto: ");
	scanf("%s", teclado);

	log_info(logger, "PID: <%i> - Operacion: <%s>", pid, datos_entradaSalida->TIPO_INTERFAZ); //Operacion deberia tomarse parametro

	t_paquete* paquete = crear_paquete(GUARDAR_DIRECCION_LOGICA);
	agregar_a_paquete(paquete, teclado, strlen(teclado)+1);
	agregar_a_paquete_int(paquete, &registro, sizeof(int));
	agregar_a_paquete_int(paquete, &tamanio, sizeof(int));

	enviar_paquete(paquete, socket_memoria);
	eliminar_paquete(paquete);
}

void ejecutar_interfaz_monitor(int pid, int registro, int tamanio, t_log* logger){
//	uint32_t size;
//	int desplazamiento = 0;

	log_info(logger, "PID: <%i> - Operacion: <IO_STDOUT_WRITE>", pid); //Operacion deberia tomarse parametro

	t_paquete* paquete = crear_paquete(LEER_DIRECCION_FISICA);
	agregar_a_paquete_int(paquete, &registro, sizeof(int));
	agregar_a_paquete_int(paquete, &tamanio, sizeof(int));

	enviar_paquete(paquete, socket_memoria);
	eliminar_paquete(paquete);

//	//Aca espera el contenido de la direccion de memoria
//	t_buffer* buffer = recibir_buffer(&size, socket_memoria);
//	char* contenido = recibir_string(buffer, &desplazamiento);

//	printf("Contenido de direccion de memoria (%s)", contenido); //Esto solo es de ayuda
}


// Procesar conexion entradasalida
void* procesar_conexion_entradasalida(void* void_args){

    t_procesar_conexion_args *args = (t_procesar_conexion_args *) void_args;
    t_log *logger = args->log;
    int cliente_socket = args->fd;
    char *server_name = args->server_name;
    free(args);

    op_code cop;
    void* buffer;
    uint32_t size;
    int desplazamiento;
    while (cliente_socket != -1) {
    	printf("ALGO LLEGO");
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
        	//----------------------------------------INTERFAZ----------------------
            case IO_GEN_SLEEP:
            	//Recibe orden del Kernel para SLEEP
            	buffer = recibir_buffer(&size, cliente_socket);
            	desplazamiento = 0;
            	int pid_generica = recibir_int(buffer, &desplazamiento);
            	int valor = recibir_int(buffer, &desplazamiento);

            	ejecutar_interfaz_generica(valor, pid_generica, logger);
            	printf("INTERFAZ EJECUTADA\n");
            	//Aca deberia enviar algo al Kernel??????

            	break;
            case IO_STDIN_READ:
            	//Recibe orden del Kernel para leer por interfaz
            	buffer = recibir_buffer(&size, cliente_socket);
            	//En estas funciones vamos a necesitar saber que tipo se manda uint_8, etc
            	desplazamiento = 0;
            	int pid_teclado = recibir_int(buffer, &desplazamiento);
            	int registro_teclado = recibir_int(buffer, &desplazamiento);
            	int tamanio_teclado = recibir_int(buffer, &desplazamiento);

            	ejecutar_interfaz_teclado(pid_teclado, registro_teclado, tamanio_teclado, logger);
            	printf("INTERFAZ EJECUTADA\n");
            	break;
            case IO_STDOUT_WRITE:
            	//Recibe orden del Kernel para mostrar por interfaz
            	buffer = recibir_buffer(&size, cliente_socket);
            	desplazamiento = 0;
            	//En estas funciones vamos a necesitar saber que tipo se manda uint_8, etc
            	int pid_monitor = recibir_int(buffer, &desplazamiento);
            	int registro_monitor = recibir_int(buffer, &desplazamiento);
            	int tamanio_monitor = recibir_int(buffer, &desplazamiento);

            	ejecutar_interfaz_monitor(pid_monitor, registro_monitor, tamanio_monitor,logger);
            	printf("INTERFAZ EJECUTADA\n");
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
