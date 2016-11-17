//Gabriella Fontani, Ben Scott, Chad Eitel
#include <project3.h>

//client setup
int client_connect(const char* addr, int portno){
	struct sockaddr_in serv_addr;
	struct hostent *server;

	printf("Connecting to server... ");

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) error("ERROR opening socket");

	server = gethostbyname(addr);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
    }
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");

	printf("Connected to manager!\n");
	return sock;
}

//link state algorithm/djikstras
//returns the routing table built from the link state algorithm
vector<string> linkStateAlgorithm(vector<vector<int> > neighbors, int id, int udpSocket){
		//Routers wait for an LSP(link state packet) for every router

		//COPY FROM INTERNET!!!?
		vector<string> routingTable;
		return routingTable;
	
}

void writeRoutingTableToFile(int fileptr, vector<string> routingTable){

	
	//write the routing table to the .out file
	
}

//main router method
void router(int id){
	printf("starting router %d\n", id);

	//neighbor id, distance, port
	vector<vector<int> > neighbors;

	//set up tcp client connection with manager
	int tcpSocket = client_connect("localhost", managerTcpPort);

	packet tempPacket;


	const char* text = "hello! I'm a router.";
	memcpy(tempPacket.data, text, strlen(text));
	send_msg(tcpSocket, &tempPacket);
	
	//sets up udp socket
	int udpSocket = 0;
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
	
	//exit router gracefully
	printf("exiting router %d\n", id);
	close(udpSocket);
	close(tcpSocket);
	exit(0);	
}

