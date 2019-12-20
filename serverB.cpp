//Name: De Huo
//USC EE450 PROJECT
#include "serverB.h"

int main(){
	// set up UDP  -- From Beej
	int sockfd;
	int rv;
	struct addrinfo hints;  // the struct addrinfo have already been filled with relevant info
	struct addrinfo *servinfo; //point out the result
	struct sockaddr_storage their_addr;
	struct addrinfo *p;  //tempoary point
	socklen_t addr_len;


	memset(&hints, 0, sizeof hints);  // make sure the struct is empty
	hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_DGRAM; // UDP dgram sockets

	if ((rv = getaddrinfo(HOST, MYPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return NULL;
	}
	// loop through all the results and bind to the first we can----Beej
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("serverB: socket");
			continue;
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("serverB: bind");
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "serverA: failed to bind socket\n");
		return NULL;
	}
	freeaddrinfo(servinfo);
  	cout << "The Server B is up and running using UDP on port " << MYPORT << "." << endl;

  	while(1)
	{
		addr_len = sizeof their_addr;
		long long fileSize;
		double tranSpeed, propSpeed;


		int dist[MAXVEX];
		double propDelay[MAXVEX] = {0};
		double end2endDelay[MAXVEX] = {0};
		double tranDelay = 0;

		recvfrom(sockfd, (char *) &dist, sizeof dist, 0,(struct sockaddr *)&their_addr, &addr_len);
		recvfrom(sockfd, (char *) &fileSize, sizeof fileSize, 0,(struct sockaddr *)&their_addr, &addr_len);
		recvfrom(sockfd, (char *) &tranSpeed, sizeof tranSpeed, 0, (struct sockaddr *)&their_addr, &addr_len);
		recvfrom(sockfd, (char *) &propSpeed, sizeof propSpeed, 0, (struct sockaddr *)&their_addr, &addr_len);
		cout << "The Server B has received data for calculation:" << endl;
		cout << "* Propagation speed: " << propSpeed << "km/s;" << endl;
		cout << "* Transmission speed: " << tranSpeed << "Bytes/s;" << endl;

		for(int i = 0; i < MAXVEX; i++)
		{
			if(dist[i] != INF)
				cout << "* Path length for destination "<< i << ":" << dist[i] << endl;
		}	

		calculating(dist, fileSize, tranSpeed, propSpeed, tranDelay, propDelay, end2endDelay);
		
		sendto(sockfd, (char *) &tranDelay, sizeof tranDelay, 0,(struct sockaddr *) &their_addr, addr_len);
		sendto(sockfd, (char *) &propDelay, sizeof propDelay, 0, (struct sockaddr *) &their_addr, addr_len);
		sendto(sockfd, (char *) &end2endDelay, sizeof end2endDelay, 0, (struct sockaddr *) &their_addr, addr_len);
		cout << "The Server B has finished sending the output to AWS." << endl;
		cout << endl << endl;
	}
}


/* 
 * Calculate 3 kinds of delay time.
 * PRE: size is the length of dist[]
 * 		fileSize, tranSpeed, propSpeed > 0 
 * 
*/
void calculating(const int dist[], const long long fileSize, const double tranSpeed, const double propSpeed, 
	double &tranDelay, double propDelay[], double end2endDelay[])
{
	tranDelay = (long long)(round(fileSize / tranSpeed / BYTE_TO_BIT * 100)) / 100.0;
	for (int i = 0; i < MAXVEX; i++)
	{
		propDelay[i] = (long long)(round(dist[i] / propSpeed * 100)) / 100.0;
		end2endDelay[i] = propDelay[i] + tranDelay;
		if (dist[i] == 0 || dist[i] == INF)
			end2endDelay[i] = INF;
	}
	cout << "The Server B has finished the calculation of the delays:" << endl;
	cout << "---------------------" << endl;
	cout << "Destination     Delay" << endl;
	cout << "---------------------" << endl;
	for(int i = 0; i < MAXVEX; i++)
	{
		if (end2endDelay[i] != INF)
			cout << setw(3) << i << setw(19) << end2endDelay[i] << endl;
	}
	cout << "---------------------" << endl;
}