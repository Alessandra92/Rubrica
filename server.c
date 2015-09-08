#include <sys/socket.h>  //inclusione funzioni relative ai socket
#include <sys/types.h>// tipi primitivi e close()
#include <netdb.h> // x close()
#include <unistd.h> //x close()
#include <sys/errno.h>  //valori errno
#include <netinet/in.h> //def rappr. ind internet
#include <arpa/inet.h> // inet_aton etc.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  MAX_CODA 4

//invia i dati presenti in buff (vengono gestiti i residui) e ritorna il numero di byte inviati o -1 in caso di errore
int invio(int, const char*, int);

//riceve i dati e li salva in buff (vengono gestiti i residui) e ritorna 0 in caso di successo o -1 in caso di errore
int ricevo(int, char[],int);
        
void main(int argc, char *argv[]){int ssock,lclient,csock;
struct sockaddr_in saddr;
struct sockaddr client;
ssock=socket(AF_INET,SOCK_STREAM,0);
    if(ssock==-1){
        perror("ERRORE creazione socket");
        exit(1);
    }
    printf("Socket creato con successo\n");
         //BINDING Indirizzo Server
    memset(&saddr, 0, sizeof(saddr));  
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(4444); //assegno # di porto
    saddr.sin_addr.s_addr=INADDR_ANY; //accetta richieste su ogni interfaccia
    if(bind(ssock,(struct sockaddr*)&saddr,sizeof(saddr))==-1){
            perror("ERRORE bind");
            exit(1);
    }
    //LISTEN
    if((listen(ssock,MAX_CODA))==-1){
		perror("Errore nella chiamata listen");
		exit(1);
    }
    printf("server pronto a ricevere connessioni\n");
    
    while(1){
    memset(&client,0,sizeof(client));
    lclient=sizeof(client);    
    if((csock=accept(ssock, &client, &lclient))==-1){
                    perror("Errore nell'accept");
                    exit(1);	
        }
    printf("Connessione riuscita\n");
        
    }
    exit(0);
}



    
    
//invia i dati presenti in buff (vengono gestiti i residui) e ritorna il numero di byte inviati o -1 in caso di errore
int invio(int socket, const char *buff, int dim){
    int rimanenti,scritti;
    const char* buffer;
    buffer=buff;
    rimanenti=dim;
    while(rimanenti>0){
        if((scritti= send(socket, buffer, rimanenti, 0 ))<0){
            if(errno==EINTR)
                scritti = 0; // non è stato trasmesso nulla
            if(errno==EPIPE) //viene ricevuto anche un sigpipe
                perror("Connessione caduta\n");
                //------------------------------------------- GESTIONE USCITA!!!
        }          
        rimanenti-=scritti;
        buffer+=scritti;           
    }
    return(rimanenti);
}
    
    
//riceve i dati e li salva in buff (vengono gestiti i residui) e ritorna 0 in caso di successo o -1 in caso di errore
int ricevo(int socket, char buff[],int dim){
    char *buffer;
    int letti, rimanenti;
    rimanenti=dim;
    buffer=buff;
    while(rimanenti>0){
        if((letti=recv(socket,buffer,rimanenti,0))<0){
             if (errno == EINTR) {/* funzione interrotta da un segnale prima di aver potuto leggere qualsiasi dato. */
               letti = 0;
             //gestione segnale
             }
             else  return(-1);
         }
          else if(letti==0) break;  //raggiunto EOF
          rimanenti-=letti;
          buffer+=letti;
        }
        return (rimanenti);
}   //------------------------------>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>MANCANO ALARM!!!!!!!!!!!!!!!!!!!!
            
