#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include <commons/collections/list.h>

#include "models.h"
#include "utils.h"
// #include "serializador.h"

t_list* colaNEW;
t_list* colaREADY;
t_list* colaEXEC;
t_list* colaBLOCKED;
t_list* colaEXIT;

t_list* pokemonesEnMapa;
t_list* objetivoGlobal;

// typedef struct
// {
//     char* nombre;
//     t_posicion* posicion;
// } t_pokemon_posicion;

// typedef struct{
//     char* nombre;
//     uint32_t cantidad;
// }t_pokemon_cantidad;

// typedef struct
// {
// 	uint32_t id;
// 	t_posicion* posicion;
//     t_list* pokemonCapturados;
//     t_list* pokemonObjetivo;
//     uint32_t cantidadObjetivo;
//     t_pokemon_posicion* pokemonPlanificado;
//     uint32_t deadlock; // 0 y 1 por ahora, es solo un mockup
// } t_entrenador;

//INICIALIZACION HILOS

t_entrenador* getEntrenadorMejorPosicionado(t_pokemon_posicion* pokemon);
uint32_t moverEntrenadorDeCola(t_list* colaReceptora, t_list* colaEmisora, t_entrenador* entrenador);


#endif /* PLANIFICADOR_H_ */