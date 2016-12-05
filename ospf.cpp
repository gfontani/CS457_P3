#include <project3.h>

//myNeighborsPorts	[n]
//allNeighborWeights	[n][n]
//routingTable	[n][3]

int moveLeastCost(vector<vector<int>> * tentative, vector<vector<int>> * confirmed){
	int index;
	int min_router;
	int min_cost;
	int min_next;

	while(true){
		if(tentative->size()==0){
			return -1; //no entries left, return -1 to indicate we are done
		}

		index = 0;
		min_router = (*tentative)[0][0];
		min_cost = (*tentative)[0][1];
		min_next = (*tentative)[0][2];

		for(int i=1; i<(int)tentative->size(); i++){
			if((*tentative)[i][1]<min_cost){
				index = i;
				min_router = (*tentative)[i][0];
				min_cost = (*tentative)[i][1];
				min_next = (*tentative)[i][2];		
			}
		}

		//check if entry is already on confirmed list
		for(int i=0; i<(int)confirmed->size(); i++){
			if((*confirmed)[i][0] == min_router){
				//see if new cost is less...
				if((*confirmed)[i][1]<=min_cost){
					//remove entry from tentative
					tentative->erase(tentative->begin()+index);
					continue;
				}
			}
		}
	break;//break while loop if we get here
	}

	//add entry to confirmed
	vector<int> temp;
	temp.push_back(min_router);
	temp.push_back(min_cost);
	temp.push_back(min_next);
	confirmed->push_back(temp);

	//remove entry from tentative
	tentative->erase(tentative->begin()+index);

	//return router index to inspect next
	return confirmed->size()-1;
}

bool not_in_confirmed(vector<int> * temp, vector<vector<int>> * confirmed){
	for(int i=0; i<(int)confirmed->size(); i++){
		if((*confirmed)[i][0] == (*temp)[0])
			return false;
	}
	return true;
}

bool update_tentative(vector<int> * temp, vector<vector<int>> * tentative){
	for(int i=0;i<(int)tentative->size(); i++){
		if((*tentative)[i][0] == (*temp)[0]){
			if((*tentative)[i][1] <= (*temp)[1]){
				return false;
			}
			else{
				tentative->erase(tentative->begin()+i);
			}
		}
	}
	tentative->push_back(*temp);
	return true;
}


int inspect(int index, vector<vector<int>> * confirmed, vector<vector<int>> * tentative, int myRouterNum){
	int router_to_inspect = (*confirmed)[index][0];
	int cost_to_add = (*confirmed)[index][1];
	int next_hop = (*confirmed)[index][2];

	for(int i=0; i<(int)allNeighborWeights[router_to_inspect].size(); i++){
		if(i!= myRouterNum && (int)allNeighborWeights[router_to_inspect][i]>=0){
				vector<int> temp;
				temp.push_back(i);
				temp.push_back(allNeighborWeights[router_to_inspect][i]+ cost_to_add);
				temp.push_back(next_hop);
				if(not_in_confirmed(&temp, confirmed)){
					update_tentative(&temp, tentative);
				}
		}
	}
	return -1;
}

int ospf(int myRouterNum){
	//create 2 lists -> tentative_list, confirmed_list
	vector<vector<int>> tentative_list;
	vector<vector<int>> confirmed_list;
	

	//place all own neighbors and costs on tentative list as 3-tuples (router, cost, next)
	for(int i=0; i<(int)allNeighborWeights[myRouterNum].size(); i++){
		if((int)allNeighborWeights[myRouterNum][i]>=0){
				vector<int> temp;
				temp.push_back(i);
				temp.push_back(allNeighborWeights[myRouterNum][i]);
				temp.push_back(i);
				tentative_list.push_back(temp);
		}
	}

	while(tentative_list.size()>0){
		int index = moveLeastCost(&tentative_list, &confirmed_list);
		inspect(index, &confirmed_list, &tentative_list, myRouterNum);
	}

	printf("router %d has reached end of ospf\n",myRouterNum);
	routingTable = confirmed_list;

	cout<<"Routing table: \ndest\tweight\tnextHop\n";
	for(unsigned int i = 0; i < routingTable.size(); i++){
		for(unsigned int j = 0; j < routingTable[i].size(); j++){
			cout<<routingTable[i][j]<<"\t";
		}
		cout<<"\n";
	}
	return 0;
}

