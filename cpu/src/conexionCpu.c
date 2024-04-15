#include "conexionCpu.h"

#define IP_CPU "127.0.0.1"

int socket_dispatch;
int socket_interrupt;

int iniciar_cpu(t_config_cpu* cpu_datos, t_log* logger_cpu){

	//Conectar a Memoria
	crear_conexion(logger_cpu, IP_CPU, cpu_datos->IP_MEMORIA, cpu_datos->PUERTO_MEMORIA);

	//Servidor CPU_DISPATCH
	crearServidor(logger_cpu, "SERVER_CPU_DISPATCH", IP_CPU, cpu_datos->PUERTO_ESCUCHA_DISPATCH, socket_dispatch, procesar_conexion_cpu);
	//SERVIDOR CPU_INTERRUPT
	crearServidor(logger_cpu, "SERVER_CPU_INTERRUPT", IP_CPU, cpu_datos->PUERTO_ESCUCHA_INTERRUPT, socket_dispatch, procesar_conexion_cpu);

	return 1;
}

void procesar_conexion_cpu(t_procesar_conexion_args* void_args){

    t_log* logger = void_args->log;
    int cliente_socket = void_args->fd;
    char *server_name = void_args->server_name;

    op_code cop;
    while (cliente_socket != -1) {

        if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_info(logger, "DISCONNECT!");
            return;
        }

        switch (cop) {
            //----------------------------------------CONSOLA-----------------------

        	case MENSAJE:
        	{
        		printf("HOLA");
        		break;
        	}
        	case GESTIONAR_CONSOLA_NUEVA:
            {
                /*t_procesar_fd_conexion* conexion = malloc(sizeof(t_procesar_fd_conexion));
                conexion->conexion = cliente_socket;*/
//                pthread_create(&procesoNuevo,NULL, atenderProcesoNuevo,cliente_socket);
//                pthread_join(procesoNuevo, NULL);
//                atenderProcesoNuevo(conexion->conexion);

                log_info(logger,"INSTRUCCIONES LISTAS");
                break;
            }

                //----------------------------------MEMORIA----------------------------------------
            case TABLA_SEGMENTOS_INICIAL:
            {
                //ACA SE RECIBIRIA LA BASE DEL SEGMENTO COMPARTIDO DONDE ESTA
//                pthread_mutex_lock(&BASE_INICIAL);
//                base_segmento_0 = recibir_base_segmento_0(cliente_socket);
                //HAY QUE ASIGNAR ESA BASE AL PROCESO.
                break;
            }
            case CREAR_SEGMENTO:
            {
                //El Kernel deberá enviarle a la Memoria el mensaje para crear un segmento con el tamaño definido
                break;
            }
            case SEGMENTO_CREADO:
            {
                //El segmento se creo exitosamente y la memoria nos devuelvio la base del nuevo segmento.
                break;
            }
            case ESPACIO_INSUFICIENTE:
            {
                //No se tiene más espacio disponible en la memoria y por lo tanto el proceso tiene que finalizar con error Out of Memory.
                break;
            }
            case COMPACTACION:
            {
                /*Se tiene el espacio disponible, pero el mismo no se encuentra contiguo,
                  por lo que se debe compactar, este caso lo vamos a analizar más en detalle,
                  ya que involucra controlar las operaciones de File System que se estén ejecutando.*/
                break;
            }
            case BORRAR_SEGMENTO:
            {
                /*Para realizar un DELETE_SEGMENT, el Kernel deberá enviarle a la Memoria el Id del
                  segmento a eliminar y recibirá como respuesta de la Memoria la tabla de segmentos actualizada.
                  Nota: No se solicitará nunca la eliminación del segmento 0 o de un segmento inexistente.*/
                break;
            }
            case DESCONEXION:
                log_error(logger, "Cliente desconectado de %s...", server_name);
                return;
            default:
                log_error(logger, "Algo anduvo mal en el server de %s", server_name);
                log_info(logger, "Cop: %d", cop);
                return;
        }
    }

    log_warning(logger, "El cliente se desconecto de %s server", server_name);
    return;
}
