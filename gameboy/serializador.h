#ifndef SERIALIZADOR_H_
#define SERIALIZADOR_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include "utils.h"
#include "models.h"


//PAQUETES
t_paquete* getPaquete(char* arrayArgumentos[], char* tipo_mensaje);
t_paquete* modoSuscriptor(uint32_t mq_cod);
void liberarPaquete(t_paquete* paquete);
void* serializar_paquete(t_paquete* paquete, int bytes);

//SERIALIZACION Y DESERIALIZACION
t_new_pokemon* deserializar_newPokemon(t_buffer* buffer);
t_paquete* serializar_newPokemon(t_new_pokemon* newPokemon);
t_paquete* serializar_appearedPokemon(t_appeared_pokemon* pokemon);
t_appeared_pokemon* deserializar_appearedPokemon(t_buffer* buffer);
t_paquete* serializar_catchPokemon(t_catch_pokemon* pokemon);
t_catch_pokemon* deserializar_catchPokemon(t_buffer* buffer);
t_paquete* serializar_caughtPokemon(t_caught_pokemon* pokemon);
t_caught_pokemon* deserializar_caughtPokemon(t_buffer* buffer);
t_paquete* serializar_getPokemon(t_get_pokemon* pokemon);
t_get_pokemon* deserializar_getPokemon(t_buffer* buffer);
t_paquete* serializar_localizedPokemon(t_localized_pokemon* localizedPokemon);
t_localized_pokemon* deserializar_localizedPokemon(t_buffer* buffer);
t_paquete* serializar_akc(t_akc* akc);
t_akc* deserializar_akc(t_buffer* buffer);

t_paquete* serializar_registerModule(t_register_module* registerModule);
t_register_module* deserializar_registerModule(t_buffer* buffer);

//CREACION DE LOS STRUCTS
t_posicion_cantidad* crearPosicionCantidad(uint32_t x, uint32_t y, uint32_t cant);
t_posicion* crearPosicion(uint32_t x, uint32_t y);
t_new_pokemon* crearNewPokemon(uint32_t IDMensajeRecibido, char* nombre, t_posicion_cantidad* posicionCantidad);
t_localized_pokemon* crearLocalizedPokemon(uint32_t IDMensajeRecibido,uint32_t IDMensajeOriginal, char* nombre, uint32_t cantPosiciones, t_list* posicion);
t_appeared_pokemon* crearAppearedPokemon(uint32_t IDMensajeRecibido, char* nombre, t_posicion* posicion);
t_catch_pokemon* crearCatchPokemon(uint32_t ID_mensaje_recibido, char* nombre, t_posicion* posicion);
t_caught_pokemon* crearCaughtPokemon(uint32_t IDMensajeRecibido, uint32_t IDMensajeOriginal, uint32_t catchStatus);
t_get_pokemon* crearGetPokemon(uint32_t ID_mensaje_recibido, char* nombre);


#endif /* SERIALIZADOR_H_ */