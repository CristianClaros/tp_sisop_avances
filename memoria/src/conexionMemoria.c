#include "conexionMemoria.h"

#define IP_MEMORIA "127.0.0.1"
#define NAME_SERVER "SERVER MEMORIA"

int socket_memoria;
t_config_memoria* datos_memoria_config;
t_list* pid_instrucciones;

int iniciar_memoria(t_config_memoria* memoria_datos, t_log* logger_memoria){

	datos_memoria_config = memoria_datos;
	pid_instrucciones = list_create();

	socket_memoria = iniciar_servidor(logger_memoria, NAME_SERVER, IP_MEMORIA, memoria_datos->PUERTO_ESCUCHA);

	while(server_escuchar(logger_memoria, NAME_SERVER, socket_memoria, procesar_conexion_memoria));

	return 1;
}

void* procesar_conexion_memoria(void* void_args){

    t_procesar_conexion_args *args = (t_procesar_conexion_args *) void_args;
    t_log *logger = args->log;
    int cliente_socket = args->fd;
    char *server_name = args->server_name;
    free(args);

    uint32_t size;
    op_code cop;
    char* buffer;

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
        	//----------------------------------------MEMORIA----------------------
        	case CREAR_PROCESO:
        		//Crear la tabla de recursos necesarios para que el proceso este en memoria
        		buffer = recibir_buffer(&size, cliente_socket);
        		int desplazamiento = 0;

        		t_pid_instruccion* pid_instruccion = malloc(sizeof(t_pid_instruccion));
        		pid_instruccion->ruta_instrucciones = malloc(sizeof(char));

        		pid_instruccion->pid = recibir_int(buffer, &desplazamiento);
        		pid_instruccion->ruta_instrucciones = recibir_string(buffer, &desplazamiento);

        		list_add(pid_instrucciones, pid_instruccion);

        		op_code code = OK;
        		send(cliente_socket,&code, sizeof(op_code),0);

        		break;
        	case LIBERAR_PROCESO:
        		//Libera los recursos creados para el proceso en la memoria
        		break;
        	case TRADUCIR_DIRECCION_LOGICA:
        		//Traduce las direccion logica a fisica que solicita el CPU
        		break;
        	case PEDIR_INSTRUCCION:
        		//Recibe la orden de enviar la instruccion a ejcutar que solicto el CPU
        		buffer = recibir_buffer(&size, cliente_socket);

        		atender_peticion_instruccion(buffer, cliente_socket);
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
        	case GUARDAR_DIRECCION_LOGICA:
        		//Escribe en un lugar de la memoria
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

void iterator_pid_instruccion(t_pcb_instrucciones* pcb_instrucciones){
	printf("PID(%i)\n",pcb_instrucciones->pid);
	list_iterate(pcb_instrucciones->instrucciones, (void*) iterator_instruccion);
}


//Peticion de instruccion

void atender_peticion_instruccion(void* buffer, int cliente_socket){
	int desplazamiento = 0;

	int pidBusqueda = recibir_int(buffer, &desplazamiento);
	int program_counter = recibir_int(buffer, &desplazamiento);

	t_pid_instruccion* data = malloc(sizeof(t_pid_instruccion));
	data = (t_pid_instruccion*) list_get(pid_instrucciones, pidBusqueda);

	char* instruccion = obtenerInstruccion(data->ruta_instrucciones, program_counter);
	usleep((datos_memoria_config->RETARDO_RESPUESTA)*1000); // Duerme el proceso por RETARDO y luego continua

	t_paquete* paquete = crear_paquete(OK);
	agregar_a_paquete(paquete, instruccion, strlen(instruccion)+1);
	enviar_paquete(paquete, cliente_socket);

	eliminar_paquete(paquete);
}

char* obtenerInstruccion(char* pathInstrucciones, int program_counter){
    FILE* file;

	char* ruta_scrip = malloc(100 * sizeof(char));

	strcpy(ruta_scrip, "");
	strcat(ruta_scrip, "../../../../..");
	strcat(ruta_scrip, datos_memoria_config->PATH_INSTRUCCIONES);
	strcat(ruta_scrip, pathInstrucciones);


    file = fopen(ruta_scrip, "rt");
    int numLineaActual = 0;
    char lineaActual[1000];

    char* instruccion = malloc(sizeof(char*));

    while(feof(file) == 0){
    	fgets(lineaActual, 1000, file);
    	if(numLineaActual == program_counter){
    		instruccion = strtok(lineaActual, "\n");
    		break;
    	}
    	numLineaActual++;
    }
    fclose(file);
    return instruccion;
}
