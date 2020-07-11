#include "messageQueueManager.h"

t_message_queue* crearMessageQueue(uint32_t mq_cod)
{
    t_message_queue* newMessageQueue = (t_message_queue*)malloc(sizeof(t_message_queue));

	switch (mq_cod)
	{
		case NEW_POKEMON:
			strcpy(newMessageQueue->name, "NEW_POKEMON");
			break;
		case APPEARED_POKEMON:
			strcpy(newMessageQueue->name, "APPEARED_POKEMON");
			break;
		case GET_POKEMON:
			strcpy(newMessageQueue->name, "GET_POKEMON");
			break;
		case CAUGHT_POKEMON:
			strcpy(newMessageQueue->name, "CAUGHT_POKEMON");
			break;
		case LOCALIZED_POKEMON:
			strcpy(newMessageQueue->name, "LOCALIZED_POKEMON");
			break;
		case CATCH_POKEMON:
			strcpy(newMessageQueue->name, "CATCH_POKEMON");
			break;
	}

    newMessageQueue->mq_cod = mq_cod;
    newMessageQueue->mensajes = list_create();
    newMessageQueue->subscribers = list_create();
	sem_init(&newMessageQueue->s_hayMensajes, 0, 0);
	sem_init(&newMessageQueue->s_haySuscriptores, 0, 0);

    return newMessageQueue;
}

t_message* crearMessage(void* mensajeRecibido) {
    //TODO: Asignar ID a mensaje
    t_message* mensajeStruct = malloc(sizeof(t_message));
    mensajeStruct->mensaje = mensajeRecibido;
	mensajeStruct->id = asignarMessageId();
	mensajeStruct->idCorrelativo = -1;
    mensajeStruct->suscriptoresConfirmados = list_create();

    return mensajeStruct;
}

t_message_queue* getMessageQueueById(uint32_t id) {
	switch (id)
	{
		case NEW_POKEMON:
			return newPokemonMessageQueue;
			break;
		case APPEARED_POKEMON:
			return appearedPokemonMessageQueue;
			break;
		case GET_POKEMON:
			return getPokemonMessageQueue;
			break;
		case CAUGHT_POKEMON:
			return caughtPokemonMessageQueue;
			break;
		case LOCALIZED_POKEMON:
			return localizedPokemonMessageQueue;
			break;
		case CATCH_POKEMON:
			return catchPokemonMessageQueue;
			break;
		default:
		return NULL;
	}
}

t_list* getSuscriptoresByMessageQueueId(uint32_t id){
    t_message_queue* messageQueue = getMessageQueueById(id);
    t_list* suscriptores = list_create();

    for(uint32_t i = 0; i < list_size(messageQueue->subscribers); i++){
        list_add(suscriptores, list_get(messageQueue->subscribers,i));
    }

    return suscriptores;
}

void processMessage(t_buffer *buffer, uint32_t operation_cod, uint32_t socket_cliente) 
{
	switch (operation_cod)
	{
		case SUBSCRIBE:
		{
			t_register_module* registerModule = deserializar_registerModule(buffer);
			subscribeNewModule(socket_cliente, registerModule->messageQueue);
			t_message_queue* messageQueue = getMessageQueueById(registerModule->messageQueue);
			log_info(logger, "Se ha suscripto un nuevo modulo id: %i a %s", 
				socket_cliente, messageQueue->name);
			free(registerModule);
			break;
		}
		case NEW_POKEMON:
		{
			t_message_queue* messageQueue = getMessageQueueById(operation_cod);
			t_new_pokemon* newPoke = deserializar_newPokemon(buffer);
			t_message* message = crearMessage(newPoke);
			log_info(logger, "Se ha recibido un mensaje del cliente %i en la cola %s", socket_cliente, messageQueue->name);
			addMessageToMQ(message, messageQueue);

			//TODO: Probar si funciona
			t_id_mensaje_recibido* idAsignado = crearIdMensajeRecibido(message->id);		
			t_paquete* paquete = serializar_idMensajeRecibido(idAsignado);
			enviarMensaje(paquete, socket_cliente);

			break;
		}
		case APPEARED_POKEMON:
		{
			t_message_queue* messageQueue = getMessageQueueById(operation_cod);
			t_appeared_pokemon* appearedPoke = deserializar_appearedPokemon(buffer);
			t_message* message = crearMessage(appearedPoke);
			log_info(logger, "Se ha recibido un mensaje del cliente %i en la cola %i", socket_cliente, operation_cod);
			addMessageToMQ(message, messageQueue);
			//TODO: Informar al creador de mensaje el ID
			break;
		}
		case GET_POKEMON:
		{
			t_message_queue* messageQueue = getMessageQueueById(operation_cod);
			t_get_pokemon* getPoke = deserializar_getPokemon(buffer);
			t_message* message = crearMessage(getPoke);
			log_info(logger, "Se ha recibido un mensaje del cliente %i en la cola %i", socket_cliente, operation_cod);
			addMessageToMQ(message, messageQueue);
			//TODO: Informar al creador de mensaje el ID
			break;
		}
		case CAUGHT_POKEMON:
		{
			t_message_queue* messageQueue = getMessageQueueById(operation_cod);
			t_caught_pokemon* caughtPoke = deserializar_caughtPokemon(buffer);
			t_message* message = crearMessage(caughtPoke);
			log_info(logger, "Se ha recibido un mensaje del cliente %i en la cola %i", socket_cliente, operation_cod);
			addMessageToMQ(message, messageQueue);
			//TODO: Informar al creador de mensaje el ID
			break;
		}
		case LOCALIZED_POKEMON:
		{
			t_message_queue* messageQueue = getMessageQueueById(operation_cod);
			t_localized_pokemon* localizedPoke = deserializar_localizedPokemon(buffer);
			t_message* message = crearMessage(localizedPoke);
			log_info(logger, "Se ha recibido un mensaje del cliente %i en la cola %i", socket_cliente, operation_cod);
			addMessageToMQ(message, messageQueue);
			//TODO: Informar al creador de mensaje el ID
			break;
		}
		case CATCH_POKEMON:
		{
			t_message_queue* messageQueue = getMessageQueueById(operation_cod);
			t_catch_pokemon* catchPoke = deserializar_catchPokemon(buffer);
			t_message* message = crearMessage(catchPoke);
			log_info(logger, "Se ha recibido un mensaje del cliente %i en la cola %i", socket_cliente, operation_cod);
			addMessageToMQ(message, messageQueue);
			//TODO: Informar al creador de mensaje el ID
			break;
		}
	}
}

