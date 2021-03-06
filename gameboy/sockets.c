#include "sockets.h"
#include "gameboy.h"

int crear_conexion(char *ip, char* puerto) {
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family,
			server_info->ai_socktype, server_info->ai_protocol);

	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen)
			== -1)
		log_error(gameboy_custom_logger, "Connect error");

	freeaddrinfo(server_info);

	return socket_cliente;
}

void liberar_conexion(int socket_cliente) {
	close(socket_cliente);
}

void enviarMensaje(t_paquete* paquete, uint32_t socket_cliente, char* nombreProceso) {
	int sizePaquete = paquete->buffer->size + 2 * sizeof(int);
	void* stream = serializar_paquete(paquete, sizePaquete);
	if(send(socket_cliente,stream,sizePaquete,MSG_CONFIRM) == -1)
	{
		log_error(gameboy_custom_logger, "Send error");
		//Desuscribir a ese gil
		liberar_conexion(socket_cliente);
		exit(1);
	}
	log_info(logger, "Conectado y enviando mensaje a %s", nombreProceso);
	liberarPaquete(paquete);
	free(stream);
}

void enviarMensaje_sinProceso(t_paquete* paquete, uint32_t socket_cliente) {
	int sizePaquete = paquete->buffer->size + 2 * sizeof(int);
	void* stream = serializar_paquete(paquete, sizePaquete);
	if(send(socket_cliente,stream,sizePaquete,MSG_CONFIRM) == -1)
	{
		log_error(gameboy_custom_logger, "Send error");
		//Desuscribir a ese gil
		liberar_conexion(socket_cliente);
		exit(1);
	}
	liberarPaquete(paquete);
	free(stream);
}

void modoSuscriptor(void* arg) {
	t_suscribe_gameboy* suscribe = (t_suscribe_gameboy*) arg;
	t_register_module* registerModule = crearSuscribe(suscribe->messageQueue, ID_MODULE); //TODO: Tomar del config
	t_paquete *paquete = serializar_registerModule(registerModule, SUBSCRIBE);

	free(registerModule);
    enviarMensaje(paquete, suscribe->conexion, "BROKER");
	
	log_info(logger, "Se ha suscrito a la cola %s", getNombreColaDeMensajes(suscribe->messageQueue));
	while(1) 
	{
		serve_client(&suscribe->conexion);
	}
}

void temporizador (void* tiempo) {
	uint32_t temp = (uint32_t) tiempo;
	sleep(temp);
	pthread_exit(&hiloSuscriptor);
}

void serve_client(uint32_t* socket)
{
	int cod_op;
	t_buffer* buffer;
	if(recv(*socket, &cod_op, sizeof(int), 0) < 0){
		cod_op = -1;
	}
	if(cod_op > 0 && cod_op < 11) {
		buffer = recibir_buffer(*socket);
		if(buffer->size > 1000 || buffer->size <= 0) {
			cod_op = -1;
			free(buffer->stream);
			free(buffer);
		}
	}
	process_request(buffer, cod_op, *socket);
}

void enviarAck(t_acknowledgement* ack) {
	t_paquete* paquete = serializar_acknowledgement(ack);
	free(ack);
	uint32_t respuestaBroker = crear_conexion(ipBroker, puertoBroker);
	enviarMensaje_sinProceso(paquete, respuestaBroker);
	liberar_conexion(respuestaBroker);
	return;
}

void process_request(t_buffer *buffer, uint32_t operation_cod, uint32_t socket_cliente) {
	t_acknowledgement* ack = NULL;
	pthread_t sendAck;
	
	switch (operation_cod)
	{
		case NEW_POKEMON:
		{
			t_new_pokemon* newPoke = deserializar_newPokemon(buffer);
			ack = crearAcknowledgement(ID_MODULE, newPoke->ID_mensaje_recibido, NEW_POKEMON);
	
			log_info(logger, "Se ha recibido un mensaje del BROKER por la cola NEW_POKEMON");

			break;
		}
		case APPEARED_POKEMON:
		{
			t_appeared_pokemon* appearedPoke = deserializar_appearedPokemon(buffer);
			ack = crearAcknowledgement(ID_MODULE, appearedPoke->ID_mensaje_recibido, APPEARED_POKEMON);

			log_info(logger, "Se ha recibido un mensaje del BROKER por la cola APPEARED_POKEMON");

			break;
		}
		case GET_POKEMON:
		{
			t_get_pokemon* getPoke = deserializar_getPokemon(buffer);
			ack = crearAcknowledgement(ID_MODULE, getPoke->ID_mensaje_recibido, GET_POKEMON);

			log_info(logger, "Se ha recibido un mensaje del BROKER por la cola GET_POKEMON");

			break;
		}
		case CAUGHT_POKEMON:
		{
			t_caught_pokemon* caughtPoke = deserializar_caughtPokemon(buffer);
			ack = crearAcknowledgement(ID_MODULE, caughtPoke->ID_mensaje_recibido, CAUGHT_POKEMON);

			log_info(logger, "Se ha recibido un mensaje del BROKER por la cola CAUGHT_POKEMON");

			break;
		}
		case LOCALIZED_POKEMON:
		{
			t_localized_pokemon* localizedPoke = deserializar_localizedPokemon(buffer);
			ack = crearAcknowledgement(ID_MODULE, localizedPoke->ID_mensaje_recibido, LOCALIZED_POKEMON);

			log_info(logger, "Se ha recibido un mensaje del BROKER por la cola LOCALIZED_POKEMON");

			break;
		}
		case CATCH_POKEMON:
		{
			t_catch_pokemon* catchPoke = deserializar_catchPokemon(buffer);
			ack = crearAcknowledgement(ID_MODULE, catchPoke->ID_mensaje_recibido, CATCH_POKEMON);

			log_info(logger, "Se ha recibido un mensaje del BROKER por la cola CATCH_POKEMON");

			break;
		}
		case MENSAJE_RECIBIDO:
		{
			t_id_mensaje_recibido* idMensajeEnviado = deserializar_idMensajeRecibido(buffer);

			log_info(gameboy_custom_logger, "Se ha recibido el id del mensaje enviado: %i", idMensajeEnviado->id_mensajeEnviado);
			liberar_conexion(socket_cliente);
			break;
		}
		case SUBSCRIBE:
		{
			t_suscripcion* suscripcion = deserializar_suscripcion(buffer);
			log_info(gameboy_custom_logger, "Id asignado en la suscripcion: %i", suscripcion->idModule);
			break;
		}
		default:
		{
			log_warning(gameboy_custom_logger, "Se corto la conexion con BROKER");
			exit(0);
			break;
		}	
	}
	if(ack != NULL)
	{
		pthread_create(&sendAck, NULL, (void*)enviarAck, ack);
		pthread_detach(sendAck);
	}
}

t_buffer *recibir_buffer(uint32_t socket_cliente)
{
	t_buffer *buffer = malloc(sizeof(t_buffer));
	int size;

	recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
	buffer->size = size;
	buffer->stream = malloc(buffer->size);
	recv(socket_cliente, buffer->stream, buffer->size, MSG_WAITALL);

	return buffer;
}