//Name: De Huo
//USC EE450 PROJECT
#include "aws.h"

int main(){
	//set up TCP --from Beej
	int sockfd, newfd, numbytes;  
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; 
    socklen_t sin_size;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(HOST, TCP_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(-1);
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("server: socket");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
				== -1) {
			perror("setsockopt");
			exit(1);
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		exit(-1);
	}
	freeaddrinfo(servinfo); // all done with this structure

	//listen
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
	
	printf( "The AWS is up and running. \n");


	while(1){
		
		sin_size = sizeof their_addr;	
		newfd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
		if (newfd == -1) {
			perror("accept");			
			exit(1);
		}


		char mapID[MAPID_SIZE + 1];
		int start;
		long long fileSize;
		if ((numbytes = recv(newfd, mapID, MAPID_SIZE, 0)) == -1) 
		{
			perror("recv");
			exit(1);
		}
		if ((numbytes = recv(newfd, (char *)& start, sizeof start, 0)) == -1) 
		{
			perror("recv");
			exit(1);
		}
		if ((recv(newfd, (char *)& fileSize, sizeof fileSize, 0 )) == -1) 
		{
			perror("recv");
			exit(1);
		}
		cout << "The AWS has received map ID " << mapID << ", start vertex " << start << " and file size " << fileSize << " from the client using TCP over port " << TCP_PORT  << endl;
		int arr[MAXVEX];
		double tranSpeed, propSpeed;
		pathfinding(mapID, start, tranSpeed, propSpeed, arr);

		int dist[MAXVEX];
		for(int i = 0; i < MAXVEX; i++)
		{
			dist[i] = arr[i];
		}	
		
		double tranDelay = 0;
		double propDelay[MAXVEX], end2endDelay[MAXVEX];

		calculating(fileSize, dist, tranSpeed, propSpeed, tranDelay, propDelay, end2endDelay);


		send(newfd, (char *) &dist, sizeof dist, 0);
		send(newfd, (char *) &tranDelay, sizeof tranDelay, 0);
		send(newfd, (char *) &propDelay, sizeof propDelay, 0);
		send(newfd, (char *) &end2endDelay, sizeof end2endDelay, 0);
		cout << "The AWS has sent calculated delay to client using TCP over port " << TCP_PORT << endl;
		cout << endl << endl;
		close(newfd); 

	}
	
}

/* 
 * Communicate with server A using UDP and get shortest paths from it.
 * PRE: mapID is valid, 
 * 		start is a valid start node 
 * 
*/
void pathfinding(char* mapID, int start, double &tranSpeed, double &propSpeed, int arr[]) {
	int mysock;
	struct addrinfo hints, *servinfo, *p;
	int rv;

	//set up UDP -- from Beej
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(HOST, PORT_SERVER_A, &hints, &servinfo))
		!= 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(-1);
	}
	// loop through all the results and make a socket----Beej
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((mysock = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
			== -1) {
			perror("talker: socket");
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "talker: failed to bind socket\n");
		exit(-1);
	}

	if (sendto(mysock, mapID, MAPID_SIZE, 0, p->ai_addr, p->ai_addrlen) == -1)
	{
	 	perror("send");
	 	exit(0);
	}
	if (sendto(mysock, (char *)& start, sizeof start, 0, p->ai_addr, p->ai_addrlen) == -1)
	{
	 	perror("send");
	 	exit(0);
	}
	cout << "The AWS has sent map ID and starting vertex to server A using UDP over port " << UDP_PORT << endl;


	int dist[MAXVEX];

	if ((recvfrom(mysock, (char *) &dist, sizeof dist, 0, NULL, NULL)) == -1) 
	{
		perror("recv");
		exit(1);
	}
	for(int i = 0; i < MAXVEX; i++)
	{
		arr[i] = dist[i];
	}

	if ((recvfrom(mysock, (char *) &tranSpeed, sizeof tranSpeed, 0, NULL, NULL)) == -1) 
	{
		perror("recv");
		exit(1);
	}
	if ((recvfrom(mysock, (char *) &propSpeed, sizeof propSpeed, 0, NULL, NULL)) == -1) 
	{
		perror("recv");
		exit(1);
	}
	cout << "The AWS has received shortest path from server A: " << endl;
	cout << "------------------------" << endl;
	cout << "Destination   Min Length" << endl;
	cout << "------------------------" << endl;
	for(int i = 0; i < MAXVEX; i++)
	{
		if(MAXVEX != INF)
			cout << setw(3) << i << setw(14) << dist[i] << endl;
	}
	cout << "------------------------" << endl;
}

