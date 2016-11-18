//Gabriella Fontani, Ben Scott, Chad Eitel
#include <project3.h>
#include <router.cpp>

//The table will have as many rows as there are routers
//and 3 columns: <routerNumber> | <UDP port> | <tcpinformation>
vector<vector<int> > routerAndPorts;
int managerTcpSock;

//print error message
void error(char const * msg)
{
	perror(msg);
	//kill all children and close all sockets?
	close(managerTcpPort);
	exit(1);
}

void send_msg(int sock, packet* to_send){
		int n;
		n = send(sock,reinterpret_cast<const char*>(to_send),sizeof(packet), 0);
			if (n < 0) error("ERROR writing to socket");
}

void recv_msg(int sock, packet* recvd){
	int n = recv(sock,reinterpret_cast<char*>(recvd),sizeof(packet), MSG_WAITALL);
	if (n < 0) error("ERROR reading from socket");
}

//server listen, returns listening socket fd
int server_bind_listen(int portno){
	int sock;
	struct sockaddr_in serv_addr;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	//try 13 ports
	for(int i=0; i<13; i++){
		serv_addr.sin_port = htons(portno);
		if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == 0)
			break;
		if(i==12)
			error("ERROR on binding"); //could not bind to 13 consecutive ports
		portno++;
	}

	listen(sock,11);
	managerTcpPort = portno;
	return sock;
}

//server setup, returns accepted socket fd
int server_accept(int sock){
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	clilen = sizeof(cli_addr);
	int newsock = accept(sock, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
	if (newsock < 0) error("ERROR on accept");

	//printf("Found a router! Waiting to receive...\n");
	return newsock;
}

 //returns the udp port of router
 int getRouterUdp(int router){
	return routerAndPorts[router][0];
 }
 
 //returns the tcp stuff of router
 int getRouterTcp(int router){
	return routerAndPorts[router][1];
 }
 
 //GABBY
 //method to create a table with number or routers and port numbers
 //will save a spave for ports but not fill it in (because we don't know the ports yet)
 void makeRouterTable(){
	 for(int i = 0; i < totalRouterNum; i++){
		 vector<int> router;
		 router.push_back(-1);
		 router.push_back(-1);
		 routerAndPorts.push_back(router);
	 }
 }
 
 void printRouterTable(){
	 printf("Router and port table: \nrouter#\tUdpPort\tTcpPort\n");
	 for(unsigned int i = 0; i < routerAndPorts.size(); i++){
		 printf("%d\t", i);
		 for(unsigned int j = 0; j < routerAndPorts[i].size(); j++){
			 printf("%d\t", routerAndPorts[i][j]);
		 }
		 printf("\n");
	 }
 }
 
 //get the router number from the message
 //get the udp port from the message
 //add the udp port to the table
 void addUdpFromRouterMessage(char* message, int router){
	 string msg(message);
	 int pos = msg.find_last_of(" \t\n");
	 string udpPort = msg.substr(pos+1);
	 routerAndPorts[router][0] = atoi(udpPort.c_str());
 }
 
 //CHAD
 //method to create all of the routers
 //fork to create all of the routers
 //in each child process call the router method from router
 void createRouters(){
	int pid=1;//begin as parent process
	//Create all the routers by forking new processes
	//loop
	for(int i=0;i<totalRouterNum; i++){
		//fork
		pid = fork();

		//if parent
		if(pid==0){
			//wait for connection and reply of each router after creation
			int tempRouterSock = server_accept(managerTcpSock);
			routerAndPorts[i][1] = tempRouterSock;
			packet tempPacket;
			recv_msg(tempRouterSock, &tempPacket);
			
			printf("manager has recvd msg: %s\n", tempPacket.data);
			addUdpFromRouterMessage(tempPacket.data, i);
			//receive tcp message from router containing router's udp port number
	
			//store router's udp port number in vector vector

			//for debugging only, will need to keep open eventually
			//close(tempRouterSock);
		}
		else if(pid>0){			
		//if child
			//keep track of child id's?
			//call the router method from router passing router id
			router(i);
			exit(0);
		}
		else{
		//failed to fork
			error("failed to fork, exiting forcefully?");
		}
	}
 }
 
 //send neighbor, neighbor's udp port, and the weight to router
 void sendInfoToRouter(string router, string neighbor, string weight){
	int neighborUdp = getRouterUdp(atoi(neighbor.c_str()));
	packet to_send;
	sprintf(to_send.data, "%s,%d,%s", neighbor.c_str(), neighborUdp, weight.c_str());
	int socket = getRouterTcp(atoi(router.c_str()));
	send_msg(socket, &to_send);
	printf("Manager sent to %s: %s\n", router.c_str(), to_send.data);
 }
 
 //GABBY
 //send the router neighbor information to each router, line by line
 void sendNeighborInformation(ifstream& fileptr){
	 if(fileptr.is_open()){
		//Reads neighbors from the file, sending info line by line to each router
		string line = "";
		while(getline(fileptr, line) && 0 != line.compare("-1")){
			vector<string> routerInfo;
			boost::split(routerInfo, line, boost::is_any_of(" "));
			if(3 != routerInfo.size()){
				error("the format of the input file is wrong for indicating neighbors");
			} 			
			//Send Neighbor information (neighbor id, link cost, UDP port number)
			sendInfoToRouter(routerInfo[0], routerInfo[1], routerInfo[2]);
			sendInfoToRouter(routerInfo[1], routerInfo[0], routerInfo[2]);
		} 
	 }
	 else{
		 error("fileptr is not open in sendNeighborInformation, exiting forecefully");
	 }
 }
 
 //BEN
//send messages to all routers according to file
 void sendMessages(ifstream& fileptr){
	 
	//loop
		//reads rest of file line by line
		//messages to router
		//sleeps between sending messages
	 
 }

void closeAllSockets(){
	for(int i = 0; i < totalRouterNum; i++){
		int tempRouterSock = routerAndPorts[i][1];
		close(tempRouterSock);
	}
	
}
 
int main(int argc, char* argv[]){
	//open file and start reading it
	//leave the file open!!!
	ifstream fileptr("exampleFile.txt");
	
	//read first number in file
	string line = "";
	getline(fileptr, line);
	totalRouterNum = atoi(line.c_str()); //hardcoded for debugging
	
	makeRouterTable();
	//printRouterTable();
	//setup TCP server
	int startingPort = 3360;
	managerTcpSock = server_bind_listen(startingPort);
	printf("manager listening on port: %d\n", managerTcpPort);
	
	//make all of the routers
	createRouters();
	printRouterTable();
	//send neighbor information
	sendNeighborInformation(fileptr);
	
	//maybe sleep to give routers time to figure out life?
	
	//Manager sends a “hey go ahead and start the link state algorithm” message to all of the routers

	//Manager waits for messages from all routers saying they are done with link state algorithm
	
	//Manager sends messages to all routers according to file
	sendMessages(fileptr);
	
	//Kill remaining child processes
	
	//close the file
	//wait()
	//exit()

	close(managerTcpSock);
	closeAllSockets();
	printf("exiting manager\n");
	exit(0);
}
