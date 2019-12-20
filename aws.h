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
#include <string>
#include <iostream>
#include <iomanip>  

using namespace std;

#define TCP_PORT "24036"
#define UDP_PORT "23036"
#define HOST "localhost"
#define BACKLOG 10
#define PORT_SERVER_A "21036"
#define PORT_SERVER_B "22036"
#define MAXVEX 15
#define MAPID_SIZE 10
#define INF 999999


void pathfinding(char*, int, double &, double &, int[]);

void calculating(long long, int*, double, double, double &, double [], double []);

