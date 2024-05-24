#include "conexionMemoria.h"

#define IP_MEMORIA "127.0.0.1"
#define NAME_SERVER "SERVER MEMORIA"

int socket_memoria;
t_config_memoria* datos_memoria_config;

int iniciar_memoria(t_config_memoria* memoria_datos, t_log* logger_memoria){

	datos_memoria_config = memoria_datos;

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
        	//----------------------------------------MEMORIA----------------------
        	case CREAR_PROCESO:
        		//Crear la tabla de recursos necesarios para que el proceso este en memoria
        		int size;
        		char* buffer = recibir_buffer(&size, cliente_socket);
        		abrir_instrucciones(buffer);

        		enviar_mensaje("TABLA RECIBIDA", cliente_socket, HANDSHAKE);
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

void abrir_instrucciones(char* ruta) {
	FILE* archivo;
	char linea[100];
	char* cadena;
	char* token;
	t_list* lista;

	char* ruta_scrip = malloc(100 * sizeof(char));

	lista = list_create();

	strcpy(ruta_scrip, "");
	strcat(ruta_scrip, "../../../../../../..");
	strcat(ruta_scrip, datos_memoria_config->PATH_INSTRUCCIONES);
	strcat(ruta_scrip, ruta);

	archivo = fopen(ruta_scrip, "r");

	if(archivo == NULL){
		printf("Error Al Abrir El Archivo\n");
		exit(1);
	}else{
		while(feof(archivo) == 0){
			fgets(linea, 100, archivo);
			t_instruccion* instruccion = malloc(sizeof(t_instruccion));
			instruccion->instruccion = malloc(sizeof(char));

			cadena = strtok(linea, "\n");
			token = strtok(cadena, " ");
			instruccion->cant_parametros = cantidad_argumentos(token);
			instruccion->parametros = list_create();
			strcpy(instruccion->instruccion, token);


			for(int i=0;i<instruccion->cant_parametros;i++){
				token = strtok(NULL, " ");
				t_parametro* parametro = malloc(sizeof(t_parametro));
				parametro->cant_caracteres = strlen(token);
				parametro->parametro = malloc(sizeof(char));

				strcpy(parametro->parametro, token);

				list_add(instruccion->parametros, parametro);

			}

			list_add(lista, (void*)instruccion);

		}
	}

	list_iterate(lista, (void*)iterator_instruccion);

	fclose(archivo);

}

void iterator_instruccion(t_instruccion* instruccion){
	printf("(%s)", instruccion->instruccion);

	list_iterate(instruccion->parametros, (void*)iterator_parametro);

	printf("\n");

}

void iterator_parametro(t_parametro* param){
	printf("(%i)",param->cant_caracteres);
	printf("(%s)",param->parametro);
}

int cantidad_argumentos(char* token){
	if(strcmp(token, "EXIT") == 0){
			return 0;
		}
	if(strcmp(token, "RESIZE") == 0){
			return 1;
		}
	if(strcmp(token, "COPY_STRING") == 0){
			return 1;
		}
	if(strcmp(token, "WAIT") == 0){
			return 1;
		}
	if(strcmp(token, "SIGNAL") == 0){
			return 1;
		}
	if(strcmp(token, "SET") == 0){
			return 2;
		}
	if(strcmp(token, "MOV_IN") == 0){
			return 2;
		}
	if(strcmp(token, "MOV_OUT") == 0){
			return 2;
		}
	if(strcmp(token, "SUM") == 0){
			return 2;
		}
	if(strcmp(token, "SUB") == 0){
			return 2;
		}
	if(strcmp(token, "JNZ") == 0){
			return 2;
		}
	if(strcmp(token, "IO_GEN_SLEEP") == 0){
			return 2;
		}
	if(strcmp(token, "IO_FS_CREATE") == 0){
			return 2;
		}
	if(strcmp(token, "IO_FS_DELETE") == 0){
			return 2;
		}
	if(strcmp(token, "IO_STDIN_READ") == 0){
			return 3;
		}
	if(strcmp(token, "IO_STDOUT_WRITE") == 0){
			return 3;
		}
	if(strcmp(token, "IO_FS_TRUNCATE") == 0){
			return 3;
		}
	if(strcmp(token, "IO_FS_WRITE") == 0){
			return 5;
		}
	if(strcmp(token, "IO_FS_READ") == 0){
			return 5;
		}
	return -1;
}
