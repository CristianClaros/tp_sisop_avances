#include "conexionCpu.h"

#define IP_CPU "127.0.0.1"
#define NAME_SERVER "CPU_DISPATCH"
#define NAME_SERVER_INTERRUPT "CPU_INTERRUPT"

int socket_dispatch;
int socket_interrupt;
int socket_memoria;

t_log* logger;

int iniciar_cpu(t_config_cpu* cpu_datos, t_log* logger_cpu){
	logger = logger_cpu;

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
        		int estado = 1; //Estado de pcb

        		t_pcb* pcb;
        		pcb = recibir_pcb(cliente_socket);

        		char* linea_instruccion = malloc(sizeof(char));
        		t_instruccion* instruccion_decode;

        		while(estado != 0){
        			linea_instruccion = fetch(pcb, socket_memoria);
        			instruccion_decode = decode(linea_instruccion);
        			estado = execute(instruccion_decode, pcb, cliente_socket);
        		}
        		mostrar_pcb(pcb);
        		if(strcmp(instruccion_decode->instruccion, "EXIT") == 0){
        			enviar_pcb(cliente_socket, pcb, FINALIZAR_PROCESO);
        		}
        		if(strcmp(instruccion_decode->instruccion, "IO_GEN_SLEEP") == 0){
        		    enviar_pcb(cliente_socket, pcb, SLEEP_INTERFAZ);

        		    t_paquete* paquete = crear_paquete(SLEEP_INTERFAZ);
        		    char* interfaz = (char*)list_get(instruccion_decode->parametros,0);
        		    int valor = atoi((char*)list_get(instruccion_decode->parametros,1));
        		    agregar_a_paquete(paquete, interfaz, strlen(interfaz)+1);
        		    agregar_a_paquete_int(paquete, &valor, sizeof(int));

        		    enviar_paquete(paquete, cliente_socket);
        		    eliminar_paquete(paquete);
        		}
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


char* fetch(t_pcb* pcb,int socket_cliente){
	char* instruccion = malloc(sizeof(char));
	void* buffer;
	int desplazamiento = 0;

	t_paquete* paquete = crear_paquete(PEDIR_INSTRUCCION);
	agregar_a_paquete_int(paquete, &(pcb->pid), sizeof(uint32_t));
	agregar_a_paquete_int(paquete, &(pcb->registro->program_counter), sizeof(uint32_t));
	enviar_paquete(paquete, socket_cliente);
	eliminar_paquete(paquete);

	//Espero respuesta de la memoria
	op_code code;
	code = recv(socket_cliente,&code,sizeof(op_code),0);
	uint32_t size;

	buffer = recibir_buffer(&size, socket_cliente);

	instruccion = recibir_string(buffer, &desplazamiento);

	pcb->registro->program_counter++;

	log_info(logger, "PID: <%i> - FETCH - Program counter: <%i>", pcb->pid, pcb->registro->program_counter);

	return instruccion;
}

t_instruccion* decode(char* instruccion){
	char* token;

	t_instruccion* instruccion_decode = malloc(sizeof(t_instruccion));
	instruccion_decode->instruccion = malloc(sizeof(char));

	token = strtok(instruccion, " ");
	instruccion_decode->cant_parametros = cantidad_argumentos(token);

	instruccion_decode->parametros = list_create();

	strcpy(instruccion_decode->instruccion, token);


	for(int i=0;i<instruccion_decode->cant_parametros;i++){
		token = strtok(NULL, " ");
		char* parametro = malloc(sizeof(char));
		strcpy(parametro, token);

		list_add(instruccion_decode->parametros, parametro);

	}

	return instruccion_decode;
}

int execute(t_instruccion* instruccion, t_pcb* pcb, int socket_kernel){

	if(strcmp(instruccion->instruccion, "SET") == 0){
		log_info(logger, "PID: <%i> - Ejecutando: <%s> - <%s, %s>", pcb->pid, instruccion->instruccion, (char*)list_get(instruccion->parametros, 0), (char*)list_get(instruccion->parametros, 1));
		set(instruccion->parametros,pcb);
	}
	if(strcmp(instruccion->instruccion, "MOV_IN") == 0){

	}
	if(strcmp(instruccion->instruccion, "MOV_OUT") == 0){

	}
	if(strcmp(instruccion->instruccion, "SUM") == 0){
		log_info(logger, "PID: <%i> - Ejecutando: <%s> - <%s, %s>", pcb->pid, instruccion->instruccion, (char*)list_get(instruccion->parametros, 0), (char*)list_get(instruccion->parametros, 1));
		sum(instruccion->parametros, pcb);
	}
	if(strcmp(instruccion->instruccion, "SUB") == 0){
		log_info(logger, "PID: <%i> - Ejecutando: <%s> - <%s, %s>", pcb->pid, instruccion->instruccion, (char*)list_get(instruccion->parametros, 0), (char*)list_get(instruccion->parametros, 1));
		sub(instruccion->parametros, pcb);
	}
	if(strcmp(instruccion->instruccion, "JNZ") == 0){
		log_info(logger, "PID: <%i> - Ejecutando: <%s> - <%s, %s>", pcb->pid, instruccion->instruccion, (char*)list_get(instruccion->parametros, 0), (char*)list_get(instruccion->parametros, 1));
		jnz(instruccion->parametros, pcb);
		}
	if(strcmp(instruccion->instruccion, "RESIZE") == 0){

		}
	if(strcmp(instruccion->instruccion, "COPY_STRING") == 0){

		}
	if(strcmp(instruccion->instruccion, "WAIT") == 0){

		}
	if(strcmp(instruccion->instruccion, "SIGNAL") == 0){

		}
	if(strcmp(instruccion->instruccion, "IO_GEN_SLEEP") == 0){
		log_info(logger, "PID: <%i> - Ejecutando: <%s> - <%s, %s>", pcb->pid, instruccion->instruccion, (char*)list_get(instruccion->parametros, 0), (char*)list_get(instruccion->parametros, 1));
		return io_gen_sleep((char*)list_get(instruccion->parametros,0), atoi((char*)list_get(instruccion->parametros,1)), pcb->pid, socket_kernel);
	}
	if(strcmp(instruccion->instruccion, "IO_STDIN_READ") == 0){

		}
	if(strcmp(instruccion->instruccion, "IO_STDOUT_WRITE") == 0){

		}
	if(strcmp(instruccion->instruccion, "IO_FS_CREATE") == 0){

		}
	if(strcmp(instruccion->instruccion, "IO_FS_DELETE") == 0){

		}
	if(strcmp(instruccion->instruccion, "IO_FS_TRUNCATE") == 0){

		}
	if(strcmp(instruccion->instruccion, "IO_FS_WRITE") == 0){

		}
	if(strcmp(instruccion->instruccion, "IO_FS_READ") == 0){

		}
	if(strcmp(instruccion->instruccion, "EXIT") == 0){
		log_info(logger, "PID: <%i> - Ejecutando: <%s> - <>", pcb->pid, instruccion->instruccion);
		return 0;
	}
	return 1;
}

//Funciones del execute
int set(t_list* lista_parametros, t_pcb* pcb){
	if(strcmp((char*)list_get(lista_parametros, 0), "PC") == 0){
		pcb->registro->program_counter = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "AX") == 0){
		pcb->registro->AX = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "BX") == 0){
		pcb->registro->BX = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "CX") == 0){
		pcb->registro->CX = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "DX") == 0){
		pcb->registro->DX = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "EAX") == 0){
		pcb->registro->EAX = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "EBX") == 0){
		pcb->registro->EBX = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "ECX") == 0){
		pcb->registro->ECX = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "EDX") == 0){
		pcb->registro->EDX = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "SI") == 0){
		pcb->registro->SI = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "DI") == 0){
		pcb->registro->DI = atoi((char*)list_get(lista_parametros, 1));
	}

	return 1;
}

