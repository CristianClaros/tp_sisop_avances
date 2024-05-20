#include "conexionKernel.h"

#define IP_KERNEL "127.0.0.1"
#define NAME_SERVER "SERVER KERNEL"
#define PUERTO_KERNEL "8003"

t_config_kernel* datos_kernel_config;
t_log* log_kernel;

int socket_kernel;
int socket_dispatch;
int socket_interrupt;
int socket_memoria;

//VARIABLE COLAS Y LISTAS
t_list* cola_ready;
t_list* cola_listo_ready;
t_list* cola_blockeado;
t_list* cola_exit;

//LISTA ARCHIVOS ABIERTOS Y RECURSOS
t_list* archivos_abiertos;
t_list* recursos_disponibles;

typedef struct{
	char* recursos;
	int instancias;
}t_recursos;

//PIDs
int pid_list = 0;
int pid = 0;

//SEMAFOROS
sem_t ready;

int pid_busqueda = 0;

int iniciar_kernel(t_config_kernel* kernel_datos, t_log* logger_kernel){

	datos_kernel_config = kernel_datos;
	log_kernel = logger_kernel;

	sem_init(&ready, 0, datos_kernel_config->GRADO_MULTIPROGRAMACION);

	socket_kernel = iniciar_servidor(logger_kernel, NAME_SERVER, IP_KERNEL, PUERTO_KERNEL);

	socket_interrupt = crear_conexion(logger_kernel, IP_KERNEL, kernel_datos->IP_CPU, kernel_datos->PUERTO_CPU_INTERRUPT);
	enviar_mensaje("KERNEL-INTERRUPT", socket_interrupt, HANDSHAKE);

	socket_dispatch = crear_conexion(logger_kernel, IP_KERNEL, kernel_datos->IP_CPU, kernel_datos->PUERTO_CPU_DISPATCH);
	enviar_mensaje("KERNEL-DISPATCH", socket_dispatch, HANDSHAKE);

	socket_memoria = crear_conexion(logger_kernel, IP_KERNEL, kernel_datos->IP_MEMORIA, kernel_datos->PUERTO_MEMORIA);
	enviar_mensaje("KERNEL", socket_memoria, HANDSHAKE);

	inicializar_variables();

	while(server_escuchar(logger_kernel, NAME_SERVER, socket_kernel, procesar_conexion_kernel) && iniciar_consola());


	return 1;
}

//INICIO DE COLAS, LISTAS, SEMAFOROS

void inicializar_variables(){
	cola_ready = list_create();
	cola_listo_ready = list_create();
	cola_blockeado = list_create();
	cola_exit = list_create();

	recursos_disponibles = iniciar_recursos();
}

t_list* iniciar_recursos(){
	t_list* recursos = list_create();
	int cantidad_recursos = string_array_size(datos_kernel_config->INSTANCIAS_RECURSOS);
	for(int i = 0;i < cantidad_recursos;i++){
		t_recursos* recurso = malloc(sizeof(t_recursos));

		recurso->recursos = malloc(sizeof(char*));

		strcpy(recurso->recursos, datos_kernel_config->RECURSOS[i]);
		recurso->instancias = atoi(datos_kernel_config->INSTANCIAS_RECURSOS[i]);

		list_add(recursos, recurso);
	}
	return recursos;
}

