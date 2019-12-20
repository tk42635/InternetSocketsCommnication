//Name: De Huo
//USC EE450 PROJECT
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h> 
#include <string>
#include <iostream>
#include <iomanip>  

using namespace std;

#define PORT_AWS "24036"
#define HOST "localhost"
#define MAPID_SIZE 10
#define INF 999999
#define MAXVEX 15

long long my_atoll(char *instr)
{
  long long retval;
  int i;

  retval = 0;
  for (; *instr; instr++) {
    retval = 10*retval + (*instr - '0');
  }
  return retval;
}

int main(int argc, char* argv[]){

	if (argc < 4)
	{
		cerr << "More arguments needed.";
		exit(-1);
	}
	char *mapID = argv[1];
	int start = atoi(argv[2]);
	long long fileSize = my_atoll(argv[3]);

	//set up TCP --from Beej
	int sockfd = 0;
	struct addrinfo hints, *servinfo, *p;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(HOST, PORT_AWS, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can----Beej
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect. \n");
		exit(0);
		return 2;
	}
	freeaddrinfo(servinfo); // all done with this structure
	printf("The client is up and running. \n");

	if (send(sockfd, mapID, MAPID_SIZE, 0) == -1) 
	{
	 	perror("send");
	 	exit(0);
	}
	if (send(sockfd, (char *)& start, sizeof start, 0) == -1)
	{
		perror("send");
		exit(0);
	}
	if (send(sockfd, (char *)& fileSize, sizeof fileSize, 0) == -1)
	{
		perror("send");
		exit(0);
	}

	cout << "The client has sent query to AWS using TCP over port " << PORT_AWS << "; start vertex " << start << "; map " << mapID << "; file size " << fileSize << "." << endl;



	int dist[MAXVEX];
	double tranDelay, propDelay[MAXVEX], end2endDelay[MAXVEX];

	if ((recv(sockfd, (char *) &dist, sizeof dist, 0)) == -1) 
	{
		perror("recv");
		exit(1);
	}
	if ((recv(sockfd, (char *) &tranDelay, sizeof tranDelay, 0)) == -1) 
	{
		perror("recv");
		exit(1);
	}
	if ((recv(sockfd, (char *) &propDelay, sizeof propDelay, 0)) == -1) 
	{
		perror("recv");
		exit(1);
	}
	if ((recv(sockfd, (char *) &end2endDelay, sizeof end2endDelay, 0)) == -1) 
	{
		perror("recv");
		exit(1);
	}

	cout << "The client has received results from AWS:" << endl;
	cout << "------------------------------------------------------" << endl;
	cout << "Destination   Min Length   Tt         Tp         Delay" << endl;
	cout << "------------------------------------------------------" << endl;
	for(int i = 0; i < MAXVEX; i++)
	{
		if (dist[i] != INF)
			cout << setw(3) << i << setw(14) << dist[i] << setw(16) << tranDelay << setw(10) << propDelay[i] << setw(12) << end2endDelay[i] << endl;
	}
	cout << "------------------------------------------------------" << endl;
}