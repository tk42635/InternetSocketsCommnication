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
#include <string>
#include <iostream>
#include <math.h>
#include <iomanip>  

using namespace std;

#define MYPORT "22036"
#define HOST "localhost"
#define MAXVEX 15
#define BYTE_TO_BIT 8
#define SEC_TO_MSEC 1000
#define INF 999999

void calculating(const int[], const long long, const double, const double, 
	double &, double [], double []);