int sum(t_list* lista_parametros, t_pcb* pcb){
	if(strcmp((char*)list_get(lista_parametros, 0), "PC") == 0){
		pcb->registro->program_counter = pcb->registro->program_counter  + registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "AX") == 0){
		pcb->registro->AX = pcb->registro->AX + registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "BX") == 0){
		pcb->registro->BX = pcb->registro->BX + registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "CX") == 0){
		pcb->registro->CX = pcb->registro->CX + registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "DX") == 0){
		pcb->registro->DX = pcb->registro->DX + registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "EAX") == 0){
		pcb->registro->EAX = pcb->registro->EAX + registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "EBX") == 0){
		pcb->registro->EBX = pcb->registro->EBX + registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "ECX") == 0){
		pcb->registro->ECX = pcb->registro->ECX + registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "EDX") == 0){
		pcb->registro->EDX = pcb->registro->EDX + registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "SI") == 0){
		pcb->registro->SI = pcb->registro->SI + registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "DI") == 0){
		pcb->registro->DI = pcb->registro->DI + registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}

	return 1;
}

int sub(t_list* lista_parametros, t_pcb* pcb){
	if(strcmp((char*)list_get(lista_parametros, 0), "PC") == 0){
		pcb->registro->program_counter = pcb->registro->program_counter - registro_origen(pcb,(char*)list_get(lista_parametros, 0));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "AX") == 0){
		pcb->registro->AX = pcb->registro->AX - registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "BX") == 0){
		pcb->registro->BX = pcb->registro->BX - registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "CX") == 0){
		pcb->registro->CX = pcb->registro->CX - registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "DX") == 0){
		pcb->registro->DX = pcb->registro->DX - registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "EAX") == 0){
		pcb->registro->EAX = pcb->registro->EAX - registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "EBX") == 0){
		pcb->registro->EBX = pcb->registro->EBX - registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "ECX") == 0){
		pcb->registro->ECX = pcb->registro->ECX - registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "EDX") == 0){
		pcb->registro->EDX = pcb->registro->EDX - registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "SI") == 0){
		pcb->registro->SI = pcb->registro->SI - registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "DI") == 0){
		pcb->registro->DI = pcb->registro->DI - registro_origen(pcb,(char*)list_get(lista_parametros, 1));
	}

	return 1;
}

