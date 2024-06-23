#include "estructuras.h"
#include "conexion.h"

//Propias de cada modulo
void iterator_instruccion(t_instruccion* instruccion){
	printf("(%s)", instruccion->instruccion);

	list_iterate(instruccion->parametros, (void*)iterator_parametro);

	printf("\n");

}

void iterator_parametro(t_parametro* param){
	printf("(%s)",param->parametro);
}

//Enviar_pcb
void enviar_pcb(int socket_cliente, t_pcb* pcb, uint32_t protocolo){
	t_paquete* paquete;

	paquete = crear_paquete(protocolo);

	agregar_a_paquete_int(paquete, &(pcb->pid), sizeof(uint32_t));
	agregar_a_paquete_int(paquete, &(pcb->quantum), sizeof(uint32_t));

	//Agregar resgistro por registro
	agregar_a_paquete_int(paquete, &(pcb->registro->program_counter), sizeof(uint32_t));
	agregar_a_paquete_int(paquete, &(pcb->registro->AX), sizeof(uint8_t));
	agregar_a_paquete_int(paquete, &(pcb->registro->BX), sizeof(uint8_t));
	agregar_a_paquete_int(paquete, &(pcb->registro->CX), sizeof(uint8_t));
	agregar_a_paquete_int(paquete, &(pcb->registro->DX), sizeof(uint8_t));
	agregar_a_paquete_int(paquete, &(pcb->registro->EAX), sizeof(uint32_t));
	agregar_a_paquete_int(paquete, &(pcb->registro->EBX), sizeof(uint32_t));
	agregar_a_paquete_int(paquete, &(pcb->registro->ECX), sizeof(uint32_t));
	agregar_a_paquete_int(paquete, &(pcb->registro->EDX), sizeof(uint32_t));
	agregar_a_paquete_int(paquete, &(pcb->registro->SI), sizeof(uint32_t));
	agregar_a_paquete_int(paquete, &(pcb->registro->DI), sizeof(uint32_t));

	enviar_paquete(paquete, socket_cliente);
	eliminar_paquete(paquete);
}

