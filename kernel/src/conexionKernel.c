#include "conexionKernel.h"

#define IP_KERNEL "127.0.0.1"
#define NAME_SERVER "SERVER KERNEL"
#define PUERTO_KERNEL "8003"

t_config_kernel* datos_kernel_config;

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

//SEMAFOROS


int iniciar_kernel(t_config_kernel* kernel_datos, t_log* logger_kernel){

	datos_kernel_config = kernel_datos;

	socket_kernel = iniciar_servidor(logger_kernel, NAME_SERVER, IP_KERNEL, PUERTO_KERNEL);

	socket_interrupt = crear_conexion(logger_kernel, IP_KERNEL, kernel_datos->IP_CPU, kernel_datos->PUERTO_CPU_INTERRUPT);
	enviar_mensaje("KERNEL-INTERRUPT", socket_interrupt, HANDSHAKE);

	socket_dispatch = crear_conexion(logger_kernel, IP_KERNEL, kernel_datos->IP_CPU, kernel_datos->PUERTO_CPU_DISPATCH);
	enviar_mensaje("KERNEL-DISPATCH", socket_dispatch, HANDSHAKE);

	socket_memoria = crear_conexion(logger_kernel, IP_KERNEL, kernel_datos->IP_MEMORIA, kernel_datos->PUERTO_MEMORIA);
	enviar_mensaje("KERNEL", socket_memoria, HANDSHAKE);

	inicializar_variables();

	while(server_escuchar(logger_kernel, NAME_SERVER, socket_kernel) && iniciar_consola());


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
    	enviar_mensaje(token, socket_memoria, CREAR_PROCESO); //Ejemplo de un msj a MEMORIA
    	//ACA SE CREA LA FUNCION DE CREAR_PROCESO
    }
    if(strcmp(token, "FINALIZAR_PROCESO") == 0){
    	token = strtok(NULL, " ");
    	printf("VOY A ELIMINAR EL PROCESO PID: %i\n", atoi(token)); //EJEMPLO de que PID voy a eliminar
    }
    if(strcmp(token, "DETENER_PLANIFICACION") == 0)
        printf("VOY A DETENER PLANIFICACION!!!\n"); //ACA VA A DETENER MEDIANTE UN SEMAFORO
    if(strcmp(token, "INICIAR_PLANIFICACION") == 0)
        printf("VOY A INICIAR_PLANIFICACION!!!\n"); //LO MISMO MEDIANTE UN SEMAFORO
    if(strcmp(token, "MULTIPROGRAMACION") == 0){
    	token = strtok(NULL, " ");
    	datos_kernel_config->GRADO_MULTIPROGRAMACION = atoi(token); //ACA VA HACE UNA RESTA CON LOS VALORES NUEVOS Y VA A INCREMENTAR EL SEMAFORO CON LAS DIFERENCIA
    }
    if(strcmp(token, "PROCESO_ESTADO") == 0)
        printf("VOY A LISTAR EL ESTADO DE LOS PROCESOS!!!\n"); //ACA VA A USAR UNA FUNCION MOSTRAR ESTADO CON TODAS LAS COLAS (READY, NEW, BLOCKED, EXIT)

}
