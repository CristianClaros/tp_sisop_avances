#include "conexionCpu.h"

#define IP_CPU "127.0.0.1"
#define NAME_SERVER "CPU_DISPATCH"
#define NAME_SERVER_INTERRUPT "CPU_INTERRUPT"

int socket_dispatch;
int socket_interrupt;
int socket_memoria;

int iniciar_cpu(t_config_cpu* cpu_datos, t_log* logger_cpu){

	//Conectar a Memoria
	socket_memoria = crear_conexion(logger_cpu, IP_CPU, cpu_datos->IP_MEMORIA, cpu_datos->PUERTO_MEMORIA);
	enviar_mensaje("CPU",socket_memoria, HANDSHAKE);

	//SERVIDOR CPU_INTERRUPT
	socket_interrupt = iniciar_servidor(logger_cpu, "CPU_INTERRUPT", IP_CPU, cpu_datos->PUERTO_ESCUCHA_INTERRUPT);

	//Servidor CPU_DISPATCH
	socket_dispatch = iniciar_servidor(logger_cpu, NAME_SERVER, IP_CPU, cpu_datos->PUERTO_ESCUCHA_DISPATCH);

	while(server_escuchar(logger_cpu, NAME_SERVER, socket_dispatch, procesar_conexion_cpu) && server_escuchar(logger_cpu, NAME_SERVER_INTERRUPT, socket_interrupt, procesar_conexion_cpu));

	return 1;
}

void* procesar_conexion_cpu(void* void_args){

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
            //---------------------------------------- CPU -----------------------
        	case EJECUTAR_PROCESO:
        		//Recibe PCB para ejecutar
        		t_pcb* pcb;
        		pcb = recibir_pcb(cliente_socket);
        		mostrar_pcb(pcb);
//        		pedir_instruccion(pcb->registro->program_counter, socket_memoria, PEDIR_INSTRUCCION);
        		enviar_mensaje("VOY A PEDIRTE INSTRUCCIONES", socket_memoria, PEDIR_INSTRUCCION);
        		//Aca empieza a hacer el Ciclo de ejecucion
//        		t_instruccion* instruccion;
//        		do{
//        			pedir_instruccion(pcb->registro->program_counter, socket_memoria, PEDIR_INSTRUCCION);
//        			uint32_t cop;
//        			recv(cliente_socket, &cop, sizeof(op_code), 0);
//
//        			instruccion = recibir_instruccion(socket_memoria);
//        			printf("Instruccion(%s)\n", instruccion->instruccion);
//
//        		}while(strcmp(instruccion->instruccion, "EXIT") != 0);
        		//Ejemplo de arriba

        		enviar_mensaje("EXIT", cliente_socket, HANDSHAKE);
        		enviar_pcb(cliente_socket, pcb, HANDSHAKE);
        		break;
        	case RECIBIR_DIRECCION_FISICA:
        		//Recibe la direccion logica transformada a fisica por la memoria
        		break;
        	case RECIBIR_INSTRUCCION:
        		//Recibe la instruccion a ejecutar
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


void pedir_instruccion(uint32_t pid, int socket_cliente, uint32_t codigo_protocolo){
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete = crear_paquete(codigo_protocolo);
	agregar_a_paquete_int(paquete, &pid, sizeof(uint32_t));

	enviar_paquete(paquete, socket_cliente);
	eliminar_paquete(paquete);
}