//Recibir PCB
t_pcb* recibir_pcb(int socket_cliente){
	uint32_t size;
	int desplazamiento = 0;
	void * buffer;

	t_pcb* pcb = malloc(sizeof(t_pcb));
	pcb->registro = malloc(sizeof(t_registros));

	buffer = recibir_buffer(&size, socket_cliente);
	//CARACTERISTICAS DEL PCB
	memcpy(&(pcb->pid), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&(pcb->quantum), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	//REGISTROS
	memcpy(&(pcb->registro->program_counter), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&(pcb->registro->AX), buffer + desplazamiento, sizeof(uint8_t));
	desplazamiento += sizeof(uint8_t);
	memcpy(&(pcb->registro->BX), buffer + desplazamiento, sizeof(uint8_t));
	desplazamiento += sizeof(uint8_t);
	memcpy(&(pcb->registro->BX), buffer + desplazamiento, sizeof(uint8_t));
	desplazamiento += sizeof(uint8_t);
	memcpy(&(pcb->registro->DX), buffer + desplazamiento, sizeof(uint8_t));
	desplazamiento += sizeof(uint8_t);
	memcpy(&(pcb->registro->EAX), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&(pcb->registro->EBX), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&(pcb->registro->ECX), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&(pcb->registro->EDX), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&(pcb->registro->SI), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&(pcb->registro->DI), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	free(buffer);
	return pcb;
}

void mostrar_pcb(t_pcb* pcb){
	printf("PID(%i)\n", pcb->pid);
	printf("QUANTUM(%i)\n", pcb->quantum);

	mostrar_registros(pcb->registro);
}

//Recibir instrucciones para ver comunicacion solamente
t_list* recibir_instrucciones(int socket_cliente){
	uint32_t size;
	int desplazamiento = 0;
	void * buffer;
	uint32_t cantidad = 0;

	t_list* valores = list_create();

	buffer = recibir_buffer(&size, socket_cliente);

	memcpy(&cantidad, buffer + desplazamiento,sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	for(int j=0;j<cantidad;j++){
		t_instruccion* instruccion = malloc(sizeof(t_instruccion));

		instruccion->parametros = list_create();

		uint32_t caracteres_instruccion;

		memcpy(&(caracteres_instruccion), buffer + desplazamiento, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		instruccion->instruccion = malloc(caracteres_instruccion);

		memcpy(instruccion->instruccion, buffer + desplazamiento, caracteres_instruccion);
		desplazamiento += caracteres_instruccion;

		//Cantidad de parametro en instruccion
		memcpy(&(instruccion->cant_parametros), buffer + desplazamiento, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);

		for(int i = 0; i< (instruccion->cant_parametros); i++){
			t_parametro* parametro = malloc(sizeof(t_parametro));
			uint32_t caracteres_parametro;

			memcpy(&(caracteres_parametro),buffer + desplazamiento, sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);

			parametro->parametro = malloc(caracteres_parametro);
			memcpy(parametro->parametro, buffer + desplazamiento, caracteres_parametro);
			desplazamiento += caracteres_parametro;

			list_add(instruccion->parametros, parametro);
		}
		list_add(valores, instruccion);
	}
	free(buffer);
	return valores;
}

//Envio de Instrucciones para ver comunicacion solamente
void enviar_instrucciones(int socket_cliente, t_list* lista_instrucciones, uint32_t protocolo){
	t_paquete* paquete;

	paquete = crear_paquete(protocolo);

	// Leemos las instrucciones y vamos agregando
	int cantidad = list_size(lista_instrucciones);

	agregar_a_paquete_int(paquete, &cantidad, sizeof(uint32_t));

	for(int i = 0; i < cantidad; i++){
		t_instruccion* instruccion = malloc(sizeof(t_instruccion));

		instruccion = (t_instruccion*) list_get(lista_instrucciones, i);
		agregar_a_paquete(paquete, instruccion->instruccion, strlen(instruccion->instruccion)+1);
		agregar_a_paquete_int(paquete, &(instruccion->cant_parametros), sizeof(uint32_t));

		for(int j = 0;j<(instruccion->cant_parametros); j++){
			t_parametro* parametro = malloc(sizeof(t_parametro));
			parametro = (t_parametro*) list_get(instruccion->parametros, j);
			agregar_a_paquete(paquete, parametro->parametro, strlen(parametro->parametro)+1);
		}
	}

	enviar_paquete(paquete, socket_cliente);
	eliminar_paquete(paquete);
}

//
t_registros* iniciar_registros(){
	t_registros* registro = malloc(sizeof(t_registros));
	registro->program_counter = 0;
	registro->AX = 0;
	registro->BX = 0;
	registro->CX = 0;
	registro->DX = 0;
	registro->EAX = 0;
	registro->EBX = 0;
	registro->ECX = 0;
	registro->EDX = 0;
	registro->SI = 0;
	registro->DI = 0;

	return registro;
}

void mostrar_registros(t_registros* registro){

	printf("Program Counter:(%i)\n", registro->program_counter);
	printf("Registro AX:(%i)\n", registro->AX);
	printf("Registro BX:(%i)\n", registro->BX);
	printf("Registro CX:(%i)\n", registro->CX);
	printf("Registro DX:(%i)\n", registro->DX);
	printf("Registro EAX:(%i)\n", registro->EAX);
	printf("Registro EBX:(%i)\n", registro->EBX);
	printf("Registro ECX:(%i)\n", registro->ECX);
	printf("Registro EDX:(%i)\n", registro->EDX);
	printf("Registro SI:(%i)\n", registro->SI);
	printf("Registro DI:(%i)\n", registro->DI);

}

//Recibir instruccion de parte de la memoria
t_instruccion* recibir_instruccion(int socket_cliente){
	uint32_t size;
	int desplazamiento = 0;
	void * buffer;

	buffer = recibir_buffer(&size, socket_cliente);

	t_instruccion* instruccion = malloc(sizeof(t_instruccion));

	instruccion->parametros = list_create();
	uint32_t caracteres_instruccion;

	memcpy(&(caracteres_instruccion), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	instruccion->instruccion = malloc(caracteres_instruccion);

	memcpy(instruccion->instruccion, buffer + desplazamiento, caracteres_instruccion);
	desplazamiento += caracteres_instruccion;

	//Cantidad de parametro en instruccion
	memcpy(&(instruccion->cant_parametros), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	for(int i = 0; i< (instruccion->cant_parametros); i++){
		t_parametro* parametro = malloc(sizeof(t_parametro));
		uint32_t caracteres_parametro;
		memcpy(&(caracteres_parametro),buffer + desplazamiento, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		parametro->parametro = malloc(caracteres_parametro);
		memcpy(parametro->parametro, buffer + desplazamiento, caracteres_parametro);
		desplazamiento += caracteres_parametro;
	}

	free(buffer);
	return instruccion;
}

//Envio de Instrucciones
void enviar_instruccion(int socket_cliente, t_instruccion* instruccion, uint32_t protocolo){
	t_paquete* paquete;

	paquete = crear_paquete(protocolo);

	agregar_a_paquete(paquete, instruccion->instruccion, strlen(instruccion->instruccion)+1);
	agregar_a_paquete_int(paquete, &(instruccion->cant_parametros), sizeof(uint32_t));

	for(int j = 0;j<(instruccion->cant_parametros); j++){
		t_parametro* parametro = malloc(sizeof(t_parametro));
		parametro = (t_parametro*) list_get(instruccion->parametros, j);
		agregar_a_paquete(paquete, parametro->parametro, strlen(parametro->parametro)+1);
	}

	enviar_paquete(paquete, socket_cliente);
	eliminar_paquete(paquete);
}

t_list* abrir_instrucciones(char* ruta) {
	FILE* archivo;
	char linea[100];
	char* cadena;
	char* token;
	t_list* lista;

	char* ruta_scrip = malloc(100 * sizeof(char));

	lista = list_create();

	strcpy(ruta_scrip, "");
	strcat(ruta_scrip, "../../../../..");
	strcat(ruta_scrip, "/home/utnso/scripts-pruebas");
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
				parametro->parametro = malloc(sizeof(char));

				strcpy(parametro->parametro, token);

				list_add(instruccion->parametros, parametro);

			}

			list_add(lista, (void*)instruccion);

		}
	}

	fclose(archivo);
	return lista;
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
