#include <unistd.h> 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int my_create_server_socket(char *port);
void print_address(const struct sockaddr * clt_addr, socklen_t addrlen);

typedef struct {
	char student_id[7];
	char text[2000]; //should be ‘\0’ terminated
} msg1_t;

typedef struct {
	char text[2000]; //should be ‘\0’ terminated
	char student_name[100]; //should be ‘\0’ terminated
} msg2_t;

int myReadBlock(int s, void *buf, int count) {
	int r, nread = 0;
	while( nread < count ) {
		r = read(s, buf+nread, count-nread);
	if(r <= 0)
		break;
	else
		nread += r;
	}
	return nread;
}

int main(int argc, char *argv[]){
  int new_socket_descriptor, socket_descriptor; 
  int n;
  struct sockaddr clt_addr;
  socklen_t addrlen;
 
      
  if(argc!=2) {
    printf("Usage: %s port_number\n", argv[0]);
    exit(1); 
  }

  
  //SIGPIPE is sent to the process if a write is made to a closed connection.
  //By default, SIGPIPE terminates the process. This makes the process to ignore the signal.
  signal(SIGPIPE, SIG_IGN);
       
  socket_descriptor = my_create_server_socket(argv[1]);

  while(1) {
    
    printf("Waiting connection\n");  
  
    addrlen = sizeof(clt_addr);
    //accept a new connection to a client
    
		
	new_socket_descriptor = accept(socket_descriptor, &clt_addr, &addrlen); 
	pid_t f = fork();
    if(f == 0){
		if(new_socket_descriptor < 0) {
		  perror("accept");
		  sleep(1);
		  continue;
		}
		
		print_address(&clt_addr, addrlen);
		msg1_t msg;
		int n = myReadBlock(new_socket_descriptor, &msg, sizeof(msg)); 
		//printf("%s\n%s\n",msg.student_id,msg.text);
		
		msg2_t res;
		strcpy(res.student_name, "Marvin Ryan Cerqueira de Teive e Argollo");
		strcpy(res.text, msg.text);
		printf("%s%s\n",res.text,res.student_name);
		int i = 0;
		char c;
		while(1){
		  c = res.text[i];
		  res.text[i] = toupper(c);
		  i++;
		  if(c == '\n' || c == '\0') break;
		}
		
		sleep(5);
		write(new_socket_descriptor, &res, sizeof(res));
		close(new_socket_descriptor);
		exit(1);
	}
  }
  
  return 0;
}

void print_address(const struct sockaddr * clt_addr, socklen_t addrlen) 
{
  char hostname[256];  
  char port[6];

  int n = getnameinfo(clt_addr, addrlen, hostname, sizeof(hostname), 
    port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV); 
  if(n != 0)
    printf("%s\n", gai_strerror(n));
  else
    printf("Connection from %s:%s\n", hostname, port);
}

int my_create_server_socket(char *port)
{
  int s, r;
  struct addrinfo hints, *a;

  memset (&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_flags = AI_PASSIVE;
  r = getaddrinfo(NULL, port, &hints, &a);
  if (r != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(r));
    exit(1);
  } 


  s = socket(PF_INET, SOCK_STREAM, 0);
  if(s == -1){
    perror("socket");
    exit(1);
  }


  //avoid bind errors if the port is still being used by previous connections
  int so_reuseaddr = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr));

  
  r = bind(s, a->ai_addr, a->ai_addrlen);
  if(r == -1) {
    perror("bind");
    exit(1);
  }    
  
  r = listen(s, 5); 
  if(r == -1) {
    perror("listen");
    exit(1);
  }   
  
  freeaddrinfo(a);
  return s;
}

