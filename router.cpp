//Gabriella Fontani, Ben Scott, Chad Eitel
#include <project3.h>
#include <ospf.cpp>

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

	//try 13 ports
	for(int i=0; i<13; i++){
		myaddr.sin_port = htons(udpPort);
		if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) == 0)
			break;
		if(i==12)
			error("ERROR on binding"); //could not bind to 13 consecutive ports
		udpPort++;
	}
	return fd;
}

string collectNeighborInfo(int tcpSocket, int id){
	//loop while data isn't -1
	//receive neighbor information from tcp connection with manager
	string lspMessage = "";
	packet to_recv;
	recv_msg(tcpSocket, &to_recv);
	printf("router #%d received %s\n", id, to_recv.data);
	while(0 != strcmp(to_recv.data, "-1")){
		vector<string> neighborInfo;
		boost::split(neighborInfo, to_recv.data, boost::is_any_of(","));
		int neighborId = atoi(neighborInfo[0].c_str());
		int neighborUdp = atoi(neighborInfo[1].c_str());
		int weight = atoi(neighborInfo[2].c_str());
		//add weight to big table
		allNeighborWeights[id][neighborId] = weight;
		//add udp to neighbor vector
		myNeighborsPorts[neighborId] = neighborUdp;
		//add info to lspMessage
		lspMessage = lspMessage + neighborInfo[0] + "," + neighborInfo[2] + ",";
		recv_msg(tcpSocket, &to_recv);
		printf("router #%d received %s\n", id, to_recv.data);
	}
	return lspMessage;
}

//check if all of the values in the vecotor are true
bool allTrue(vector<bool> received){
	for(unsigned int i = 0; i < received.size(); i++){
		if(received[i] == false){
			return false;
		}
	}
	return true;
}

void sendLsp(string lsp, int id, int receivedFrom){
	int udpSocket = udp_listen(id);
	packet to_send;
	sprintf(to_send.data, lsp.c_str());
	for(unsigned int i = 0; i < myNeighborsPorts.size(); i++){
		if((myNeighborsPorts[i] != -1) && ((int)i != receivedFrom)){
			int port = myNeighborsPorts[i];
			send_udp_msg(udpSocket, port, &to_send);
			//printf("router %d sent to router %d on udp: %s\n", id, i, to_send.data);
		}
	}
	close(udpSocket);	
}

void receiveLsps(int udpSocket, int id){
	//keep track of which routers we have received isps from
	vector<bool> receivedLsp;
	for(int i = 0; i < totalRouterNum; i++){
		receivedLsp.push_back(false);
	}
	receivedLsp[id] = true;
	while(!allTrue(receivedLsp)){
		packet to_recv;
		recv_udp_msg(udpSocket, &to_recv);
		vector<string> neighborInfo;
		boost::split(neighborInfo, to_recv.data, boost::is_any_of(","));
		int from = atoi(neighborInfo[0].c_str());
		//printf("router %d received on udp: %s from : %d\n", id, to_recv.data, from);
		if(false == receivedLsp[from]){
			//fork
			int pid = fork();
			if(pid==0){
				//if parent
				//store data
				unsigned int counter = 1;
				int neighbor = 0;
				int weight = 0;
				while(counter < neighborInfo.size()-1){
					neighbor = atoi(neighborInfo[counter].c_str());
					counter++;
					weight = atoi(neighborInfo[counter].c_str());
					counter++;
					allNeighborWeights[from][neighbor] = weight;
				}
				receivedLsp[from] = true;
			}
			else if(pid>0){
				//if child
				//sendLSP to neighbors
				sendLsp(to_recv.data, id, from);
				exit(0);
			}
			else{
			//failed to fork
				error("failed to fork, exiting forcefully?");
			}
		}
		
	}
}

void exchangeLSP(int udpSocket, int id, string lsp){
	//fork
	int pid = fork();
	if(pid==0){
		//if parent
		//parent listen on udp for all routers’ info and build LSP table
		receiveLsps(udpSocket, id);
	}
	else if(pid>0){	
		//if child
		//wait x time (for other routers to be listening on udp)
		sleep(3);
		//child sends own LSP to all neighbors in the string form:
		//"my_number, neighbors_1, cost_1,...., neighbors_n, cost_n"
		sendLsp(lsp, id, -1);
		exit(0);
	}
	else{
	//failed to fork
		error("failed to fork, exiting forcefully?");
	}
}

//djikstras algorithm
//modifies the routing table built from the link state algorithm
void djikstrasAlgorithm(int id){
  
		//Routers wait for an LSP(link state packet) for every router

		//COPY FROM INTERNET!!!?
		vector<int> temp;
		temp.push_back(-1);
                routingTable.push_back(temp);
	
}

//takes the destination
//returns the next hop router id
int getNextHop(int destRouter){
	for(unsigned int i = 0; i < routingTable.size(); i++){
		if(destRouter == routingTable[i][0]){
			return routingTable[i][2];
		}
	}
	return -1;
}

//receive message
//forward message if not end router
//messages are in the format: from,to,message
void receiveAndForwardMessages(int id, int udpSocket, ofstream fileStream){
	//receive and parse the message
	packet to_recv;
	recv_udp_msg(udpSocket, &to_recv);
	vector<string> messageInfo;
	boost::split(messageInfo, to_recv.data, boost::is_any_of(","));
	int from = atoi(messageInfo[0].c_str());
	int to = atoi(messageInfo[1].c_str());
	string message = messageInfo[2];
	fileStream<<"Time: "<<currentDateTime()<<"\t\tReceived message \""<<message<<"\" from router "<<from<<"\n";
	//check destination
	if(to == id){
		//I am the destination
		//write final message to file
		
		fileStream<<"Time: "<<currentDateTime()<<"\t\tI am the last router, will not forward message.\n"; 
	}
	else{
		//I am not the destination
		//forward packet and write message
		int nextHop = getNextHop(to);
		int nextHopUdp = myNeighborsPorts[nextHop];
		send_udp_msg(udpSocket, nextHopUdp, &to_recv);
		fileStream<<"Time: "<<currentDateTime()<<"\t\tForwarding to router "<<nextHop<<"\n"; 
	}
}

