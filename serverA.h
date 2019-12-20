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
#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <iomanip>  

using namespace std;

#define MAPID_SIZE 10
#define MAXVEX 15
#define INF 999999
#define MYPORT "21036" 
#define HOST "localhost"
#define FILEPATH "map.txt"


typedef struct  _graph
{
    int numvex;
    int numedge;
    double tranSpeed;
    double propSpeed;
    int matrix[MAXVEX][MAXVEX];
} Graph;

bool isNum(string tmp)
{
    for(int i = 0; i < tmp.size(); i++)
		if(tmp[i] > 57 || tmp[i] < 48)
			return false;
    return true;		
}

void readMap(map<string, Graph> &, string);

void dijkstra(string, map<string, Graph> &, int, int[]);
