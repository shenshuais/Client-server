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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define MAXDATASIZE 100
#define LENGTH 60000

int main(int argc, char *argv[]) {
  int numbytes;
  char buf[MAXDATASIZE];

  int server_fd, client_fd;

  struct sockaddr_in server;
  struct sockaddr_in client;

  int server_size;
  int client_size;

  // Create a socket for the server
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  // Set socket options
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)  {
    perror("Server-setsockopt() error!");
    exit(1);
  } else printf("Server-setsockopt is OK...\n");

  // Configure the server's address and port
  server.sin_family = AF_INET;
  server.sin_port = htons(atoi(argv[1]));
  server.sin_addr.s_addr = INADDR_ANY;
  memset(&(server.sin_zero), '\0', 8);

  server_size = sizeof(server);
  
  // Bind the server socket to its address and port
  if (bind(server_fd, (struct sockaddr *)&server, server_size) == -1) {
    perror("bind");
    exit(1);
  }

  // Put the server in listening mode to look for connections, and set the connection queue to 1
  // Wait for a single connection
  if (listen(server_fd, 1) == -1) {
    perror("listen");
    exit(1);
  }

  client_size = sizeof(client);

  // When establishing a connection with a client, create a socket for the connection and add the client information
  if ((client_fd = accept(server_fd, (struct sockaddr *)&client, &client_size)) == -1) {
    perror("accept");
    exit(1);
  }

  // Display the IP address of the client that connected to the server
  printf("server: client connection from %s\n", inet_ntoa(client.sin_addr));

  while (1) {
    // Receive the data sent by the client through its socket and store it in buf
    if ((numbytes = recv(client_fd, buf, MAXDATASIZE - 1, 0)) == -1) {
      perror("recv");
      exit(1);
    }

    // Check if the received command is "exit" to exit the loop
    if (strcmp(buf, "exit") == 0) break;

    buf[numbytes] = '\0'; // Add a null character at the end of the command to work with a valid string

    // Open the "abk.txt" file in write mode
    FILE *file = fopen("abk.txt", "w");
    if (file == NULL) {
      perror("fopen failed");
      exit(1);
    }

    // Execute the command stored in buf and redirect the output to a pipe
    FILE *pipe = popen(buf, "r");
    if (pipe == NULL) {
      perror("popen failed");
      exit(1);
    }

    // Read the output of the command from the pipe and write it to the file
    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), pipe)) > 0) {
      fwrite(buffer, 1, bytesRead, file);
    }

    // Close the file and the pipe
    fclose(file);
    pclose(pipe);

    // Consider the command "ps -e -o pid,ppid,user,stat,command"

    // Read the "abk.txt" file and send its information to the client
    char *fs_name = "abk.txt";
    char sdbuf[LENGTH];
    printf("[Server] Sending output to the Client...\n");
    FILE *fs = fopen(fs_name, "r");
    if (fs == NULL) {
      printf("ERROR: File %s not found on server.\n", fs_name);
      exit(1);
    }

    bzero(sdbuf, LENGTH);
    int fs_block_sz; 
    // Read the information from the file containing the output of the executed command
    while ((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs)) > 0) {
      // Send the contents of the buffer to the client
      if (send(client_fd, sdbuf, fs_block_sz, 0) < 0) {
        printf("ERROR: sending command output to the client\n");
        exit(1);
      }
      bzero(sdbuf, LENGTH);
    }
    // Close the file again
    fclose(fs);
    printf("Ok sent to client!\n");
  }

  // Close the descriptors of the client and the server
  close(client_fd);
  close(server_fd);

  // Disable receiving and sending data through the server socket
  // Close both reading and writing of the server socket
  shutdown(server_fd, SHUT_RDWR);

  // Exit with exit(0) which means successful termination
  exit(0);

  return 0;
}