//PROCESAR CONEXION
void* procesar_conexion_kernel(void* void_args){

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



//CONSOLA INTERACTIVA

int iniciar_consola(){
    char *linea;

    printf("CONSOLA INTERACTIVA INICIADA .....\n");

    while (1){
        linea = readline(">");

        if (!strncmp(linea, "exit", 4)) {
            free(linea);
            break;
        }

        if (linea && strcmp(linea,"") != 0) {
            add_history(linea);

            analizar_comando(linea);
        }
        free(linea);
    }
    return 0;
}


void analizar_comando(char* linea){
    char* token;
    char* flag = " ";

    token = strtok(linea,flag);

    if(strcmp(token, "EJECUTAR_SCRIP") == 0)
        printf("VOY A EJECUTAR_SCRIP!!!\n"); //NO ENTIENDO BIEN QUE HACE ACA
    if(strcmp(token, "INICIAR_PROCESO") == 0){
    	token = strtok(NULL," ");
    	//ACA SE CREA LA FUNCION DE CREAR_PROCESO

    	pthread_t crear_proceso;
    	pthread_create(&crear_proceso, NULL, (void*)iniciar_proceso, (void*)token);
    	pthread_detach(crear_proceso);

    }
    if(strcmp(token, "FINALIZAR_PROCESO") == 0){
    	token = strtok(NULL, " ");
    	pid_busqueda = atoi(token);
    	t_proceso* process_exit = (t_proceso*)list_find(cola_ready, (void*)buscar_pid);
    	int pid_encontrado;
    	pid_encontrado = process_exit->pid_list;

    	t_proceso* proceso_exit = list_remove(cola_ready, pid_encontrado);

    	sem_post(&ready);
    	log_info(log_kernel, "PID: <%i> - Estado Anterior: <%s> - Estado Actual: <EXIT>", proceso_exit->pcb->pid, proceso_exit->estado);
    	cambiar_estado(proceso_exit, "EXIT");

    	list_add(cola_exit, proceso_exit);

    }
    if(strcmp(token, "DETENER_PLANIFICACION") == 0)
        printf("VOY A DETENER PLANIFICACION!!!\n"); //ACA VA A DETENER MEDIANTE UN SEMAFORO
    if(strcmp(token, "INICIAR_PLANIFICACION") == 0)
        printf("VOY A INICIAR_PLANIFICACION!!!\n"); //LO MISMO MEDIANTE UN SEMAFORO
    if(strcmp(token, "MULTIPROGRAMACION") == 0){
    	token = strtok(NULL, " ");
    	int diferencia = datos_kernel_config->GRADO_MULTIPROGRAMACION - atoi(token);
    	for(int i=0;i < abs(diferencia); i++){
    		if(diferencia > 0){
    			sem_wait(&ready);
    		}else{
    			sem_post(&ready);
    		}
    	}
    }
    if(strcmp(token, "PROCESO_ESTADO") == 0){
        printf("LISTA DE PROCESOS!!!\n");
    	list_iterate(cola_ready, (void*) iterator);
    	list_iterate(cola_exit, (void*) iterator);
    }
}

void iterator(t_proceso* proceso){
	printf("PID(%i), ESTADO(%s), POSICION: (%i)\n",proceso->pcb->pid, proceso->estado, proceso->pid_list);
}

//CAmbiar estado
void* cambiar_estado(t_proceso* proceso, char* estado){
	proceso->estado = estado;
	return NULL;
}

//INICIAR PROCESO
void* iniciar_proceso(char* ruta){
	t_proceso* proceso = malloc(sizeof(t_proceso));

	proceso->estado = malloc(sizeof(char));
	proceso->pcb = malloc(sizeof(t_pcb));

	cambiar_estado(proceso, "NEW");
	proceso->pcb->pid = pid;
	proceso->pid_list = pid_list;

	proceso->pcb->program_counter = 0;
	proceso->pcb->quantum = datos_kernel_config->QUANTUM;
	printf("CREE PCB");
	//ESTA ES UNA FUNCION DE RECIBIR TABLA
	op_code cop; //ACA VERIFICA SI PUDO ABRIR LA RUTA
	recv(socket_memoria,&(cop),sizeof(op_code), 0);
	recibir_mensaje(socket_memoria);
	log_info(log_kernel, "Se crea el proceso <%i> en NEW", proceso->pcb->pid);

	sem_wait(&ready);
	log_info(log_kernel, "PID: <%i> - Estado Anterior: <%s> - Estado Actual: <READY>", proceso->pcb->pid, proceso->estado);
	cambiar_estado(proceso, "READY");

	list_add(cola_ready, proceso);
	log_info(log_kernel, "Cola Ready <COLA>: [<LISTA DE PIDS>]");


	return NULL;
}

bool buscar_pid(t_proceso* proceso){
	return proceso->pcb->pid == pid_busqueda;
}
