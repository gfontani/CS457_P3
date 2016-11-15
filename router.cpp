//Gabriella Fontani, Ben Scott, Chad Eitel
#include <project3.h>


//link state algorithm/djikstras
//returns the routing table built from the link state algorithm
vector<string> linkStateAlgorithm(vector<vector<int>> neighbors, int id, int udpSocket){
		//Routers wait for an LSP(link state packet) for every router

		//COPY FROM INTERNET!!!?
	
}

void writeRoutingTableToFile(int fileptr, vector<string> routingTable){

	
	//write the routing table to the .out file
	
}

//main router method
void router(int id, int managerPort){
	//neighbor id, distance, port
	vector<vector<int>> neighbors;

	//set up tcp client connection with manager
	
	//sets up udp socket
	udpSocket = 0;
	//send message to manager with udp port
	
	//receive neighbor information from tecp connection with manager
	
	//wait for go ahead from manager
	
	//Routers do link state algorithm to make the routing tables
	vector<string> routingTable = linkStateAlgorithm(neighbors, id, udpSocket);
	
	//create file name based on id
	//should be id.out
	string filename = "";
	
	//open write to file
	int fileptr = 0;
	
	//Routers write their routing tables to their file
	writeRoutingTableToFile(fileptr, routingTable);
	
	//Routers send message to manager when done

	//Routers receive messages, look up info in routing table, and pass on message
	
	//When routers send and receive they write to their .out file
	
	
}

