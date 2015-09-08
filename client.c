#include <sys/socket.h>  // inclusione funzioni relative ai socket
#include <sys/types.h> // inclusione tipi primitivi
#include <stdlib.h>  //strtol
#include <sys/errno.h>  //valori errno
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/signal.h>

int csock;
char *ssAddress;
char *ssPort;
char *garbage;
short int port; 
struct sockaddr_in server;
struct hostent *he;
char esito[2];
char scelta[10];

typedef struct{
    char user[32];
    char pass[32];
}credenziali;

#define TRUE 1
#define FALSE 0

//Richiede dati autenticazione fino a che non rispettano i canoni imposti
void autenticazione(credenziali);

//legge una riga da terminale senza il carattere \n e ritorna la misura della stringa o -1 in caso di errore
int leggi(char*, int);

//invia i dati presenti in buff (vengono gestiti i residui) e ritorna il numero di byte inviati o -1 in caso di errore
int invio(int, const char*, int);
    
//riceve i dati e li salva in buff (vengono gestiti i residui) e ritorna il numero di byte ricevuti o -1 in caso di errore
int ricevo(int, char[],int);
/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------MAIN------------------------------------------------------------------------------------------------------------*/



void main(int argc, char *argv[]){
        credenziali cred;
        printf("Avvio client con pid: %d\n",getpid());
	/*ParseLnCmd(argc, argv, ssAddress, ssPort);
	port = strtol(ssPort, garbage, 0);  
	if ( *garbage ){
		printf("client: porta non riconosciuta.\n");
		exit(EXIT_FAILURE);
	}*/  
	autenticazione(cred);
        //CREAZIONE SOCKET
        csock=socket(AF_INET,SOCK_STREAM,0);
        if(csock==-1){
           perror("Errore nella creazione socket");
           exit(1);
        }
                printf("creazione socket avvenuta con successo\n");

        //COMPILAZIONE INDIRIZZO SERVER
	server.sin_family=AF_INET;
	server.sin_port=htons(4444);
        server.sin_addr.s_addr=inet_addr("127.0.0.1");
	/*if ( inet_aton(szAddress, &server.sin_addr) <0 ){                                     //CONTROLLARE SE con =0 da errore
		printf("client: indirizzo IP non valido.\nclient: risoluzione nome...\n");
		if ((he=gethostbyname(szAddress)) == NULL){
			printf("fallita.\n");
  			exit(-1);
		}
		printf("riuscita.\n\n");
		server.sin_addr = *((struct in_addr *)he->h_addr);
    
        } */
	
        if(connect(csock,(struct sockaddr*)&server, sizeof(server))==-1){
            perror("Errore nella connect");
            exit(-1);}
                printf("connessione socket avvenuta con successo\n");
        
                                                     //CONNESSIONE OK
        //--------------------------------------------------------------------INVIO DATI AUTENTICAZIONE
        if(invio(csock,&cred,sizeof(credenziali))==-1){
             perror("Errore nella send\n");
             exit(-1);
        }
        printf("inviati i dati per l'autenticazione: ");
        //---------------------------------------------------------------------RICEZIONE ESITO AUTENTICAZIONE
        if(ricevo(csock,esito,sizeof(esito))==-1){
             perror("Errore nella ricezione dei dati dell'utente\n");
             exit(-1);
        }
        if(esito[0]=='1') {
             printf("autenticazione riuscita!\n");
             cicla=0;
        }
                                          //se indirizzo e/o password sono sbagliati si può scegliere di reinserire i dati
        else{
             close(csock);
             if(esito[0]=='0') printf("nome inesistente\n");
             if(esito[0]=='2') printf("password errata\n");
             do{
                  printf("Vuoi tentare nuovamente l'accesso?(si/no)\n");
                  if(leggi(scelta,sizeof(scelta))==-1){
                        perror("Errore nella lettura della scelta\n");
                        exit(-1);
                  }   
/*                  
                  if(strcmp(scelta,"si")==0) {
                        cicla=1;
                        valido=1;
                  }
                  else{
                        if(strcmp(scelta,"no")==0) {
                            exit(0);
                        }
                        else {
                            printf("Comando non valido.\n"); 
                            valido=0;
                            }}

//-------------------------------------------------DA--------------------RIVEDERE------------------------------------------------ */

                
                
                
                
                
                
                
                
        exit(0);
}


/*-------------------------------------------------------------FUNZIONI AUSILIARIE------------------------------------------------------------------------------------------------*/


void autenticazione(credenziali up){
 int ancora;
 memset((void*)&up,0,sizeof(up));
 do{ 
   ancora=FALSE;
   printf("inserire nome utente(massimo 31 caratteri): ");
   if(leggi(up.user,32)==-1){
    printf("Errore nella lettura del nome: riprova\n");
    ancora=TRUE;}
 }while(ancora);
 do{
   ancora=FALSE;
   printf("inserire password(massimo 31 caratteri): ");
   if(leggi(up.pass,32)==-1){
    printf("Errore nella lettura della password: riprova\n");
    ancora=TRUE;}
 }while(ancora);
}

//legge una riga da terminale senza il carattere \n e ritorna la misura della stringa o -1 in caso di errore
int leggi(char *buff, int dim){
   memset(buff,0,sizeof(buff));
   if(fgets(buff ,dim, stdin)==NULL)
      return -1;
   if(buff[strlen(buff)-1]!='\n'){
     //necessario flush dell'input
     char ch;
     while ((ch=getchar()) != EOF && ch!='\n' );}
     else buff[strlen(buff)-1]='\0';
   return strlen(buff);
}

//invia i dati presenti in buff (vengono gestiti i residui) e ritorna 0 in caso di successo o -1 in caso di errore
int invio(int socket, const char *buff, int dim){
    int rimanenti,scritti;
    const char *buffer;
    buffer=buff;
    rimanenti=dim;
    while(rimanenti>0){
        if((scritti= send(socket, buffer, rimanenti, 0 ))<0){
            if(errno==EINTR)                                 //ricevuto signal
                scritti = 0; // non è stato trasmesso nulla
            if(errno==EPIPE) //viene ricevuto un sigpipe
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
}
        
             
        
    















    