/* 
 * Communicate with server B using UDP and get 3 kinds of delay time for each path from it.
 * PRE: size is the length of arr[],
 * 		fileSize, tranSpeed, propSpeed > 0 
 * 
*/
void calculating(long long fileSize, int arr[], double tranSpeed, double propSpeed,
	double &tranDelay, double propDelay[], double end2endDelay[]) 
{
	int mysock;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	
	//set up UDP -- from Beej
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;


	if ((rv = getaddrinfo(HOST, PORT_SERVER_B, &hints, &servinfo))
		!= 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(-1);
	}
	// loop through all the results and make a socket----Beej
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((mysock = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
			== -1) {
			perror("talker: socket");
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "talker: failed to bind socket\n");
		exit(-1);
	}

	int dist[MAXVEX];
	for(int i = 0; i < MAXVEX; i++)
	{
		dist[i] = arr[i];
	}	


	if (sendto(mysock, (char *) &dist, sizeof dist, 0, p->ai_addr, p->ai_addrlen) == -1)
	{
	 	perror("send");
	 	exit(0);
	}
	if (sendto(mysock, (char *) &fileSize, sizeof fileSize, 0, p->ai_addr, p->ai_addrlen) == -1)
	{
	 	perror("send");
	 	exit(0);
	}
	if (sendto(mysock, (char *) &tranSpeed, sizeof tranSpeed, 0, p->ai_addr, p->ai_addrlen) == -1)
	{
	 	perror("send");
	 	exit(0);
	}
	if (sendto(mysock, (char *) &propSpeed, sizeof propSpeed, 0, p->ai_addr, p->ai_addrlen) == -1)
	{
	 	perror("send");
	 	exit(0);
	}
	
	cout << "The AWS has sent path length, propagation speed and transmission speed to server B using UDP over port " << UDP_PORT << endl;

	double propDelay_[MAXVEX], end2endDelay_[MAXVEX];
	if ((recvfrom(mysock, (char *) &tranDelay, sizeof tranDelay, 0, NULL, NULL)) == -1)
	{ 
		perror("recv");
		exit(1);
	}
	if ((recvfrom(mysock, (char *) &propDelay_, sizeof propDelay_, 0, NULL, NULL)) == -1)
	{ 
		perror("recv");
		exit(1);
	}
	if ((recvfrom(mysock, (char *) &end2endDelay_, sizeof end2endDelay_, 0, NULL, NULL)) == -1) 
	{
		perror("recv");
		exit(1);
	}

	for(int i = 0; i < MAXVEX; i++)
	{
		propDelay[i] = propDelay_[i];
		end2endDelay[i] = end2endDelay_[i];
	}

	cout << "The AWS has received delays from server B:" << endl;
	cout << "-----------------------------------------" << endl;
	cout << "Destination   Tt         Tp         Delay" << endl;
	cout << "-----------------------------------------" << endl;
	for(int i = 0; i < MAXVEX; i++)
	{
		if (end2endDelay[i] != INF)
			cout << setw(3) << i << setw(17) << tranDelay << setw(10) << propDelay[i] << setw(12) << end2endDelay[i] << endl;
	}
	cout << "-----------------------------------------" << endl;

}