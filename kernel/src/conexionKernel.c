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
int socket_interfaz1;

//VARIABLE COLAS Y LISTAS
t_list* cola_ready;
t_list* cola_exec;
t_list* cola_blocked;
t_list* cola_exit;

//Listas de kernel
t_list* lista_interfaz;

//LISTA ARCHIVOS ABIERTOS Y RECURSOS
t_list* archivos_abiertos;
t_list* recursos_disponibles;

typedef struct{
	char* recursos;
	int instancias;
}t_recursos;

//PIDs
int pid_list = 0;
uint32_t pid = 0;

//SEMAFOROS
sem_t ready;
sem_t listo_ready;
sem_t exec;
sem_t planificador;

//Mutex
pthread_mutex_t mutex_instrucciones;
pthread_mutex_t mutex_cola_ready;
pthread_mutex_t mutex_cola_exec;


uint32_t pid_busqueda = 0;
char* nombre_interfaz;

int iniciar_kernel(t_config_kernel* kernel_datos, t_log* logger_kernel){

	datos_kernel_config = kernel_datos;
	log_kernel = logger_kernel;
	lista_interfaz = list_create();

	sem_init(&ready, 0, datos_kernel_config->GRADO_MULTIPROGRAMACION);
	sem_init(&listo_ready, 0, 1);
	sem_init(&exec, 0, 1);
	sem_init(&planificador, 0, 1);

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
	cola_exec = list_create();
	cola_blocked = list_create();
	cola_exit = list_create();

	recursos_disponibles = iniciar_recursos();

	sem_wait(&listo_ready);
	iniciar_planificador_corto();
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

void iniciar_planificador_corto(){
	pthread_t planificador_corto;
	if(strcmp(datos_kernel_config->ALGORITMO_PLANIFICACION, "FIFO") == 0){
		pthread_create(&planificador_corto, NULL, (void*)iniciar_fifo, NULL);
		pthread_detach(planificador_corto);
	}
	if(strcmp(datos_kernel_config->ALGORITMO_PLANIFICACION, "RR") == 0){
		pthread_create(&planificador_corto, NULL, (void*)iniciar_rr, NULL);
		pthread_detach(planificador_corto);
	}
	if(strcmp(datos_kernel_config->ALGORITMO_PLANIFICACION, "VRR") == 0){
		pthread_create(&planificador_corto, NULL, (void*)iniciar_vrr, NULL);
		pthread_detach(planificador_corto);
	}
}

void iniciar_fifo(){
	sem_wait(&planificador);
	while(1){
		sem_wait(&planificador);
		sem_wait(&listo_ready);
		sem_wait(&exec);

		t_proceso* proceso = malloc(sizeof(t_proceso));
		pthread_mutex_lock(&mutex_cola_exec);
		proceso = (t_proceso*) list_remove(cola_ready, 0);
		sem_post(&ready);
		cambiar_estado(proceso, "EXEC");

		list_add(cola_exec, proceso);
		enviar_pcb(socket_dispatch, proceso->pcb, EJECUTAR_PROCESO);

		//ESPERA RESPUESTA DEL CPU

		op_code cod;
		t_pcb* pcb_retorno = malloc(sizeof(t_pcb));
		recv(socket_dispatch, &cod, sizeof(op_code), 0);
		pcb_retorno = recibir_pcb(socket_dispatch);

		proceso = (t_proceso*) list_remove(cola_exec, 0);

		proceso->pcb = pcb_retorno;
		//SI FINALIZA LAS INSTRUCICONES
		if(cod == FINALIZAR_PROCESO){
			cambiar_estado(proceso, "EXIT");
			list_add(cola_exit, proceso);
		}
		//SI debe hacer uso del i/o
		if(cod == SLEEP_INTERFAZ){
			cambiar_estado(proceso, "BLOCKED");
			list_add(cola_blocked, proceso);
			//Todo esto va como proceso de blocked
			cod = recv(socket_dispatch, &cod, sizeof(op_code),0);
			uint32_t size;
			int desplazamiento = 0;
			void* buffer = recibir_buffer(&size, socket_dispatch);

			char* interfaz = recibir_string(buffer, &desplazamiento);
			int valor = recibir_int(buffer, &desplazamiento);

			//Buscar interfaz en la lista
			t_paquete* paquete_enviar = crear_paquete(IO_GEN_SLEEP);
			agregar_a_paquete_int(paquete_enviar, &(proceso->pcb->pid),sizeof(uint32_t));
			agregar_a_paquete_int(paquete_enviar, &valor, sizeof(int));
			enviar_paquete(paquete_enviar, socket_interfaz1);

			eliminar_paquete(paquete_enviar);

		}

		pthread_mutex_unlock(&mutex_cola_exec);
		sem_post(&exec);
		sem_post(&listo_ready);
	}
}

void iniciar_rr(){

}

void iniciar_vrr(){

}

//PROCESAR CONEXION
void* procesar_conexion_kernel(void* void_args){

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
        	case IDENTIFICAR_INTERFAZ:
        		//Identifica la interfaz
        		buffer = recibir_buffer(&size, cliente_socket);
        		t_interfaz* interfaz = malloc(sizeof(t_interfaz));
        		interfaz->nombre = malloc(sizeof(char));
        		interfaz->tipo_interfaz = malloc(sizeof(char));

        		desplazamiento = 0;
        		interfaz->nombre = recibir_string(buffer, &desplazamiento);
        		interfaz->socket = cliente_socket;
        		//Aca esta linea de abajo vuela, porque debo buscar en la lista su socket
        		socket_interfaz1 = cliente_socket;

        		interfaz->tipo_interfaz = recibir_string(buffer, &desplazamiento);

        		list_add(lista_interfaz, interfaz);


        		log_info(logger, "Interfaz conectada (%s - %s)", interfaz->nombre, interfaz->tipo_interfaz);

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
        		buffer = recibir_buffer(&size, cliente_socket);

        		desplazamiento = 0;
        		nombre_interfaz = recibir_string(buffer, &desplazamiento);
        		int pid_sleep = recibir_int(buffer, &desplazamiento);
        		int valor_sleep = recibir_int(buffer, &desplazamiento);


        		if(list_any_satisfy(lista_interfaz, (void*)buscar_interfaz)){
        			t_interfaz* interfaz = (t_interfaz*) list_find(lista_interfaz, (void*) buscar_interfaz);

        			t_paquete* paquete = crear_paquete(IO_GEN_SLEEP);
        			agregar_a_paquete_int(paquete, &(pid_sleep), sizeof(uint32_t));
        			agregar_a_paquete_int(paquete, &(valor_sleep), sizeof(int));

        			enviar_paquete(paquete, interfaz->socket);
        			eliminar_paquete(paquete);

        			//Espera rspuesta de la interfaz
        		}
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
    	cambiar_estado(proceso_exit, "EXIT");

    	list_add(cola_exit, proceso_exit);

    }
    if(strcmp(token, "DETENER_PLANIFICACION") == 0)
    	sem_wait(&listo_ready);
    if(strcmp(token, "INICIAR_PLANIFICACION") == 0)
    	sem_post(&listo_ready);
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

    	list_iterate(cola_ready, (void*) iterator);
    	list_iterate(cola_exit, (void*) iterator);
    	list_iterate(cola_exec, (void*) iterator);
    	list_iterate(cola_blocked, (void*) iterator);
    }
}