/* listens to manager and collects what routers it needs to send
 * msgs too.  Fills the routersToSendMessagesTo vector.
 * vector holds only what routers, this router needs to send msgs too.
 * ie if vector holds 2,4,5 then this vector sends messages to routers 
 * 2 then 4 then 5.
 */
string collectMessagesToSendInfo(int tcpSocket, int udpSocket, int id){
	//loop while data isn't -1
	
	string messages = "";
	packet to_recv;
	recv_msg(tcpSocket, &to_recv);
	printf("messages router #%d received %s\n", id, to_recv.data);
	while(0 != strcmp(to_recv.data, "-1")){
          
		vector<string> messageInfo;
		boost::split(messageInfo, to_recv.data, boost::is_any_of(","));
                int fromRouter = atoi(messageInfo[0].c_str());
                int toRouter = atoi(messageInfo[1].c_str());
                printf("router: from %d to %d\n", fromRouter, toRouter);
                //send udp mesg to next router. port has to be next hop.
                
                int hop_port = myNeighborsPorts[toRouter]; //need to change this to get next hop.
                printf("sending udp from %d to %d port %d\n", id, toRouter, hop_port);
                send_udp_msg(udpSocket, hop_port, &to_recv); //sending on to next router
                
                routersToSendMessegesTo.push_back(toRouter); //add data to messages to send vector
                packet tempPacket;
                sprintf(tempPacket.data, "hello from router #%d, thanks for the data %d", id, udpPort);
                
                send_msg(tcpSocket, &tempPacket);//send ack msg
		recv_msg(tcpSocket, &to_recv);
                sleep(2);
        }
        return "";
}

void writeRoutingTableToFile(ofstream& myStream){
	//printf("printing to file\n");
	myStream<<"Routing table: \ndest\tweight\tnextHop\n";
	for(unsigned int i = 0; i < routingTable.size(); i++){
		for(unsigned int j = 0; j < routingTable[i].size(); j++){
			myStream<<routingTable[i][j]<<"\t";
		}
		myStream<<"\n";
	}
}

void writeMyNeighborsPortsToFile(ofstream& myStream){
	//printf("printing to file\n");
	myStream<<"My neighbors udp ports: \n";
	for(unsigned int i = 0; i < myNeighborsPorts.size(); i++){
		myStream<<i<<"\t";
	}
	myStream<<"\n";
	for(unsigned int i = 0; i < myNeighborsPorts.size(); i++){
		myStream<<myNeighborsPorts[i]<<"\t";
	}
	myStream<<"\n";
}

void writeAllNeighborWeightsToFile(ofstream& myStream){
	//printf("printing to file\n");
	myStream<<"All Router's neighbor's weights: \n";
	myStream<<"\t";
	for(unsigned int i = 0; i < allNeighborWeights.size(); i++){
		myStream<<i<<"\t";
	}
	myStream<<"\n";
	for(unsigned int i = 0; i < allNeighborWeights.size(); i++){
		myStream<<i<<"\t";
		for(unsigned int j = 0; j < allNeighborWeights[i].size(); j++){
			myStream<<allNeighborWeights[i][j]<<"\t";
		}
		myStream<<"\n";
	}
}

//main router method
void router(int id){
	//create file name based on id
	//should be id.out
	printf("id = %d\n", id);
	string sid = to_string(id);
	string filename = sid + ".out";
	//open ofstream to use for debugging and final stuff
	ofstream fileStream;
	fileStream.open(filename);
	//sets up udp socket
	int udpSocket = udp_listen(id);

	//set up tcp client connection with manager
	int tcpSocket = client_connect("localhost", managerTcpPort);
	//send message to manager with udp port
	packet tempPacket;
	sprintf(tempPacket.data, "hello from router #%d, my udp port is %d", id, udpPort);
	send_msg(tcpSocket, &tempPacket);

	//receive all neighbor info from router and fill out appropriate tables
	string lspMessage = collectNeighborInfo(tcpSocket, id);
	lspMessage = to_string(id) + "," + lspMessage;
	printf("%s\n", lspMessage.c_str());
    writeAllNeighborWeightsToFile(fileStream);
    writeMyNeighborsPortsToFile(fileStream);
    writeRoutingTableToFile(fileStream);
    exchangeLSP(udpSocket, id, lspMessage);
    djikstrasAlgorithm(id);
    sleep(3);
    cout<<collectMessagesToSendInfo(tcpSocket, udpSocket,  id)<< id <<endl;
    


        
        
	//wait for go ahead from manager: this will be the -1 received after the loop 
	//so don't wait for go ahead from master, just start after loop is done
	
	//Routers do link state algorithm to make the routing tables

	//Routers write their routing tables to their file
	
	//Routers send message to manager when done

	//Routers receive messages, look up info in routing table, and pass on message
	
	//When routers send and receive they write to their .out file
	
	fileStream.close();
	//exit router gracefully
	printf("exiting router %d\n", id);
	close(udpSocket);
	close(tcpSocket);
	exit(0);	
}

