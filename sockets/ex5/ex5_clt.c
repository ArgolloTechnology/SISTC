#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

void recv_server_reply1(int);
void recv_server_reply2(int);
int my_connect(char *servername, char *port);
void print_socket_address(int sd);

typedef struct {
	char student_id[7];
	char text[2000]; //should be ‘\0’ terminated
} msg1_t;

typedef struct {
	char text[2000]; //should be ‘\0’ terminated
	char student_name[100]; //should be ‘\0’ terminated
} msg2_t;

int myReadLine1(int s, char *buf, int count){
	int r, n=0;
	if(count<=0)
		return 0;
	else if(count==1) {
		buf[0] = 0;
		return 0;
	} else
		--count; //leave space for '\0'
	do {
		r = read(s, buf+n, 1);
		if(r==1)
		++n;
	} while( r==1 && n < count && buf[n-1]!='\n');
	buf[n] = '\0';
	return n;
}

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

int main (int argc, char* const argv[]) {

  if(argc!=3) {
    printf("Usage: %s server_ip_address port_number\n",argv[0]);
    exit(1); 
  } 
  
  //connect to server
  int socket_descriptor = my_connect(argv[1], argv[2]);
  
  

    printf("escreva a mensagem:\n");
	msg1_t msg;
	strcpy(msg.student_id, "1220700");
	char s[2000];
	fgets(s, sizeof(s), stdin);
	int nBytes = strlen(s);
	char nBytesString[16];
	sprintf(nBytesString, "%d", nBytes);
	strcpy(msg.text, nBytesString);
	strcat(msg.text, "\n");
	strcat(msg.text, s);
  write(socket_descriptor, &msg, sizeof(msg));
  /*
  char linha1[2000] = "", linha2[2000] = "";
  int n1 = myReadLine1(socket_descriptor, linha1, 2000);
  int n2 = myReadLine1(socket_descriptor, linha2, 2000);
  printf("%s",buffer);
  */
  msg2_t res;
  int nbytes = myReadBlock(socket_descriptor, &res, sizeof(res));
  printf("Mensagem: %sNome: %s\n", res.text, res.student_name);
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
