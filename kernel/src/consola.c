#include "consola.h"

int iniciar_consola(int socket_memoria){
    char *linea;
    char* token;

    char* flag = " ";

    printf("INICIANDO CONSOLA INTERACTIVA.....\n");

    while (1) {
        linea = readline(">");

        if (!strncmp(linea, "exit", 4)) {
            free(linea);
            break;
        }

        if (linea && strcmp(linea,"") != 0) {
            add_history(linea);

            analizar_comando(linea, socket_memoria);
        }
        free(linea);
    }
    return 0;
}

void analizar_comando(char* linea, int socket_memoria){
    char* token;
    char* flag = " ";

    token = strtok(linea,flag);

    if(strcmp(token, "EJECUTAR_SCRIP") == 0)
        printf("VOY A EJECUTAR_SCRIP!!!\n");
    if(strcmp(token, "INICIAR_PROCESO") == 0)
    	enviar_mensaje("DAME LA TABLA DE MEMORIA!", socket_memoria, CREAR_PROCESO);
    if(strcmp(token, "FINALIZAR_PROCESO") == 0)
        printf("VOY A FINALIZAR_PROCESO!!!\n");
    if(strcmp(token, "DETENER_PLANIFICACION") == 0)
        printf("VOY A DETENER PLANIFICACION!!!\n");
    if(strcmp(token, "INICIAR_PLANIFICACION") == 0)
        printf("VOY A INICIAR_PLANIFICACION!!!\n");
    if(strcmp(token, "MULTIPROGRAMACION") == 0)
        printf("VOY A CAMBIAR LA MULTIPROGRAMACION!!!\n");
    if(strcmp(token, "PROCESO_ESTADO") == 0)
        printf("VOY A LISTAR EL ESTADO DE LOS PROCESOS!!!\n");

    while(token){
        printf("%s\n", token);
        token = strtok(NULL, " ");
    }
}

void iniciar_proceso(char* linea_comando){
    char* ruta_path = strtok(linea_comando, " ");
    char* size = strtok(NULL, " ");
    char* prioridad = strtok(NULL, " ");

    printf("RUTA: %s, size: %s, prioridad: %s", ruta_path, size, prioridad);
    printf("Se envio proceso!!!\n");
}