void subscribeNewModule(uint32_t idNewModule, uint32_t mq_cod)
{
	t_message_queue* messageQueue = getMessageQueueById(mq_cod);
	
	pthread_mutex_lock(&messageQueue->s_subscribers);
		list_add(messageQueue->subscribers, (void *)idNewModule);
	pthread_mutex_unlock(&messageQueue->s_subscribers);
}

void addMessageToMQ(t_message* message, t_message_queue* messageQueue)
{
	pthread_mutex_lock(&messageQueue->s_mensajes);
		list_add(messageQueue->mensajes, message);
	pthread_mutex_unlock(&messageQueue->s_mensajes);
	
	//Se crea un hilo que se encargue de insertarlo en la caché
	pthread_t cacheMessageThread;
	pthread_create(&cacheMessageThread, NULL, (void *)cacheMessage, message);
	pthread_detach(cacheMessageThread);
}

void sendMessageFromQueue(t_message* message, uint32_t socket_cliente)
{
	log_info(broker_custom_logger, "Mensaje id %i enviado a cliente %i", message->id, socket_cliente);
	log_info(broker_custom_logger, "Mensaje id %i removido de la cola", message->id);
}

void dispatchMessagesFromMQ(t_message_queue* messageQueue)
{
	t_message* message = malloc(sizeof(t_message));
	while(true)
	{
		log_info(broker_custom_logger, "%s: Dispatch messages a la espera", messageQueue->name);
		sem_wait(&messageQueue->s_haySuscriptores);
		sem_wait(&messageQueue->s_hayMensajes);

		pthread_mutex_lock(&messageQueue->s_mensajes);
			if(list_size(messageQueue->mensajes) > 0) {
				message = (t_message*)list_get(messageQueue->mensajes, 0);
			} 
		pthread_mutex_unlock(&messageQueue->s_mensajes);

		pthread_mutex_lock(&messageQueue->s_subscribers);
			uint32_t currentSubscriber;
			for(uint32_t i = 0; i < list_size(messageQueue->subscribers); i++) {
				currentSubscriber = (uint32_t)list_get(messageQueue->subscribers, i);
				sendMessageFromQueue(message, currentSubscriber);
			}
		pthread_mutex_unlock(&messageQueue->s_subscribers);


		sem_post(&messageQueue->s_hayMensajes);
	}
	//free(message);
}

void cacheMessage(t_message* message)
{
	log_info(logger, "Agregando mensaje id %i a la cache...", message->id);
	sleep(2);
	//TODO: Agregar mensaje a la cache
	log_info(logger, "Mensaje id %i agregado a la cache", message->id);
}

void inicializarCounterMessageId()
{
    globalMessageCounterId = 1;
	return;
}

uint32_t asignarMessageId()
{
	pthread_mutex_lock(&s_counterMessageId);
		globalMessageCounterId++;
	pthread_mutex_unlock(&s_counterMessageId);
	
    return globalMessageCounterId;
}