void iterator(t_proceso* proceso){
	printf("PID(%i), ESTADO(%s)\n",proceso->pcb->pid, proceso->estado);
}

//CAmbiar estado
void* cambiar_estado(t_proceso* proceso, char* estado){
	log_info(log_kernel, "PID: <%i> - Estado Anterior: <%s> - Estado Actual: <%s>", proceso->pcb->pid, proceso->estado, estado);
	proceso->estado = estado;
	return NULL;
}

//INICIAR PROCESO
void* iniciar_proceso(char* ruta){
	t_proceso* proceso = malloc(sizeof(t_proceso));

	proceso->estado = malloc(sizeof(char));
	proceso->pcb = malloc(sizeof(t_pcb));

	proceso->estado = "NEW";
	proceso->pcb->pid = pid;
	pid++;
	proceso->pid_list = pid_list;
	pid_list++;

	proceso->pcb->quantum = datos_kernel_config->QUANTUM;
	proceso->pcb->registro = iniciar_registros();

	//ACA PIDO TABLA
	pthread_mutex_lock(&mutex_instrucciones);

	t_paquete* paquete = crear_paquete(CREAR_PROCESO);
	agregar_a_paquete_int(paquete, &(proceso->pcb->pid), sizeof(uint32_t));
	agregar_a_paquete(paquete, ruta, strlen(ruta)+1);
	enviar_paquete(paquete, socket_memoria);
	eliminar_paquete(paquete);

	op_code code;
	recv(socket_memoria, &code, sizeof(op_code),0);
	if(code != OK){
		perror("NO SE PUDO CREAR TABLA POR PROBLEMAS DE ARCHIVO\n");
	}
	log_info(log_kernel, "Se crea el proceso <%i> en %s", proceso->pcb->pid, proceso->estado);
	pthread_mutex_unlock(&mutex_instrucciones);

	sem_wait(&ready);
	pthread_mutex_lock(&mutex_cola_ready);
	cambiar_estado(proceso, "READY");

	list_add(cola_ready, proceso);
	log_info(log_kernel, "Cola Ready <COLA>: [<LISTA DE PIDS>]");

	pthread_mutex_unlock(&mutex_cola_ready);
	sem_post(&planificador);

	return NULL;
}

bool buscar_pid(t_proceso* proceso){
	return proceso->pcb->pid == pid_busqueda;
}

bool buscar_interfaz(t_interfaz* interfaz){
	return (strcmp(interfaz->tipo_interfaz, nombre_interfaz) == 0);
}

