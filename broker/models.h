#ifndef MODELS_H_
#define MODELS_H_
#define ERROR -1

#include "global-includes.h"

#pragma region MessageQueue
typedef enum
{
	NEW_POKEMON = 1,
	APPEARED_POKEMON = 2,
	CATCH_POKEMON = 3,
	CAUGHT_POKEMON = 4,
	GET_POKEMON = 5,
	LOCALIZED_POKEMON = 6,
} mq_cod;

//Basicamente, las colas de mensaje, tambien representan mensajes
typedef enum 
{
    SUBSCRIBE = 7,
	MENSAJE_RECIBIDO = 8,
	ACKNOWLEDGEMENT = 9
} operation_cod;

typedef struct {
    uint32_t mq_cod;
    t_list* mensajes;
    t_list* subscribers;
} t_message_queue;

typedef struct{
	uint32_t id;
	uint32_t idCorrelativo;
	t_list* suscriptoresConfirmados; 
	void* mensaje; //Puede ser de cualquier tipo de mensaje 
} t_message;
#pragma endregion

#pragma region Comunicacion
typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	mq_cod codigo_mensaje;
	t_buffer* buffer;
} t_paquete;
#pragma endregion

#pragma region Estructuras_Auxiliares
typedef struct
{
	uint32_t posicion_x;
	uint32_t posicion_y;
	uint32_t cantidad;
} t_posicion_cantidad;

typedef struct
{
	uint32_t posicion_x;
	uint32_t posicion_y;
} t_posicion;

typedef struct
{
    char* nombre;
    t_posicion* posicion;
} t_pokemon_posicion;

typedef struct{
    char* nombre;
    uint32_t cantidad;
}t_pokemon_cantidad;
#pragma endregion

#pragma region Mensajes_Deserializados
typedef struct 
{
	uint32_t messageQueue;
} t_register_module;

typedef struct
{
	uint32_t ID_mensaje_recibido;
	uint32_t sizeNombre;
	char* nombre;
	t_posicion_cantidad* posicionCantidad;
} t_new_pokemon;

typedef struct{
	uint32_t ID_mensaje_recibido;
	uint32_t ID_mensaje_original;
	uint32_t sizeNombre;
	char* nombre;
	uint32_t cantidadPosiciones; 
	t_list* posiciones; //una lista de t_posicion
}t_localized_pokemon;

typedef struct{
	uint32_t ID_mensaje_recibido;
	uint32_t sizeNombre;
	char* nombre;
	t_posicion* posicion;
} t_appeared_pokemon;

typedef struct{
	uint32_t ID_mensaje_recibido;
	uint32_t sizeNombre;
	char* nombre;
	t_posicion* posicion;
}t_catch_pokemon;

typedef struct{
	uint32_t ID_mensaje_recibido;
	uint32_t ID_mensaje_original;
	uint32_t catchStatus; 
}t_caught_pokemon;

typedef struct{
	uint32_t ID_mensaje_recibido;
	uint32_t sizeNombre;
	char* nombre;
} t_get_pokemon;

//Esta estructura representa al mensaje que envía el broker con el ID del mensaje recibido
//Se lo envía al emisor del mensaje, como un "número de pedido", por si recibe respuesta.
typedef struct{
	uint32_t id_mensajeEnviado;
} t_id_mensaje_recibido;

typedef struct{

} t_acknowledgement;
#pragma endregion


#endif /* MODELS_H_ */