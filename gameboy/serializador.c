#include "serializador.h"

void* serializar_paquete(t_paquete* paquete, int bytes) 
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_mensaje), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento += paquete->buffer->size;

	return magic;
}

t_buffer* crear_buffer(uint32_t sizeStream, void* stream)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeStream;
	buffer->stream = malloc(buffer->size);
	buffer->stream = stream;
	return buffer;
}

t_paquete* crear_paquete(uint32_t mq_cod, uint32_t sizeStream, void* stream)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_mensaje = mq_cod;
	paquete->buffer = crear_buffer(sizeStream, stream);

	return paquete;
}

t_paquete* serializar_newPokemon(t_new_pokemon* newPokemon)
{
    t_buffer* newPokemon_buffer = malloc(sizeof(t_buffer));
    newPokemon_buffer->size = sizeof(uint32_t)*2 //ID_mensaje_recibido + sizePokemon
                            + strlen(newPokemon->nombre) + 1
                            + sizeof(t_posicion_cantidad);

    void* stream = malloc(newPokemon_buffer->size);
    int offset = 0;

    memcpy(stream + offset, &newPokemon->ID_mensaje_recibido, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &newPokemon->sizeNombre, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, newPokemon->nombre, strlen(newPokemon->nombre)+1);
    offset += strlen(newPokemon->nombre) +1;
    memcpy(stream + offset, &newPokemon->posicionCantidad->posicion_x, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &newPokemon->posicionCantidad->posicion_y, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &newPokemon->posicionCantidad->cantidad, sizeof(uint32_t));

    newPokemon_buffer->stream = stream;

	t_paquete* paquete = crear_paquete(NEW_POKEMON, newPokemon_buffer->size, newPokemon_buffer->stream);

    return paquete;
}