int jnz(t_list* lista_parametros, t_pcb* pcb){
	if(strcmp((char*)list_get(lista_parametros, 0), "PC") == 0){
		if(pcb->registro->program_counter != 0)
			pcb->registro->program_counter = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "AX") == 0){
		if(pcb->registro->AX != 0)
			pcb->registro->program_counter = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "BX") == 0){
		if(pcb->registro->BX != 0)
			pcb->registro->program_counter = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "CX") == 0){
		if(pcb->registro->CX != 0)
			pcb->registro->program_counter = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "DX") == 0){
		if(pcb->registro->DX != 0)
			pcb->registro->program_counter = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "EAX") == 0){
		if(pcb->registro->EAX != 0)
			pcb->registro->program_counter = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "EBX") == 0){
		if(pcb->registro->EBX != 0)
			pcb->registro->program_counter = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "ECX") == 0){
		if(pcb->registro->ECX != 0)
			pcb->registro->program_counter = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "EDX") == 0){
		if(pcb->registro->EDX != 0)
			pcb->registro->program_counter = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "SI") == 0){
		if(pcb->registro->SI != 0)
			pcb->registro->program_counter = atoi((char*)list_get(lista_parametros, 1));
	}
	if(strcmp((char*)list_get(lista_parametros, 0), "DI") == 0){
		if(pcb->registro->DI != 0)
			pcb->registro->program_counter = atoi((char*)list_get(lista_parametros, 1));
	}

	return 1;
}

int registro_origen(t_pcb* pcb, char* registro_origen){
	if(strcmp(registro_origen, "PC") == 0)
		return pcb->registro->program_counter;
	if(strcmp(registro_origen, "AX") == 0)
		return pcb->registro->AX;
	if(strcmp(registro_origen, "BX") == 0)
		return pcb->registro->BX;
	if(strcmp(registro_origen, "CX") == 0)
		return pcb->registro->CX;
	if(strcmp(registro_origen, "DX") == 0)
		return pcb->registro->DX;
	if(strcmp(registro_origen, "EAX") == 0)
		return pcb->registro->EAX;
	if(strcmp(registro_origen, "EBX") == 0)
		return pcb->registro->EBX;
	if(strcmp(registro_origen, "ECX") == 0)
		return pcb->registro->ECX;
	if(strcmp(registro_origen, "EDX") == 0)
		return pcb->registro->EDX;
	if(strcmp(registro_origen, "SI") == 0)
		return pcb->registro->SI;
	if(strcmp(registro_origen, "DI") == 0)
		return pcb->registro->DI;

	return 0;
}


int io_gen_sleep(char* interfaz, int valor, uint32_t pid, int socket_kernel){
//	t_paquete* paquete = crear_paquete(SLEEP_INTERFAZ);
//	agregar_a_paquete(paquete, interfaz, strlen(interfaz)+1);
//	agregar_a_paquete_int(paquete, &pid, sizeof(uint32_t));
//	agregar_a_paquete_int(paquete, &valor, sizeof(int));
//
//	enviar_paquete(paquete, socket_dispatch);
//	eliminar_paquete(paquete);

	//SUpongo que encontro la interfaz

	return 0;
}
