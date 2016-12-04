//Gabriella Fontani, Ben Scott, Chad Eitel
#include <project3.h>

int udpPort;

//client setup
int client_connect(const char* addr, int portno){
	struct sockaddr_in serv_addr;
	struct hostent *server;

	//printf("Connecting to server... ");

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

	//printf("Connected to manager!\n");
	return sock;
} 

//link state algorithm/djikstras
//returns the routing table built from the link state algorithm
vector<string> linkStateAlgorithm(vector<vector<int>> neighbors, int id, int udpSocket){
  
		//Routers wait for an LSP(link state packet) for every router

		//COPY FROM INTERNET!!!?
		vector<string> routingTable;
                string tommy = "I am a routing table";  //for testing remove
                routingTable.push_back(tommy);
		return routingTable;
	
}

void writeRoutingTableToFile(string fileName, vector<string> routingTable){
  ofstream myStream;
  myStream.open(fileName);
  printf("printing to file\n");
  for(unsigned int i = 0; i<routingTable.size(); ++i){
  // cout<<"routing table i " << i << " " << routingTable[i] <<"\n";
    myStream<<routingTable[i]<<"\t";
    myStream<<"\n";
  }
  myStream.close();
	//write the routing table to the .out file
}

int udp_listen(int id){
	int fd;
	udpPort = managerTcpPort + id;
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket");
		return 0;
	}
	struct sockaddr_in myaddr;
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(udpPort); //set port

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}
	return fd;
}

//main router method
void router(int id){

	//sets up udp socket
	int udpSocket = udp_listen(id);

	//set up tcp client connection with manager
	int tcpSocket = client_connect("localhost", managerTcpPort);
	//send message to manager with udp port
	packet tempPacket;
	sprintf(tempPacket.data, "hello from router #%d, my udp port is %d", id, udpPort);
	send_msg(tcpSocket, &tempPacket);

	//neighbor id, port, distance/weight
	vector<vector<int> > neighbors;
	
	//loop while data isn't -1
	//receive neighbor information from tcp connection with manager
       
	packet to_recv;
	recv_msg(tcpSocket, &to_recv);
	printf("router #%d received %s\n", id, to_recv.data);
        
        printf("second recieve\n");
        sleep(10);
        packet msg_data;
	recv_msg(tcpSocket, &msg_data);
        printf("msg router #%d recieved %s\n", id, msg_data.data);
        
        
	//wait for go ahead from manager: this will be the -1 received after the loop 
	//so don't wait for go ahead from master, just start after loop is done
	
	//Routers do link state algorithm to make the routing tables
	vector<string> routingTable = linkStateAlgorithm(neighbors, id, udpSocket);
	
	//create file name based on id
	//should be id.out
        printf("id = %d\n", id);
        string sid = to_string(id);
	string filename = sid + ".out";
        
	//Routers write their routing tables to their file
    
        writeRoutingTableToFile(filename, routingTable);
	
	//Routers send message to manager when done

	//Routers receive messages, look up info in routing table, and pass on message
	
	//When routers send and receive they write to their .out file
	
	//exit router gracefully
	printf("exiting router %d\n", id);
	close(udpSocket);
	close(tcpSocket);
	exit(0);	
}

