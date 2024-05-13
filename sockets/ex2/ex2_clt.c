#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>


void recv_server_reply1(int);
void recv_server_reply2(int);
int my_connect(char *servername, char *port);
void print_socket_address(int sd);

int main (int argc, char* const argv[]) {

  if(argc!=4) {
    printf("Usage: %s server_ip_address port_number\n",argv[0]);
    exit(1); 
  }
    
  char buffer[4096];  
  
  int fds = open(argv[3], O_RDONLY);
  if(fds == -1) {
    perror("open source");
    return 1;
  }
  while(1){
	  int n = read(fds, buffer, sizeof(buffer));
	  if(n <=  0) break;
  }	
  
  //connect to server
  int socket_descriptor = my_connect(argv[1], argv[2]);
  
  //send string to server
  write(socket_descriptor, buffer, strlen(buffer));
  
  return 0;
}

int my_connect(char *servername, char *port) {

  //get server address using getaddrinfo
  struct addrinfo hints;
  struct addrinfo *addrs;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  int r = getaddrinfo(servername, port, &hints, &addrs);
  if (r != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(r));
    exit(1);
  }
  
  //create socket
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1) {
    perror("socket");  
    exit(2);
  }
  
  //connect to server
  r = connect(s, addrs->ai_addr, addrs->ai_addrlen);
  if (r == -1) {
    perror("connect");  
    close(s);
    exit(3);
  }

  freeaddrinfo(addrs);  
  return s;
}

void print_socket_address(int sd)
{
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(addr);

  getsockname(sd, (struct sockaddr *) &addr, &addrlen);

  char hostname[256];  
  char port[6];

  int n = getnameinfo((struct sockaddr *) &addr, addrlen, hostname, sizeof(hostname), 
    port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV); 
  if(n != 0)
    printf("%s\n", gai_strerror(n));
  else
    printf("Socket address: %s:%s\n", hostname, port);  
}
