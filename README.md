# CS457_P3
Here is our git repo for the 457 Project3
Group Members: Chad Eitel, Gabriella Fontani, Benjamin Scott

to run the program call $./manager <filename> <br />

The file for this program needs to be set up exactly like the example file in the instructions.  Meaning that the first line in the file is the number of routers in the network and the following lines hold the topology of the network.  A "-1" signifies the end of the topology description.  After the "-1" the file indicates what messages it wants sent around the network by listing the "from" "to" routers.  The end of message passing part of the file is indicated with a "-1". <br/> 

Example of the file format: <br/>
3<br/>
0 1 20<br/>
0 2 40<br/>
1 2 10<br/>
2 0 10<br/>
-1<br/>
0 2<br/>
2 0<br/>
1 2<br/>
-1<br/>

Once the program starts running each router writes to a file labeled routerN.out, where N is the identifier of each individual router.  Each router writes to its file each message it receives from the master over the TCP connection.  It also writes all of the neighbor LSP messages it receives over the UDP connection it has with its neighbors.  Each router then writes its forwarding table to the file.  The manager then starts the message sending process by sending packets to the "from" router that indicate the "to" router.  Each router, after receiving a message, figures out if the it is the destination router.  If it isn't the destination router, the router looks up in its forwarding table what the next hop is to get the messages to its final destination.  At the same time each router logs each message it sends and receives in its out file.  Once the manager has sent out all of the message packets it sends a "-1" to each router to let them know that the manager is done sending messages.  After the "-1" is received, all of the routers and the manager exit.

The program takes a few seconds to complete so there is some output printed to the terminal to let you know that the program is in fact working.  

Have a great Christmas Break.

