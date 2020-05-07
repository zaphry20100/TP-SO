#ifndef BROKER_H_
#define BROKER_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "utils.h"
#include "sockets.h"
#include "models.h"
#include "messageQueue.h"

char* IP;
char* PUERTO;

t_message_queue* newPokemonMessageQueue;
t_message_queue* appearedPokemonMessageQueue;
t_message_queue* catchPokemonMessageQueue;
t_message_queue* getPokemonMessageQueue;
t_message_queue* localizedPokemonMessageQueue;
t_message_queue* caughtPokemonMessageQueue;

t_message_queue* crearMessageQueue(uint32_t mq_cod);
void terminar_programa(int, t_log*, t_config*);

#endif /* BROKER_H_ */