# CS457_P3
Group Members: Chad Eitel, Gabriella Fontani, Benjamin Scott
CS457 Project3
Professor Indrajit Ray
Due 12/8/2016


to run the program call $./manager <filename>

The file for this program needs to be set up exactly like the example file in the instructions.  Meaning that the first line in the file is the number of routers in the network and the following lines hold the topology of the network.  A "-1" signifies the end of the topology description.  After the "-1" the file indicates what messages it wants sent around the network by listing the "from" "to" routers.  The end of message passing part of the file is indicated with a "-1". There cannot be any additional white space in the file, otherwise the program will fail.  Note that consecutive numbers on a line are separated by spaces, not tabs. 

Example of the file format:
3
0 1 20
0 2 40
1 2 10
2 0 10
-1
0 2
2 0
1 2
-1

Once the program starts running each router writes to a file labeled routerN.out, where N is the identifier of each individual router. Each router writes to its file each message it receives from the manager over the TCP connection.  It also writes all of the neighbor LSP messages it receives over the UDP connection it has with its neighbors.  After receiving all of the LSPs from all of the routers (through the router's neighbors), each router performs Djikstra's algorithm to find the shortest path between nodes, creating a forwarding table. Each router then writes its forwarding table to the file.  The manager then starts the message sending process by sending packets to the "from" router that indicate the "to" router.  Each router, after receiving a message, figures out if the it is the destination router.  If it isn't the destination router, the router looks up in its forwarding table what the next hop is to get the messages to its final destination.  At the same time each router logs each message it sends and receives in its out file.  Note that any lsp messages that are still floating around are ignored and not written to the file. Once the manager has sent out all of the message packets it sends a "-1" to each router to let them know that the manager is done sending messages. After the "-1" is received, all of the routers and the manager exit. When each process finished, there is output printed to the terminal indicating which router/manager has exited.

The program takes a few seconds to complete so there is some output printed to the terminal to let you know that the program is in fact working. The user knows that the program is complete once an exit message is printed for all routers and the manager.

Many messages are passed between the routers and manager in the course of the program. Most of the message formats are as follows:
 - Message sent to manager when router created: "hello from router #<router_id>, my udp port is <router_udp_port>"
 - Message from manager to tell routers of their neighbors: "<neighbor_id>,<neighbor_udp_port>,<neighbor_weight>"
 - Messages exchanged between routers to exchange LSPs: "lsp,<from_router_id>,<origin_router_id>,<neighbor1_id>,<weight_to_neighbor1>,...,<neighborN_id>,<weight_to_neighborN>"
 - Message from routers to manager to indicate completeion of link state algorithm: "<router_id> done!"
 - Messages forwarded between routers: "<from_router_id>,<to_router_id>,<message>"
 
Have a great Christmas Break.