t_new_pokemon* deserializar_newPokemon(t_buffer* buffer)
{
	t_new_pokemon* newPokemon = malloc(sizeof(t_new_pokemon));
	newPokemon->posicionCantidad = malloc(sizeof(t_posicion_cantidad));
	
	void* stream = buffer->stream;

	memcpy(&(newPokemon->ID_mensaje_recibido), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(newPokemon->sizeNombre), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	newPokemon->nombre = malloc(newPokemon->sizeNombre);
	memcpy(newPokemon->nombre, stream, newPokemon->sizeNombre);
	stream += newPokemon->sizeNombre;
	memcpy(&(newPokemon->posicionCantidad->posicion_x), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(newPokemon->posicionCantidad->posicion_y), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(newPokemon->posicionCantidad->cantidad), stream, sizeof(uint32_t));
	 
	return newPokemon;
}

t_paquete* serializar_localizedPokemon(t_localized_pokemon* localizedPokemon){
	t_buffer* buffer = malloc(sizeof(t_buffer));
	uint32_t sizeListaPos = sizeof(uint32_t)* localizedPokemon->cantidadPosiciones * 2;

	buffer->size = sizeof(uint32_t)*4 //ID_mensaje_recibido + ID_mensaje_original + sizePokemon
				+ strlen(localizedPokemon->nombre)+1 //nombre pokemon
				+ sizeListaPos; // se le multiplica la cantidad de items de la lista por su size en bytes


	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &localizedPokemon->ID_mensaje_recibido, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &localizedPokemon->ID_mensaje_original, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &localizedPokemon->sizeNombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, localizedPokemon->nombre, strlen(localizedPokemon->nombre)+1);
	offset += strlen(localizedPokemon->nombre)+1;
	memcpy(stream + offset, &localizedPokemon->cantidadPosiciones, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	for(uint32_t i = 0; i < localizedPokemon->cantidadPosiciones; i++){
		memcpy(stream + offset, &(((t_posicion*)list_get(localizedPokemon->posiciones,i))->posicion_x),sizeof(uint32_t)); //HABIA QUE VER SI ESTO NO GENERA PROBLEMAS COMO EL CHAR* QUE NO SE ENVIABA
		offset += sizeof(uint32_t);
		memcpy(stream + offset, &(((t_posicion*)list_get(localizedPokemon->posiciones,i))->posicion_y),sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}

	buffer->stream = stream;

	t_paquete* paquete = crear_paquete(LOCALIZED_POKEMON, buffer->size, buffer->stream);

	return paquete;			
}

t_localized_pokemon* deserializar_localizedPokemon(t_buffer* buffer){
	t_localized_pokemon* localizedPokemon = malloc(sizeof(t_localized_pokemon));
	
	void* stream = buffer->stream;

	memcpy(&(localizedPokemon->ID_mensaje_recibido), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(localizedPokemon->ID_mensaje_original), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(localizedPokemon->sizeNombre), stream, sizeof(u_int32_t));
	stream += sizeof(uint32_t);
	localizedPokemon->nombre = malloc(localizedPokemon->sizeNombre);
	memcpy(localizedPokemon->nombre, stream, localizedPokemon->sizeNombre);
	stream += localizedPokemon->sizeNombre;
	log_info(logger, localizedPokemon->nombre);
	memcpy(&(localizedPokemon->cantidadPosiciones), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	localizedPokemon->posiciones = list_create();
	for(uint32_t i = 0; i < localizedPokemon->cantidadPosiciones; i++){
		uint32_t x;
		uint32_t y;
		memcpy(&(x), stream, sizeof(uint32_t));
		stream += sizeof(uint32_t);
		memcpy(&(y), stream, sizeof(uint32_t));
		stream += sizeof(uint32_t);
		t_posicion* posicion = crearPosicion(x,y);
		list_add(localizedPokemon->posiciones, posicion);
		
	}

	return localizedPokemon;
}


t_paquete* serializar_appearedPokemon(t_appeared_pokemon* pokemon)
{
	
	t_paquete* paquete;
    t_buffer* pokemon_buffer = malloc(sizeof(t_buffer));
    pokemon_buffer->size = sizeof(uint32_t)*3
                            + strlen(pokemon->nombre) + 1
                            + sizeof(t_posicion);

    void* stream = malloc(pokemon_buffer->size);
    int offset = 0;

    memcpy(stream + offset, &pokemon->ID_mensaje_recibido, sizeof(uint32_t));
    offset += sizeof(uint32_t);
	memcpy(stream + offset, &pokemon->ID_mensaje_original, sizeof(uint32_t));
	offset += sizeof(uint32_t);
    memcpy(stream + offset, &pokemon->sizeNombre, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, pokemon->nombre, strlen(pokemon->nombre)+1);
    offset += strlen(pokemon->nombre) +1;
    memcpy(stream + offset, &pokemon->posicion->posicion_x, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &pokemon->posicion->posicion_y, sizeof(uint32_t));

    pokemon_buffer->stream = stream;

	paquete = crear_paquete(APPEARED_POKEMON, pokemon_buffer->size, pokemon_buffer->stream);

    return paquete;
}

t_appeared_pokemon* deserializar_appearedPokemon(t_buffer* buffer)
{
	t_appeared_pokemon* pokemon = malloc(sizeof(t_appeared_pokemon));
	pokemon->posicion = malloc(sizeof(t_posicion));
	
	void* stream = buffer->stream;

	memcpy(&(pokemon->ID_mensaje_recibido), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(pokemon->ID_mensaje_original), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(pokemon->sizeNombre), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	pokemon->nombre = malloc(pokemon->sizeNombre);
	memcpy(pokemon->nombre, stream, pokemon->sizeNombre);
	stream += pokemon->sizeNombre;
	memcpy(&(pokemon->posicion->posicion_x), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(pokemon->posicion->posicion_y), stream, sizeof(uint32_t));

	return pokemon;
}

t_paquete* serializar_caughtPokemon(t_caught_pokemon* pokemon)
{
    t_paquete* paquete;
    t_buffer* pokemon_buffer = malloc(sizeof(t_buffer));
    pokemon_buffer->size = sizeof(uint32_t)*3;

    void* stream = malloc(pokemon_buffer->size);
    int offset = 0;

    memcpy(stream + offset, &pokemon->ID_mensaje_recibido, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &pokemon->ID_mensaje_original, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &pokemon->catchStatus, sizeof(uint32_t));

    pokemon_buffer->stream = stream;

    paquete = crear_paquete(CAUGHT_POKEMON, pokemon_buffer->size, pokemon_buffer->stream);

    return paquete;
}

t_caught_pokemon* deserializar_caughtPokemon(t_buffer* buffer)
{
    t_caught_pokemon* pokemon = malloc(sizeof(t_caught_pokemon));

    void* stream = buffer->stream;

    memcpy(&(pokemon->ID_mensaje_recibido), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(pokemon->ID_mensaje_original), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(pokemon->catchStatus), stream, sizeof(uint32_t));

    return pokemon;
}

t_paquete* serializar_catchPokemon(t_catch_pokemon* pokemon)
{

    t_paquete* paquete;
    t_buffer* pokemon_buffer = malloc(sizeof(t_buffer));
    pokemon_buffer->size = sizeof(uint32_t)*2
                            + strlen(pokemon->nombre) + 1
                            + sizeof(t_posicion);

    void* stream = malloc(pokemon_buffer->size);
    int offset = 0;

    memcpy(stream + offset, &pokemon->ID_mensaje_recibido, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &pokemon->sizeNombre, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, pokemon->nombre, strlen(pokemon->nombre)+1);
    offset += strlen(pokemon->nombre) +1;
    memcpy(stream + offset, &pokemon->posicion->posicion_x, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &pokemon->posicion->posicion_y, sizeof(uint32_t));

    pokemon_buffer->stream = stream;

    paquete = crear_paquete(CATCH_POKEMON, pokemon_buffer->size, pokemon_buffer->stream);

    return paquete;
}

t_catch_pokemon* deserializar_catchPokemon(t_buffer* buffer)
{
    t_catch_pokemon* pokemon = malloc(sizeof(t_catch_pokemon));
    pokemon->posicion = malloc(sizeof(t_posicion));

    void* stream = buffer->stream;

    memcpy(&(pokemon->ID_mensaje_recibido), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(pokemon->sizeNombre), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
	pokemon->nombre = malloc(pokemon->sizeNombre);
    memcpy(pokemon->nombre, stream, pokemon->sizeNombre);
    stream += pokemon->sizeNombre;
    memcpy(&(pokemon->posicion->posicion_x), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(pokemon->posicion->posicion_y), stream, sizeof(uint32_t));

    return pokemon;
}


t_paquete* serializar_getPokemon(t_get_pokemon* pokemon) 
{
    t_paquete* paquete;
    t_buffer* getPokemonBuffer = malloc(sizeof(t_buffer));

    getPokemonBuffer->size = 2 * sizeof(uint32_t) + strlen(pokemon->nombre) + 1;
    void* stream = malloc(getPokemonBuffer->size);
    int offset = 0;

    memcpy(stream + offset, &(pokemon->ID_mensaje_recibido), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(pokemon->sizeNombre), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, pokemon->nombre, strlen(pokemon->nombre)+1);

    getPokemonBuffer->stream = stream;
    paquete = crear_paquete(GET_POKEMON, getPokemonBuffer->size, getPokemonBuffer->stream);

    return paquete;
}

t_get_pokemon* deserializar_getPokemon(t_buffer* buffer)
{
    t_get_pokemon* pokemon = malloc(sizeof(t_get_pokemon));

    void* stream = buffer->stream;
    
    memcpy(&(pokemon->ID_mensaje_recibido), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(pokemon->sizeNombre), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
	pokemon->nombre = malloc(pokemon->sizeNombre);
    memcpy(pokemon->nombre, stream, pokemon->sizeNombre);

    return pokemon;
}

t_paquete* serializar_registerModule(t_register_module* registerModule, uint32_t operation) {
	t_buffer* registerModuleBuffer = malloc(sizeof(t_buffer));
	registerModuleBuffer->size = sizeof(uint32_t) * 2;
	void* stream = malloc(registerModuleBuffer->size);
	int offset = 0;

	memcpy(stream + offset, &(registerModule->messageQueue), sizeof(uint32_t));
	offset += sizeof(uint32_t);
    memcpy(stream + offset, &(registerModule->moduleId), sizeof(uint32_t));
    offset += sizeof(uint32_t);

	registerModuleBuffer->stream = stream;
	t_paquete* paquete = crear_paquete(operation, registerModuleBuffer->size, registerModuleBuffer->stream);

	return paquete;
}
t_register_module* deserializar_registerModule(t_buffer* buffer) {
	t_register_module* registerModule = malloc(sizeof(t_register_module));
	void* stream = buffer->stream;
	memcpy(&(registerModule->messageQueue), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
    memcpy(&(registerModule->moduleId), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	return registerModule;
}

t_paquete* serializar_suscripcion(t_suscripcion* suscripcion) {
    t_buffer* suscripcionBuffer = malloc(sizeof(t_buffer));
	suscripcionBuffer->size = sizeof(uint32_t) * 2;
    void* stream = malloc(suscripcionBuffer->size);
	int offset = 0;

    memcpy(stream + offset, &(suscripcion->idModule), sizeof(uint32_t));
	offset += sizeof(uint32_t);
    memcpy(stream + offset, &(suscripcion->socket), sizeof(uint32_t));
    offset += sizeof(uint32_t);

    suscripcionBuffer->stream = stream;
    t_paquete* paquete = crear_paquete(SUBSCRIBE, suscripcionBuffer->size, suscripcionBuffer->stream);

    return paquete;
}
t_suscripcion* deserializar_suscripcion(t_buffer* buffer) {
    t_suscripcion* suscripcion = malloc(sizeof(t_suscripcion));

    void* stream = buffer->stream;

    memcpy(&(suscripcion->idModule), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
    memcpy(&(suscripcion->socket), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    return suscripcion;
}

t_paquete* serializar_acknowledgement(t_acknowledgement* akc)
{
	t_buffer* akcBuffer = malloc(sizeof(t_buffer));
	akcBuffer->size = sizeof(uint32_t) * 3;
	void* stream = malloc(akcBuffer->size);
	int offset = 0;

    memcpy(stream + offset, &(akc->idModule), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &(akc->idMessageReceived), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &(akc->mq), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	akcBuffer->stream = stream;
	t_paquete* paquete = crear_paquete(ACKNOWLEDGEMENT, akcBuffer->size, akcBuffer->stream);

	return paquete;
}

t_acknowledgement* deserializar_acknowledgement(t_buffer* buffer)
{
	t_acknowledgement* akc = malloc(sizeof(t_acknowledgement));

	void* stream = buffer->stream;

    memcpy(&(akc->idModule), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(akc->idMessageReceived), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(akc->mq), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	return akc;
}

t_paquete* serializar_idMensajeRecibido(t_id_mensaje_recibido* idMensajeRecibido) 
{
    t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t);
    void* stream = malloc(buffer->size);

    memcpy(stream, &(idMensajeRecibido->id_mensajeEnviado), sizeof(uint32_t));

    buffer->stream = stream;
    t_paquete* paquete = crear_paquete(MENSAJE_RECIBIDO, buffer->size, buffer->stream);

    return paquete;
}
t_id_mensaje_recibido* deserializar_idMensajeRecibido(t_buffer* buffer) 
{
    t_id_mensaje_recibido* idMensajeEnviado = malloc(sizeof(t_id_mensaje_recibido)); //Enviado para el que lo deserializa

    void* stream = buffer->stream;

    memcpy(&(idMensajeEnviado->id_mensajeEnviado), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    return idMensajeEnviado;
}


t_posicion_cantidad* crearPosicionCantidad(uint32_t x, uint32_t y, uint32_t cant) 
{
	t_posicion_cantidad* position = malloc(sizeof(t_posicion_cantidad));

	position->posicion_x = x;
	position->posicion_y = y;
	position->cantidad = cant;

	return position;
}

t_posicion* crearPosicion(uint32_t x, uint32_t y)
{
	t_posicion* position = malloc(sizeof(t_posicion));
	
	position->posicion_x = x;
	position->posicion_y = y;

	return position;
}


t_new_pokemon* crearNewPokemon(uint32_t IDMensajeRecibido, char* nombre, t_posicion_cantidad* posicionCantidad)
{
	t_new_pokemon* newPokemon = malloc(sizeof(t_new_pokemon));

	newPokemon->ID_mensaje_recibido = IDMensajeRecibido;
	newPokemon->sizeNombre = strlen(nombre)+1;
	newPokemon->nombre = nombre;
	newPokemon->posicionCantidad = posicionCantidad;

	return newPokemon;
}

t_localized_pokemon* crearLocalizedPokemon(uint32_t IDMensajeRecibido,uint32_t IDMensajeOriginal, char* nombre, uint32_t sizePosicion, t_list* posicion)
{
	t_localized_pokemon* localizedPokemon = malloc(sizeof(t_localized_pokemon));

	localizedPokemon->ID_mensaje_recibido = IDMensajeRecibido;
	localizedPokemon->ID_mensaje_original = IDMensajeOriginal;
	localizedPokemon->sizeNombre = strlen(nombre)+1;
	localizedPokemon->nombre = nombre;
	localizedPokemon->cantidadPosiciones = sizePosicion;
	localizedPokemon->posiciones = list_create();
	list_add_all(localizedPokemon->posiciones, posicion);

	return localizedPokemon;
}

t_appeared_pokemon* crearAppearedPokemon(uint32_t IDMensajeRecibido, uint32_t IDMensajeOriginal, char* nombre, t_posicion* posicion){
	t_appeared_pokemon* appearedPokemon = malloc(sizeof(t_appeared_pokemon));

	appearedPokemon->ID_mensaje_recibido = IDMensajeRecibido;
	appearedPokemon->ID_mensaje_original = IDMensajeOriginal;
	appearedPokemon->sizeNombre = strlen(nombre)+1;
	appearedPokemon->nombre = nombre;
	appearedPokemon->posicion = posicion;

	return appearedPokemon;
}

t_catch_pokemon* crearCatchPokemon(uint32_t ID_mensaje_recibido, char* nombre, t_posicion* posicion)
{
	t_catch_pokemon* catchPokemon = malloc(sizeof(t_catch_pokemon));

	catchPokemon->ID_mensaje_recibido = ID_mensaje_recibido;
	catchPokemon->sizeNombre = strlen(nombre)+1;
	catchPokemon->nombre = nombre;
	catchPokemon->posicion = posicion;

	return catchPokemon;
}

t_caught_pokemon* crearCaughtPokemon(uint32_t IDMensajeRecibido, uint32_t IDMensajeOriginal, uint32_t catchStatus)
{
	t_caught_pokemon* caughtPokemon = malloc(sizeof(t_caught_pokemon));

	caughtPokemon->ID_mensaje_recibido = IDMensajeRecibido;
	caughtPokemon->ID_mensaje_original = IDMensajeOriginal;
	caughtPokemon->catchStatus = catchStatus;

	return caughtPokemon;
}

t_get_pokemon* crearGetPokemon(uint32_t ID_mensaje_recibido, char* nombre)
{
	t_get_pokemon* getPokemon = malloc(sizeof(t_get_pokemon));

	getPokemon->ID_mensaje_recibido = ID_mensaje_recibido;
	getPokemon->sizeNombre = strlen(nombre)+1;
	getPokemon->nombre = nombre;

	return getPokemon;
}

t_paquete* getPaquete(t_list* listaArgumentos, char* tipo_mensaje)
{
	t_paquete *paquete;
	if (strcmp(tipo_mensaje, "NEW_POKEMON") == 0)
	{
		t_posicion_cantidad *posCant = crearPosicionCantidad((uint32_t)atoi((char*)list_get(listaArgumentos,1)),(uint32_t)atoi((char*)list_get(listaArgumentos,2)),(uint32_t)atoi((char*)list_get(listaArgumentos,3)));
		t_new_pokemon *newPokemon;

		if(list_size(listaArgumentos) == 5){
			newPokemon = crearNewPokemon(atoi((char*)list_get(listaArgumentos,4)),(char*)list_get(listaArgumentos,0), posCant);
		}else{
			newPokemon = crearNewPokemon(0,(char*)list_get(listaArgumentos,0), posCant);
		}

		paquete = serializar_newPokemon(newPokemon);

		free(posCant);
		free(newPokemon);
	}
	else if (strcmp(tipo_mensaje, "APPEARED_POKEMON") == 0)
	{
		t_appeared_pokemon* appearedPokemon;
		t_posicion* posicion = crearPosicion((uint32_t)atoi((char*)list_get(listaArgumentos,1)),(uint32_t)atoi((char*)list_get(listaArgumentos,2)));
		if(list_size(listaArgumentos) == 3){
			appearedPokemon = crearAppearedPokemon(0,0, (char*)list_get(listaArgumentos,0), posicion); //para cuando no lo manden con el id correlativo
		}else{
			appearedPokemon = crearAppearedPokemon(0,atoi((char*)list_get(listaArgumentos,3)), (char*)list_get(listaArgumentos,0), posicion); //para cuando se les cante mandarlo con el id correlativo
		}

		paquete = serializar_appearedPokemon(appearedPokemon);

		free(posicion);
		free(appearedPokemon);
	}
	else if (strcmp(tipo_mensaje, "CATCH_POKEMON") == 0)
	{
		t_posicion* posicion = crearPosicion((uint32_t)atoi((char*)list_get(listaArgumentos,1)),(uint32_t)atoi((char*)list_get(listaArgumentos,2)));
		t_catch_pokemon* catchPokemon;

		if(list_size(listaArgumentos) == 3){
			catchPokemon = crearCatchPokemon(0, (char*)list_get(listaArgumentos,0), posicion);
		}else{
			catchPokemon = crearCatchPokemon((uint32_t)atoi((char*)list_get(listaArgumentos,3)), (char*)list_get(listaArgumentos,0), posicion);
		}

		paquete = serializar_catchPokemon(catchPokemon);

		free(posicion);
		free(catchPokemon);
	}
	else if (strcmp(tipo_mensaje, "CAUGHT_POKEMON") == 0)
	{
		t_caught_pokemon* pokemon;

		if(string_equals_ignore_case((char*)list_get(listaArgumentos,1), "OK")){
			pokemon = crearCaughtPokemon(0, (uint32_t)atoi((char*)list_get(listaArgumentos,0)), 1);
		}else{
			pokemon = crearCaughtPokemon(0, (uint32_t)atoi((char*)list_get(listaArgumentos,0)), 0);
		}

		paquete = serializar_caughtPokemon(pokemon);

		free(pokemon);
	}
	else if (strcmp(tipo_mensaje, "GET_POKEMON") == 0)
	{
		t_get_pokemon* getPokemon;

		if(list_size(listaArgumentos) == 1){
			getPokemon = crearGetPokemon(0, (char*)list_get(listaArgumentos,0));
		}else{
			getPokemon = crearGetPokemon(atoi((char*)list_get(listaArgumentos,1)), (char*)list_get(listaArgumentos,0));
		}

		paquete = serializar_getPokemon(getPokemon);
		free(getPokemon);
	}
	else if (strcmp(tipo_mensaje, "LOCALIZED_POKEMON") == 0)
	{ //ESTE AL FINAL NO LO HACE GAMEBOY, PARECE SER LA UNICA MQ QUE NO -> PERO LO DEJO IGUAL ASI PRUEBO LA SERIALIZACION Y LA DESERIALIZACION (DESPUES SACAR THO)

		t_posicion *pos1 = crearPosicion(5, 10);
		t_posicion *pos2 = crearPosicion(8, 11);

		t_list *listPos = list_create();
		list_add(listPos, pos1);
		list_add(listPos, pos2);

		t_localized_pokemon *localizedPokemon = crearLocalizedPokemon(0,0,"lucario", 2, listPos);
		
		paquete = serializar_localizedPokemon(localizedPokemon); 		

		free(pos1);
		free(pos2);
		free(listPos);
		free(localizedPokemon);
	}

	return paquete;
}

void liberarPaquete(t_paquete* paquete){
	free(paquete->buffer->stream);
	free(paquete->buffer);	
	free(paquete);
}

t_register_module* crearSuscribe(uint32_t ID_message_queue, uint32_t ID_module) {
	t_register_module* suscribe = malloc(sizeof(t_register_module));
	suscribe->messageQueue = ID_message_queue;
	suscribe->moduleId = ID_module;

	return suscribe;
}

t_acknowledgement* crearAcknowledgement(uint32_t idModule, uint32_t ID_mensaje_recibido, uint32_t mq_cod) {
	t_acknowledgement* ack = malloc(sizeof(t_acknowledgement));
	ack->idModule = idModule;
	ack->idMessageReceived = ID_mensaje_recibido;
	ack->mq = mq_cod;

	return ack;
}

t_id_mensaje_recibido* crearIdMensajeRecibido(uint32_t id) {
    t_id_mensaje_recibido* mensaje = malloc(sizeof(t_id_mensaje_recibido));

    mensaje->id_mensajeEnviado = id;

    return mensaje;
}

t_suscripcion* crearSuscripcion(uint32_t idAssigned, uint32_t socket_cliente) {
    t_suscripcion* suscripcion = malloc(sizeof(t_suscripcion));

    suscripcion->idModule = idAssigned;
    suscripcion->socket = socket_cliente;

    return suscripcion;
}





