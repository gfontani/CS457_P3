//Gabriella Fontani, Ben Scott, Chad Eitel
#include <project3.h>
#include <router.cpp>

vector<vector<string>> routerAndPorts;
int managerTcpPort;
int totalRouterNum;

 //GABBY
 //method to create a table with number or routers and port numbers
 //will save a spave for ports but not fill it in (because we don't know the ports yet)
 void makeRouterTable(int numRouters){
	 
 }
 
 //CHAD
 //method to create all of the routers
 //fork to create all of the routers
 //in each child process call the router method from router
 void createRouters(){
	//Create all the routers by forking
	
	//loop
		//fork
		
		//if parent
			//wait for reply of each router after creation
	
			//receive tcp message from router containing router's udp port number
	
			//store router's udp port number in vector vector
						
		//if child
			//keep track of child id's?
			//call the router method from router passing router id

	
 }
 
 //GABBY
 //send the router neighbor information to each router, line by line
 void sendNeighborInformation(ifstream fileptr){
	 //Manager reads neighbors from the file, sending info line by line to each router
	 
	 //Send Neighbor information (neighbor id, link cost, UDP port number)
	 
 }
 
 //BEN
//send messages to all routers according to file
 void sendMessages(ifstream fileptr){
	 
	//loop
		//reads rest of file line by line
		//messages to router
		//sleeps between sending messages
	 
 }

 
int main(int argc, char* argv[]){
	//open file and start reading it
	//leave the file open!!!
	ifstream fileptr;
	
	//read first number in file
	int numRouters = 0;
	makeTable(numRouters);
	
	//setup TCP server
	managerTcpPort = 0;
	
	//make all of the routers
	createRouters();
	
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

	return 0;
}
