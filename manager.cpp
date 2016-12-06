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

//copied from: http://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
string currentDateTime() {
	timeval curTime;
	gettimeofday(&curTime, NULL);
	int milli = curTime.tv_usec / 1000;

	char buffer [80];
	strftime(buffer, 80, "%H:%M:%S", localtime(&curTime.tv_sec));

	char currentTime[88] = "";
	sprintf(currentTime, "%s:%d", buffer, milli);
	return currentTime;
}

void send_msg(int sock, packet* to_send){
		int n;
		n = send(sock,reinterpret_cast<const char*>(to_send),sizeof(packet), 0);
			if (n < 0) error("ERROR writing to socket");
}

void recv_msg(int sock, packet* recvd){
        while(true){
	int n = recv(sock,reinterpret_cast<char*>(recvd),sizeof(packet), MSG_WAITALL);
	if (n < 0) error("ERROR reading from socket");
        if (n > 0){break;}
        }
}

int recv_udp_msg(int sock, packet* recvd){
	struct sockaddr_in remaddr; /* remote address */ 
	socklen_t addrlen = sizeof(remaddr); /* length of addresses */
	int n = recvfrom(sock, reinterpret_cast<char*>(recvd), sizeof(packet), 0, (struct sockaddr *)&remaddr, &addrlen);
	return n;
}

