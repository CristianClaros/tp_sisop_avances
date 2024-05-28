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

