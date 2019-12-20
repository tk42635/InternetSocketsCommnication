//Name: De Huo
//USC EE450 PROJECT
#include "serverA.h"

int main(){

	map<string, Graph> mapIdx;
	readMap(mapIdx, FILEPATH);

	// set up UDP  -- From Beej
	int sockfd;
	int rv;
	struct addrinfo hints;	 // the struct addrinfo have already been filled with relevant info
	struct addrinfo *servinfo; //point out the result
	struct sockaddr_storage their_addr;
	struct addrinfo *p;  //tempoary point
	socklen_t addr_len;


	memset(&hints, 0, sizeof hints);  // make sure the struct is empty
	hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_DGRAM; // UDP dgram sockets

	if ((rv = getaddrinfo(HOST, MYPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 0;
	}

	// loop through all the results and bind to the first we can----Beej
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("serverA: socket");
			continue;
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("serverA: bind");
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "serverA: failed to bind socket\n");
		return 0;
	}
	freeaddrinfo(servinfo);
	cout << "The Server A is up and running using UDP on port " << MYPORT << endl;


	while(1){
		addr_len = sizeof their_addr;
		char mapID[MAPID_SIZE + 1];
		int start = 0;
		

		recvfrom(sockfd, mapID, MAPID_SIZE, 0,(struct sockaddr *)&their_addr, &addr_len);
		recvfrom(sockfd, (char *)& start, sizeof start, 0,(struct sockaddr *)&their_addr, &addr_len);
		cout << "The ServerA has received input for finding shortest paths: starting vertex " << start << " of map " << mapID << "." << endl;

		int dist[MAXVEX];
		dijkstra(mapID, mapIdx, start, dist);


		double tranSpeed = mapIdx.at(mapID).tranSpeed;
		double propSpeed = mapIdx.at(mapID).propSpeed;

		sendto(sockfd, (char *)  &dist, sizeof dist, 0,(struct sockaddr *) &their_addr, addr_len);
		sendto(sockfd, (char *) &tranSpeed, sizeof tranSpeed, 0, (struct sockaddr *) &their_addr, addr_len);
		sendto(sockfd, (char *) &propSpeed, sizeof tranSpeed, 0, (struct sockaddr *) &their_addr, addr_len);
		
		cout << "The Server A has sent shortest paths to AWS." << endl;
		cout << endl << endl;
	}
}

/* 
 * Build maps from map.txt and store them in a map container so that all can be accessed by mapID.
 * PRE: filePath is a string path to map.txt
 * 
*/
void readMap(map<string, Graph> &mapIdx, string filePath)
{
	fstream myfile;
  	myfile.open(filePath);
	if(!myfile.is_open())
	{
		cerr << "File not found." << endl;
		exit(-1);
	}
	else
	{
		bool end = false;
		int numMaps = 0;
		string mapID, nextFirst;
		myfile >> nextFirst;
		while(!end)
		{
			mapID = nextFirst;  //get mapID of a new map
			Graph newGraph;
			set<int> vexIdx;
			newGraph.numedge = 0;
			newGraph.numvex = 0;
			for (int i = 0; i < MAXVEX; i++)
				for (int j = 0; j < MAXVEX; j++)
					newGraph.matrix[i][j] = INF;
				
			myfile >> newGraph.propSpeed >> newGraph.tranSpeed; //get propagation speed and transmission speed
			
			//loop to get all edges(start, end, distance) of this map
			while (1) 
			{
				if(myfile >> nextFirst)
				{
					if(!isNum(nextFirst))
						break;
					int start, end, dist;
					start = atoi(nextFirst.c_str());
					myfile >> end >> dist;
					vexIdx.insert(start);
					vexIdx.insert(end);
					newGraph.matrix[start][end] = dist;
					newGraph.matrix[end][start] = dist;
					newGraph.numedge++;
				}
				else
				{
					end = true;
					break;
				}
				
			}
			newGraph.numvex = vexIdx.size();

			mapIdx.insert(pair<string, Graph>(mapID, newGraph));
			numMaps++;
		}
		cout << "The Server A has constructed a list of " << numMaps << " maps:" << endl;
		cout << "---------------------------------" << endl;
		cout << "Map ID   Num Vertices   Num Edges" << endl;
		cout << "---------------------------------" << endl;
		map<string, Graph>::iterator iter;
		for(iter = mapIdx.begin(); iter != mapIdx.end(); iter++) 
        	cout << setw(3) << iter->first << setw(10) << iter->second.numvex << setw(15) << iter->second.numedge << endl;
		cout << "---------------------------------" << endl;
	}
}

/* 
 * Djikstra algorithm to calculate all shortest paths from starting node to the other nodes.
 * PRE: mapID is a valid key in mapIdx
 * 		start is a valid node in that graph
 * 
*/
void dijkstra(string mapID, map<string, Graph> &mapIdx, int start, int dist[])
{
	Graph G = mapIdx.at(mapID);
	const int numvex = G.numvex;
	int i, j, k;
	int min;
	int tmp;
	int flag[MAXVEX], prev[MAXVEX];

	for (i = 0; i < MAXVEX; i++)
	{
		flag[i] = 0;              
		prev[i] = 0;              
		dist[i] = G.matrix[start][i];
	}

	flag[start] = 1;
	dist[start] = INF;


	for (i = 1; i < MAXVEX; i++)
	{
		min = INF;
		for (j = 0; j < MAXVEX; j++)
		{
			if (flag[j] == 0 && dist[j] < min)
			{
				min = dist[j];
				k = j;
			}
		}

		flag[k] = 1;

		for (j = 0; j < MAXVEX; j++)
		{
			tmp = (G.matrix[k][j] == INF ? INF : (min + G.matrix[k][j])); 
			if (flag[j] == 0 && (tmp < dist[j]))
			{
				dist[j] = tmp;
				prev[j] = k;
			}
		}
	}
	cout << "The Server A has identified the following shortest paths:" << endl;
	cout << "------------------------" << endl;
	cout << "Destination   Min Length" << endl;
	cout << "------------------------" << endl;
	for(int i = 0; i < MAXVEX; i++)
	{
		if(dist[i] != INF)
			cout << setw(3) << i << setw(14) << dist[i] << endl;
	}
	cout << "------------------------" << endl;

}