void send_udp_msg(int sock, int port, packet* to_send){
	struct hostent *hp; /* host information */ 
	struct sockaddr_in servaddr; /* server address */ 
	
	/* fill in the server's address and data */ 
	memset((char*)&servaddr, 0, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(port); 
	
	/* look up the address of the server given its name */ 
	hp = gethostbyname("localhost"); 
	if (!hp) { 
		error("could not obtain address of localhost"); 
	} 
	
	/* put the host's address into the server address structure */ 
	memcpy((void *)&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length);
	
	//send message
	int n = sendto(sock, reinterpret_cast<const char*>(to_send), strlen(reinterpret_cast<const char*>(to_send)), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (n < 0) error("ERROR writing to socket");
}

//server listen, returns listening socket fd
int server_bind_listen(int portno){ //portno = port number
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
			error("ERROR on server binding"); //could not bind to 13 consecutive ports
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
 //creates a table with number or routers and port numbers
 //will save a space for ports but not fill it in (because we don't know the ports yet)
 //initializes the tables that the routers use to the correct size full of -1
 void makeAllTables(){
	//initialize routerAndPorts table
	for(int i = 0; i < totalRouterNum; i++){
	 vector<int> router;
	 router.push_back(-1);
	 router.push_back(-1);
	 routerAndPorts.push_back(router);
	}

	//initialize allNeighborWeights table. Will be n by n
	for(int i = 0; i < totalRouterNum; i++){
		vector<int> router;
		for(int j = 0; j < totalRouterNum; j++){
			router.push_back(-1);
		}
		allNeighborWeights.push_back(router);
	}

	//initialize myNeighborsPorts. will be size n
	for(int i = 0; i < totalRouterNum; i++){
	 myNeighborsPorts.push_back(-1);
	}

	//initialize routingTable. will be n by 3.
	for(int i = 0; i < totalRouterNum; i++){
	 vector<int> router;
	 router.push_back(-1);
	 router.push_back(-1);
	 router.push_back(-1);
	 routingTable.push_back(router);
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
 void createRouters(ofstream& managerFileStream){
	int pid=1;//begin as parent process
	//Create all the routers by forking new processes
	//loop
	for(int i=0;i<totalRouterNum; i++){
		//fork
		pid = fork();
		//if parent
		if(pid==0){
			managerFileStream<<"Time: "<<currentDateTime()<<" Forked to create router "<<i<<"\n";
			//wait for connection and reply of each router after creation
			int tempRouterSock = server_accept(managerTcpSock);
			routerAndPorts[i][1] = tempRouterSock;
			packet tempPacket;
			bzero(tempPacket.data, DATA_SIZE);
			recv_msg(tempRouterSock, &tempPacket);
			managerFileStream<<"Time: "<<currentDateTime()<<" Received message: "<<tempPacket.data<<"\n";
			addUdpFromRouterMessage(tempPacket.data, i);
		}
		else if(pid>0){			
		//if child
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
 void sendInfoToRouter(string router, string neighbor, string weight, ofstream& managerFileStream){
	int neighborUdp = getRouterUdp(atoi(neighbor.c_str()));
	packet to_send;
	bzero(to_send.data, DATA_SIZE);
	sprintf(to_send.data, "%s,%d,%s", neighbor.c_str(), neighborUdp, weight.c_str());
	int socket = getRouterTcp(atoi(router.c_str()));
	send_msg(socket, &to_send);
	managerFileStream<<"Time: "<<currentDateTime()<<" Sent message to "<<router<<": "<<to_send.data<<"\n";
        
 }
 
 //GABBY
 //send the router neighbor information to each router, line by line
  void sendNeighborInformation(ifstream& fileptr, ofstream& managerFileStream){
	 if(fileptr.is_open()){
		//Reads neighbors from the file, sending info line by line to each router
		string line = "";
		while(getline(fileptr, line) && 0 != line.compare("-1")){ //returns 0 if line and "-1" are equal.
			vector<string> routerInfo;
			boost::split(routerInfo, line, boost::is_any_of(" "));
			if(3 != routerInfo.size()){
				error("the format of the input file is wrong for indicating neighbors");
			} 			
			//Send Neighbor information (neighbor id, link cost, UDP port number)
			sendInfoToRouter(routerInfo[0], routerInfo[1], routerInfo[2], managerFileStream);
			sendInfoToRouter(routerInfo[1], routerInfo[0], routerInfo[2], managerFileStream);
		} 
		//Manager sends a “hey go ahead and start the link state algorithm” message to all of the routers
		//send -1 to all routers
		for(int i = 0; i < totalRouterNum; i++){
			int socket = getRouterTcp(i);
			packet to_send;
			bzero(to_send.data, DATA_SIZE);
			sprintf(to_send.data, "-1");
			send_msg(socket, &to_send);
			managerFileStream<<"Time: "<<currentDateTime()<<" Sent message to "<<i<<": "<<to_send.data<<"\n";
		}
	 }
	 else{
		 error("fileptr is not open in sendNeighborInformation, exiting forecefully");
	 }
 }
 
 //BEN
//read the second half of the file and notify routers
//what other routers they need to send msgs too.
void sendMessages(ifstream& fileptr, ofstream& managerFileStream){
	if(fileptr.is_open()){
		string line = "";
		int udpSocket = udp_listen(-1);
		//reads the second part of the file.
		while(getline(fileptr, line) && 0 != line.compare("-1")){
			vector<string> fromTo;
			boost::split(fromTo, line, boost::is_any_of(" "));
			string fromRouter = fromTo[0];
			string toRouter = fromTo[1];
			int initialRouterPort = routerAndPorts[stoi(fromRouter)][0];
			//printf("Manager: intial router port %d", initialRouterPort);
			//printf("from*** %s to %s\n", fromRouter.c_str(), toRouter.c_str());
			packet router_msg;
			bzero(router_msg.data, DATA_SIZE);
			sprintf(router_msg.data, "-1,%s,%s", toRouter.c_str(), line.c_str());
			//printf("sending mesg packet[%s] to %s \n", router_msg.data, fromRouter.c_str());
			send_udp_msg(udpSocket, initialRouterPort, &router_msg);
			managerFileStream<<"Time: "<<currentDateTime()<<" Sent message to "<<fromRouter<<": "<<router_msg.data<<"\n";
			//cout<<"receiving message"<<endl;
			//sleeps between sending messages
			sleep(2);
		}

		for(int i = 0; i < totalRouterNum; i++){
			//sleep(1);
			int nextRouterPort = routerAndPorts[i][0];

			packet to_send;
			bzero(to_send.data, DATA_SIZE);
			sprintf(to_send.data, "-1");

			send_udp_msg(udpSocket, nextRouterPort, &to_send);
			//printf("sending end file to %d: %s\n", i, to_send.data);
			managerFileStream<<"Time: "<<currentDateTime()<<" sending end file to "<<i<<": "<<to_send.data<<"\n";
		}
	}
	else{
		error("fileptr is not open in sendNeighborInformation, exiting forecefully");
	}	 
}

void closeAllSockets(){
	for(int i = 0; i < totalRouterNum; i++){
		int tempRouterSock = routerAndPorts[i][1];
		close(tempRouterSock);
	}
	
}

void usage(){
  cout<<"Usage: ./manager <filename> :  please add a file describing topology."<<endl;
  cout<<"manager program can only handle 1-10 routers."<<endl;
  printf("bad file, exiting manager.\n");
	exit(0);
}
 
int main(int argc, char* argv[]){
	//open ofstream to use for debugging and final stuff
	ofstream managerFileStream;
	managerFileStream.open("manager.out");

	//open file and start reading it
	//leave the file open!!!
	if(argc < 2){
		cout<<"less then 1"<<endl;
		usage();//error check
	}
	string fileName = "";
	fileName = argv[1];
	ifstream fileptr(fileName);

	//read first number in file
	if (fileptr.is_open()){
		string line = "";
		getline(fileptr, line);
		//cout<<"line "<<line<<endl;
		totalRouterNum = atoi(line.c_str()); 
		//cout<<"totalRouterNum "<<totalRouterNum<<endl;
		if (totalRouterNum < 1 || totalRouterNum > 10){ //error check
			cout<<"num to big or too small"<<endl;
			usage();
		}
		
		//initialize all tables to be full of -1
		makeAllTables();

		//setup TCP server
		int startingPort = 3360;
		managerTcpSock = server_bind_listen(startingPort);
		managerFileStream<<"Time: "<<currentDateTime()<<" Listening on port: "<<managerTcpPort<<"\n\n";

		//make all of the routers
		managerFileStream<<"Time: "<<currentDateTime()<<" Start creating routers\n";
		createRouters(managerFileStream);
		managerFileStream<<"Time: "<<currentDateTime()<<" Finish creating routers\n\n";
		
		//send neighbor information
		managerFileStream<<"Time: "<<currentDateTime()<<" Start sending neighbor information\n";
		sendNeighborInformation(fileptr, managerFileStream);
		managerFileStream<<"Time: "<<currentDateTime()<<" Finish sending neighbor information\n\n";
		//printf("manager sent neighbor information\n");
		
		//Manager waits for messages from all routers saying they are done with link state algorithm
		managerFileStream<<"Time: "<<currentDateTime()<<" Waiting for routers to finish link state algorithm...\n";
		for(int i = 0; i < totalRouterNum; i++){
			int socket = routerAndPorts[i][1];
			packet temp;
			bzero(temp.data, DATA_SIZE);
			recv_msg(socket, &temp);
			managerFileStream<<"Time: "<<currentDateTime()<<"Manager received "<<temp.data<<" from "<<i<<"\n";
		}
		managerFileStream<<"Time: "<<currentDateTime()<<" Routers finished link state algorithm\n\n";

		//printf("manager finished waiting information\n");

		//Manager sends messages to all routers according to file
		//cout<<"calling send Messages"<<endl;
		managerFileStream<<"Time: "<<currentDateTime()<<" Start sending messages to routers\n";
		sendMessages(fileptr, managerFileStream);
		managerFileStream<<"Time: "<<currentDateTime()<<" Finish sending messages to routers\n\n";
		managerFileStream.close();
	} //skips this if file doesn't open.
	else{
		error("bad file name");
	}

	close(managerTcpSock);
	closeAllSockets();
	printf("exiting manager\n");
	exit(0);
}
