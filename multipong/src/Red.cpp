#include "Red.h"
#include <iostream>
#include <stdlib.h>     /* atoi */
#include <cstring>

#define MAX_LEN 512

Red::Red()
{
    //ctor
}

Red::~Red()
{
    //dtor
}


int Red::inicia(){
    /* Inicializamos la Red */
    if(SDLNet_Init()==-1) {
        std::cout << "SDLNet_Init: " << SDLNet_GetError() << std::endl;
        return -2;
    }else{
        std::cout << "Red Inicializada" << std::endl;
    }

    return 0;
}

int Red::iniciaServidor(int port){
    // create a listening TCP socket on port 9999 (server)
    IPaddress ip;

    std::cout << "Creando servidor : " << std::endl;

    if(SDLNet_ResolveHost(&ip,NULL,port)==-1) {
        std::cout << "SDLNet_ResolveHost: " << SDLNet_GetError() << std::endl;
        return -1;
    }

    tcpsock=SDLNet_TCP_Open(&ip);
    if(!tcpsock) {
        std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
        return -2;
    }

    std::cout << "Servidor creado : " << std::endl;

    return 0;
}

int Red::esperaClientes(int nclientes, int numeroPlayers, int playerNumInicial){
    connectedClients = 0;
    int playerNum = playerNumInicial;

    std::cout << "Esperando cliente : " << std::endl;

    //Esperar conexion de usuarios
    while(connectedClients<nclientes){
        clientes[connectedClients]=SDLNet_TCP_Accept(tcpsock);
        if(clientes[connectedClients]) {
            //Enviamos el n�mero de cliente
            std::cout << "Cliente conectado :" << connectedClients << std::endl;

            servidorEnviaNumeros(&clientes[connectedClients],numeroPlayers, playerNum);
            playerNum++;
            connectedClients++;
        }
    }

    return 0;
}

int Red::iniciaCliente(std::string host, int port){
    // connect to localhost at port 9999 using TCP (client)
    IPaddress ip;

    if(SDLNet_ResolveHost(&ip,host.c_str(),port)==-1) {
        std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
        return -1;
    }

    tcpsock=SDLNet_TCP_Open(&ip);
    if(!tcpsock) {
        std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
        return -1;
    }

    //Recibir numero cliente

    return 0;
}

int Red::envia(TCPsocket* cliente, char* msg){
    int len = strlen(msg)+1;
    int result=SDLNet_TCP_Send(*cliente,msg,len);
    if(result<len) {
        std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
        // It may be good to disconnect sock because it is likely invalid now.
        return -1;
    }

    //std::cout << "Envio Mensaje " << msg << std::endl;

    return 0;
}

int Red::recibe(TCPsocket* servidor, char * msg){
    int result;

    //std::cout << "Esperando Respuesta : " << std::endl;
    result=SDLNet_TCP_Recv(*servidor,msg,MAX_LEN);
    if(result<=0) {
        // An error may have occured, but sometimes you can just ignore it
        // It may be good to disconnect sock because it is likely invalid now.
        return -1;
    }else{
        //std::cout << "Mensaje recibido : " << msg << std::endl;
    }

    return 0;
}

int Red::clienteRecibeDatos(char* msg){
    if(recibe(&tcpsock,msg)>=0){
        //Recibimos el numero de jugador
        return 0;
    }else{
        return -1;
    }
}

int Red::clienteRecibeNumeros(int *numeroJugadores, int *jugador){
    if(recibe(&tcpsock,buffer)>=0){
        sscanf(buffer,"%d %d",numeroJugadores, jugador);
        std::cout << "Numero cliente recibido "<< *jugador << std::endl;
        //Recibimos el numero de jugador
        return 0;
    }else{
        return -1;
    }
}

int Red::servidorEnviaNumeros(TCPsocket * cliente, int numeroJugadores, int numeroCliente){
    sprintf(buffer,"%d %d",numeroJugadores, numeroCliente);

    int len = std::strlen(buffer)+1;
    int result=SDLNet_TCP_Send(*cliente,buffer,len);
    if(result<len) {
        printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
        // It may be good to disconnect sock because it is likely invalid now.
        return -1;
    }

    std::cout << "Envio numero cliente " << buffer << std::endl;

    return 0;
}



int Red::servidorEnviaDatosATodos(char* msg){

    int len = std::strlen(msg)+1;

    for(int i=0; i<connectedClients; i++){
        //std::cout << "ENVIO : " << msg << std::endl;
        int result=SDLNet_TCP_Send(clientes[i],msg,len);
        if(result<len) {
            std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
            // It may be good to disconnect sock because it is likely invalid now.
            //return -1;
        }
    }

    return 0;
}

int Red::clienteEnviaDireccion(int cliente, int direccion){
    sprintf(buffer,"%d %d",cliente, direccion);
    std::cout << "Cliente envia direccion " << buffer << std::endl;
    int len = strlen(buffer)+1;
    int result=SDLNet_TCP_Send(tcpsock,buffer,len);
    if(result<len) {
        std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
        // It may be good to disconnect sock because it is likely invalid now.
        //return -1;
    }

    //std::cout << "Envio Mensaje " << msg << std::endl;

    return 0;
}

int Red::servidorRecibeDatos(std::vector<Pala*> palas, float deltaTime){
    int direccion;
    int cliente;


    for(int i=0;i<connectedClients;i++){
        if(recibe(&clientes[i],buffer)>=0){
            //Recibimos datos de un cliente
            sscanf(buffer,"%d %d",&cliente, &direccion);
            std::cout << "Recibimos datos en servidor: " << buffer << std::endl;
            palas[cliente]->Update(deltaTime,(Direcction)direccion);

        }else{
            std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
        }
    }

    return 0;
}
