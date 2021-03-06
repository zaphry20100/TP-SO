#ifndef TEAM_H_
#define TEAM_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<pthread.h>
#include<semaphore.h>
#include<readline/readline.h>
#include<commons/collections/list.h>

#include "sockets.h"
#include "utils.h"
#include "models.h"
#include "planificador.h"
#include "hiloEntrenador.h"

char* IP;
char* PUERTO;
char* LOG;
char* ALGORITMO;
bool primerRafaga;

//GLOBALES
uint32_t idModule;
uint32_t cantEntrenadores;
uint32_t cantCambiosCtx;
uint32_t cantDeadlocks;
uint32_t cantIntercambios;
uint32_t cantCiclosTotales;

t_list* objetivoGlobal;
t_list* pokemonesEnMapa;
t_list* entrenadoresCatch;

t_list* colaNEW;
t_list* colaREADY;
t_list* colaEXEC;
t_list* colaBLOCKED;
t_list* colaEXIT;
t_list* trIds;
t_list* listaDeadlocks;

t_suscribe* suscribeCaught;
t_suscribe* suscribeAppeared;
t_suscribe* suscribeLocalized;

pthread_t threadREADY;
pthread_t threadEXEC;
pthread_t threadEXIT;
pthread_t threadSERVER;
pthread_t threadSUSCRIBE_CAUGHT;
pthread_t threadSUSCRIBE_APPEARED;
pthread_t threadSUSCRIBE_LOCALIZED;
pthread_t threadRECONNECT;
pthread_t threadDETECT_DISCON;
pthread_t threadDETECT_DEADLOCK;
pthread_t finalizarPrograma;

sem_t mutexNEW;
sem_t mutexREADY;
sem_t mutexEXEC;
sem_t mutexBLOCKED;
sem_t mutexEXIT;
sem_t mutexPlanificadorEXEC;
sem_t mutexPokesEnMapa;
sem_t counterPokesEnMapa;
sem_t counterEntrenadoresCatch;
sem_t waitForFinish;
sem_t mutexReconnect;
sem_t mutexEntrenadoresCatch;
sem_t pokesObjetivoGlobal;
sem_t mutexDetector;
sem_t estaDesconectado;
sem_t mutexReporteDeadlock;
sem_t counterEntrenadoresDisponibles;

uint32_t cicloCPU;
uint32_t quantum;
uint32_t currentQuantum;

//INICIALIZACION Y TERMINACION DEL MODULO
void inicializarTeam();
void terminar_programa();

#endif /* TEAM_H_ */