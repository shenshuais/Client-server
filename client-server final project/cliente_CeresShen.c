/////////////////////////////////////////////////////////////////////////////////////
//                    NATIONAL AUTONOMOUS UNIVERSITY OF MEXICO                   //
//                               SCHOOL OF ENGINEERING                            //
//                          CLIENT-SERVER ARCHITECTURE PROJECT                    //
//                                   FINAL PROJECT                                //
//                                  SERVER PROGRAM                                //
//                            STUDENTS:                                          //
//                            - Ceres Martínez Hanna Sophia                       //
//                            - Shen Shuai                                        //
/////////////////////////////////////////////////////////////////////////////////////


 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <errno.h>
 #include <string.h>
 #include <netdb.h>
 #include <sys/types.h>
 #include <netinet/in.h>
 #include <sys/socket.h>

 #define MAXDATASIZE 100      

 #define MAXDATASIZE_RESP 60000 

 int main(int argc, char *argv[]){

  //Comands
  char comando[MAXDATASIZE];     
  int len_comando;                
  
  // Answers
  char buf[MAXDATASIZE_RESP];   
  int numbytes;                   
  
  int sockfd;                     
  struct hostent *he;             
  struct sockaddr_in cliente;     

  if (argc != 3) {                        
    fprintf(stderr,"usage: client hostname puerto\n");
    exit(1);                      
  }

  if ((he=gethostbyname(argv[1])) == NULL) {  
   perror("gethostbyname");
   exit(1);
  }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
   perror("socket");
   exit(1);
  }
//Settings
  cliente.sin_family = AF_INET;              
  cliente.sin_port = htons( atoi(argv[2]) );  
  cliente.sin_addr = *((struct in_addr *)he->h_addr);   
  memset(&(cliente.sin_zero), '\0',8);      


  if (connect(sockfd, (struct sockaddr *)&cliente, sizeof(struct sockaddr)) == -1) {
   perror("connect");
   exit(1);
  }



  while(1){

   
    fgets(comando,MAXDATASIZE-1,stdin);
    len_comando = strlen(comando) - 1;    
    comando[len_comando] = '\0';          
    printf("Comando: %s\n",comando);

    if(send(sockfd,comando, len_comando, 0) == -1) {
     perror("send()");
     exit(1);                            
    } else printf("Comando enviado\n");

    if(strcmp(comando,"exit")==0)
      break;     

  
    if ((numbytes=recv(sockfd, buf, MAXDATASIZE_RESP-1, 0)) == -1) {
     perror("recv");
     exit(1);
    }
    buf[numbytes] = '\0';               
    printf("Recibido:\n%s\n",buf);        
  }
  
  close(sockfd);

  return 0;
} 
