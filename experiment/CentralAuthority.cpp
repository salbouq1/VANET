#include "Elements.h"
///////////////////////////////////////////////////////////
CentralAuthority::CentralAuthority(Map* geo_map, vector<Block*> block_list,
								   vector<Car*> car_list)
{
	this->geomap = geo_map;
	this->blockList = block_list;
	this->carList = car_list;
}

void CentralAuthority::ComputeGlobalTrust()
{//todo:
	double trust = 0;
	int count = 0;

	for(size_t carID = 1; carID < carList.size(); carID ++)
	{
		if(carList[carID]->exp_trust.count(0) > 0)
		{
			trust += carList[carID]->exp_trust[0];
			count ++;
		}
	}

	if(count > 0)
		this->globalExpBasedTrust[0] = trust / count;

	cout << "Peer 0 known by " << count << " peers. ";
}

void CentralAuthority::UpdateExpTrustForAllPeers()
{
	//todo:

	//for(size_t i = 0; i < carList.size(); i++)
	//{
	//	carList[i]->exp_trust = this->globalExpBasedTrust;
	//}
}

void CentralAuthority::PrintAverageNumberExpBasedTrust()
{
	int numberOfCars = carList.size();
	int count = 0, count2 = 0;

	for(vector<Car*>::const_iterator carIter = carList.begin();
		carIter != carList.end(); carIter ++)
	{
		Car* car = *carIter;
		count += car->exp_trust.size();

		if(car->exp_trust.size() > 0)
			count2 ++;
	}

	cout << "Trust Entri # / peer = " << (double)count / (double) numberOfCars 
		<< ", NONEMP/ALL = " << count2 << "/" << carList.size()  
		<< endl << flush;
}

void CentralAuthority::PrintExpBasedTrustForPeer(int peerID)
{
	cout << "Exp-based Trust of Peer " << peerID << " is " << 
		this->globalExpBasedTrust[peerID] << endl;
}
