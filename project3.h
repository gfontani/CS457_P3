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

//packet structure for initial lsps
typedef struct{
	int id; //id of router that sends the message
	vector<int> neighbors; //the neighbors of router that sends the message
} udp_packet;


void error(char const * msg);
int managerTcpPort;
int totalRouterNum;
//The UDP ports of my neighbors. -1 if not a neighbor
vector<int> myNeighborsPorts;
//table with the weights of all the neighbors of all the routers
//-1 if there is no neighbor
vector<vector<int>> allNeighborWeights;
//list of distance to other nodes and next hop
//format: destination | weight | nextHop
vector<vector<int>> routingTable;
vector<int> routersToSendMessegesTo;
void recv_msg(int sock, packet* recvd);
void send_msg(int sock, packet* to_send);
void recv_udp_msg(int sock, packet* recvd);
void send_udp_msg(int sock, int port, packet* to_send);

#endif
