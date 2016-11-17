//Gabriella Fontani, Ben Scott, Chad Eitel
#ifndef _PROJECT3_H_INCLUDED_   // if hasn't been included yet...
#define _PROJECT3_H_INCLUDED_   // #define this so the compiler knows it has been included

#define DATA_SIZE 1400	//this value should be <= the maximum size allowed through a socket connection in one "send"

#include <stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<cstdlib>
#include<strings.h>
#include<sstream>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>
#include<arpa/inet.h>
#include<signal.h>
#include<csignal>
#include<fstream> //include for talking to files.
#include <vector>
#include <boost/algorithm/string.hpp>

using namespace std;

//packet structure for simple char*
typedef struct{
	char data[DATA_SIZE]; //simple char* string
} packet;


void error(char const * msg);
int managerTcpPort;
int totalRouterNum;
void recv_msg(int sock, packet* recvd);
void send_msg(int sock, packet* to_send);

#endif
