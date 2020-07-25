#include "sockets.h"

/*CLIENT SIDE*/

int crear_conexion(char *ip, char* puerto) {
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	uint32_t socket_cliente = socket(server_info->ai_family,
			server_info->ai_socktype, server_info->ai_protocol);

	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
		return ERROR;
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

void liberar_conexion(uint32_t socket_cliente) {
	close(socket_cliente);
}

void establecerConexionBroker() {
    suscribeCaught = getSuscribe(CAUGHT_POKEMON);
    suscribeAppeared = getSuscribe(APPEARED_POKEMON);
    suscribeLocalized = getSuscribe(LOCALIZED_POKEMON);

    if (suscribeAppeared->conexion != -1) {
        printf("Se establecio una conexion con el Broker :D\n");
		printf("Desactivando modo default \n");
		int valorSem;
		if (sem_getvalue(&estaDesconectado, &valorSem) == 0){
            if(valorSem != 0){
				sem_wait(&estaDesconectado);
            }
        }
        mandarGET();
        if(pthread_create(&threadSUSCRIBE_CAUGHT,NULL,(void*)suscribe,(void*)suscribeCaught) != 0)
            printf("Error CAUGHT\n");

        if(pthread_create(&threadSUSCRIBE_APPEARED,NULL,(void*)suscribe,(void*)suscribeAppeared) != 0)
            printf("Error APPEARED\n");

        if(pthread_create(&threadSUSCRIBE_LOCALIZED,NULL,(void*)suscribe,(void*)suscribeLocalized) != 0)
            printf("Error LOCALIZED\n");

        pthread_detach(threadSUSCRIBE_CAUGHT);
        pthread_detach(threadSUSCRIBE_APPEARED);
        pthread_detach(threadSUSCRIBE_LOCALIZED);
    }
    else {
        printf("No se pudo establecer una conexion con el Broker :c\n");
		int valorSem;
		if (sem_getvalue(&estaDesconectado, &valorSem) == 0){
            if(valorSem == 0){
				printf("Activando modo default \n");
				modoDesconectado();
				sem_post(&estaDesconectado);
            }
        }
        printf("Intentando reconexion en %i segundos...\n", (uint32_t)config_get_int_value(config,"TIEMPO_RECONEXION"));
        sem_post(&mutexReconnect);
    }
}

void reconectarBroker() {
	while(1) {
		sem_wait(&mutexReconnect);
		sleep((uint32_t)config_get_int_value(config,"TIEMPO_RECONEXION"));
		printf("Intentando reconexion con Broker...\n");
		establecerConexionBroker();
	}
}
 
void detectarDesconexion() {
	while(1) {
		sem_wait(&detectorDesconexion);
		sem_wait(&detectorDesconexion);
		sem_wait(&detectorDesconexion);
		printf("Se desconecto el Broker :c\n");
		printf("Activando modo default \n");
		modoDesconectado();
		sem_post(&estaDesconectado);
		sem_post(&mutexReconnect);
	}
}

void enviarMensaje(t_paquete* paquete, uint32_t socket_cliente) {
	uint32_t sizePaquete = paquete->buffer->size + 2 * sizeof(uint32_t);
	void* stream = serializar_paquete(paquete, sizePaquete);
	send(socket_cliente,stream,sizePaquete,0);
	liberarPaquete(paquete);
	free(stream);
}

void mandarGET() {

	char* ip = config_get_string_value(config, "IP_BROKER");
    char* puerto = config_get_string_value(config, "PUERTO_BROKER");
    uint32_t conexion = crear_conexion(ip, puerto);

	for(uint32_t i=0;i<list_size(objetivoGlobal);i++) {
		t_pokemon_cantidad* poke = (t_pokemon_cantidad*)list_get(objetivoGlobal,i);
		if(poke->cantidad > 0) {
			//sendGET(poke->nombre);
			t_get_pokemon* getPokemon = crearGetPokemon(11, poke->nombre);
			t_paquete* paquete = serializar_getPokemon(getPokemon);
			printf("GET %s\n",poke->nombre);
			free(getPokemon);
			enviarMensaje(paquete, conexion);
		}
	}
	liberar_conexion(conexion);
}

void suscribe(void* structSuscribe) {
	t_suscribe* s = (t_suscribe*) structSuscribe;

	t_register_module* suscribe = crearSuscribe(s->messageQueue, idModule);
	t_paquete* paquete = serializar_registerModule(suscribe);
	printf("SUSCRIBE %i \n", s->messageQueue);
	free(suscribe);
	enviarMensaje(paquete, s->conexion);
	
	while(1) {
		serve_suscribe(&s->conexion);
	}
}

/*SERVER SIDE*/

void iniciar_servidor(void)
{
	uint32_t socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP, PUERTO, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

	printf("Iniciando servidor...\n");
    while(1)
    	esperar_cliente(socket_servidor);
}

void esperar_cliente(uint32_t socket_servidor)
{
	struct sockaddr_in dir_cliente;

	uint32_t tam_direccion = sizeof(struct sockaddr_in);

	uint32_t socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	pthread_join(thread, NULL);

}

void serve_client(uint32_t* socket)
{
	
	int cod_op;
	t_buffer* buffer;
	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	if(cod_op > 0 && cod_op < 11) {
		buffer = recibir_buffer(*socket);
	}
	process_request(cod_op, buffer, *socket);
}

void process_request(uint32_t cod_op, t_buffer* buffer, uint32_t cliente_fd) {
	switch (cod_op) {
		case APPEARED_POKEMON:
			{
				//log_info(logger, "SIZE BUFFER EN NEW: %i", buffer->size);
				// deserializar_appearedPokemon(buffer);
				t_appeared_pokemon* appearedPoke = deserializar_appearedPokemon(buffer);
				t_pokemon_posicion* poke = crearPokemonPosicion(appearedPoke->nombre, appearedPoke->posicion);
				printf("Llego un poke %s\n", poke->nombre);
				insertPokeEnMapa(poke);

				free(appearedPoke);
				free(buffer->stream);
				free(buffer);
				break;
			}
		case CAUGHT_POKEMON:
			{
				t_caught_pokemon* caughtPoke = deserializar_caughtPokemon(buffer);
				procesarMensajeCaught(caughtPoke);
				free(buffer->stream);
				free(buffer);
				break;
			}
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
	}
}

void serve_suscribe(uint32_t* socket)
{
	int cod_op;
	t_buffer* buffer;
	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	if(cod_op > 0 && cod_op < 11) {
		buffer = recibir_buffer(*socket);
		if(buffer->size > 1000) {
			cod_op = -1;
			free(buffer->stream);
			free(buffer);
		}
	}
	process_suscribe_request(cod_op, buffer, *socket);
}

void process_suscribe_request(uint32_t cod_op, t_buffer* buffer, uint32_t cliente_fd) {
	switch (cod_op) {
		case APPEARED_POKEMON:
			{
				//log_info(logger, "SIZE BUFFER EN NEW: %i", buffer->size);
				// deserializar_appearedPokemon(buffer);
				t_appeared_pokemon* appearedPoke = deserializar_appearedPokemon(buffer);
				t_pokemon_posicion* poke = crearPokemonPosicion(appearedPoke->nombre, appearedPoke->posicion);
				printf("Llego un poke %s\n", poke->nombre);
				insertPokeEnMapa(poke);
				t_acknowledgement* ack = crearAcknowledgement(idModule, appearedPoke->ID_mensaje_recibido,APPEARED_POKEMON);
				enviarAck(ack);
				free(appearedPoke);
				free(buffer->stream);
				free(buffer);
				break;
			}
		case CAUGHT_POKEMON:
			{
				t_caught_pokemon* caughtPoke = deserializar_caughtPokemon(buffer);
				printf("Me llego este mensaje con id correlativo %i\n", caughtPoke->ID_mensaje_original);
				procesarMensajeCaught(caughtPoke);
				t_acknowledgement* ack = crearAcknowledgement(idModule, caughtPoke->ID_mensaje_recibido,CAUGHT_POKEMON);
				enviarAck(ack);
				free(buffer->stream);
				free(buffer);
				break;
			}
		default:
			{
				sem_post(&detectorDesconexion);
				pthread_exit(&threadSUSCRIBE_CAUGHT);
				pthread_exit(&threadSUSCRIBE_APPEARED);
				pthread_exit(&threadSUSCRIBE_LOCALIZED);
				break;
			}
	}
}

t_buffer* recibir_buffer(uint32_t socket_cliente)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	int size;
    
    recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
    buffer->size = size;
	buffer->stream = malloc(buffer->size);
	recv(socket_cliente, buffer->stream, buffer->size, MSG_WAITALL);

	return buffer;
}

uint32_t escuchaBroker() {
	char* IP_BROKER = config_get_string_value(config,"IP_BROKER");
    char* PUERTO_BROKER = config_get_string_value(config,"PUERTO_BROKER");

	return crear_conexion(IP_BROKER, PUERTO_BROKER);
}

void enviarAck(t_acknowledgement* ack) {
	t_paquete* paquete = serializar_acknowledgement(ack);
	free(ack);
	uint32_t conexion = escuchaBroker();
	enviarMensaje(paquete, conexion);
	liberar_conexion(conexion);
}