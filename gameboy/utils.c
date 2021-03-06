
#include "utils.h"

t_list* cortarArgumentos(int lengthArray, char *argumentos[])
{	t_list* lista = list_create();

	for (uint32_t i = 3; i < lengthArray; i++)
	{
		list_add(lista,argumentos[i]);
	}
	return lista;
}

uint32_t getColaDeMensajes(char* queue) {
	if(strcmp(queue, "NEW_POKEMON") == 0) {
		return NEW_POKEMON;
	}
	if(strcmp(queue, "APPEARED_POKEMON") == 0) {
		return APPEARED_POKEMON;
	}
	if(strcmp(queue, "CATCH_POKEMON") == 0) {
		return CATCH_POKEMON;
	}
	if(strcmp(queue, "CAUGHT_POKEMON") == 0) {
		return CAUGHT_POKEMON;
	}
	if(strcmp(queue, "GET_POKEMON") == 0) {
		return GET_POKEMON;
	}
	if(strcmp(queue, "LOCALIZED_POKEMON") == 0) {
		return LOCALIZED_POKEMON;
	}
	return -1;
}

uint32_t procesarComando(char** ip, char** puerto, char *proceso, char *tipo_mensaje) {
	uint32_t isValid = 0;
	char *brokerCommands[] = {"NEW_POKEMON", "APPEARED_POKEMON", "CATCH_POKEMON", "CAUGHT_POKEMON", "GET_POKEMON"};
	char *teamCommands[] = {"APPEARED_POKEMON", "CAUGHT_POKEMON"};
	char *gamecardCommands[] = {"NEW_POKEMON", "CATCH_POKEMON", "GET_POKEMON"};
	char *ipAux, *puertoAux;

	if (strcmp(proceso, "BROKER") == 0)
	{
		//log_info(logger, "Conectado a BROKER a la cola %s", tipo_mensaje);
		ipAux = config_get_string_value(config, "IP_BROKER");
		puertoAux = config_get_string_value(config, "PUERTO_BROKER");
		if (perteneceAlArray(tipo_mensaje, brokerCommands, 5) == 1)
		{
			isValid = 1;
		}
	}
	else if (strcmp(proceso, "SUSCRIPTOR") == 0)
	{
		//log_info(logger, "Conectado a BROKER en modo suscriptor");
		ipAux = config_get_string_value(config, "IP_BROKER");
		puertoAux = config_get_string_value(config, "PUERTO_BROKER");
		isValid = 1;
	}
	else if (strcmp(proceso, "TEAM") == 0)
	{
		//log_info(logger, "Conectado a TEAM por la cola %s", tipo_mensaje);
		ipAux = config_get_string_value(config, "IP_TEAM");
		puertoAux = config_get_string_value(config, "PUERTO_TEAM");
		if (perteneceAlArray(tipo_mensaje, teamCommands, 1) == 1)
		{
			isValid = 1;
		}
	}
	else if (strcmp(proceso, "GAMECARD") == 0)
	{
		//log_info(logger, "Conectado a GAMECARD por la cola %s", tipo_mensaje);
		ipAux = config_get_string_value(config, "IP_GAMECARD");
		puertoAux = config_get_string_value(config, "PUERTO_GAMECARD");
		if (perteneceAlArray(tipo_mensaje, gamecardCommands, 3) == 1)
		{
			isValid = 1;
		}
	}

	*ip = malloc(strlen(ipAux));
	*puerto = malloc(strlen(puertoAux));
	strcpy(*ip, ipAux);
	strcpy(*puerto, puertoAux);
	return isValid;
}



uint32_t perteneceAlArray(char *val, char *arr[], uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		if (strcmp(arr[i], val) == 0)
			return 1;
	}
	return 0;
}

uint32_t arraySize(void* arr[]) {
	uint32_t size = 0;
	while(arr[size] != NULL) {
		size++;
	}
	return size;
}

uint32_t arraySizeChar(char* arr[]) {
	uint32_t size = 0;
	while(arr[size] != NULL) {
		size++;
	}
	return size;
}

t_suscribe_gameboy* crearSuscribeGameboy(uint32_t con, uint32_t mq) {
	t_suscribe_gameboy* suscribe = malloc(sizeof(t_suscribe_gameboy));

	suscribe->conexion = con;
	suscribe->messageQueue = mq;

	return suscribe;
}

char* getNombreColaDeMensajes(uint32_t mq)
{
	switch (mq)
	{
		case NEW_POKEMON:
			return "NEW_POKEMON";
		case APPEARED_POKEMON:
			return "APPEARED_POKEMON";
		case CATCH_POKEMON:
			return "CATCH_POKEMON";
		case CAUGHT_POKEMON:
			return "CAUGHT_POKEMON";
		case GET_POKEMON:
			return "GET_POKEMON";
		case LOCALIZED_POKEMON:
			return "LOCALIZED_POKEMON";
		default:
			return "CODIGO DESCONOCIDO";
	}
}