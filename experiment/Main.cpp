#include "Elements.h"
extern int logger_file_index_number;

void Run(size_t run_id, Collector* collector)
{
	init_random(run_id + 100);

	Map* geomap = new Map(MAP_FILE);
	//init cars
	int carNum = CAR_NUMBER_PER_BLOCK * geomap->BlockCount;

	vector<Car*> malcars;
	vector<Car*> goodcars;
	int malicious_peer_count = 0;
	int number_malicious_peers = 1;
	if (ENABLE_MULTIPLE_MAL_PEERS == true)
	{
		number_malicious_peers = (int)((double)carNum / 100.0 * (double)PERCENT_OF_MAL_PEERS);
	}

	for(int id = 0; id < carNum; id++)
	{
		//put the car to a randomly chosen block
		int blockid = rand() % geomap->BlockCount;
		//specialization
		if(id == 0 && FIRST_SENDER_AT_CORNER) blockid = 0;
		//end of specialization
		bool honesty = true;
		if(malicious_peer_count < number_malicious_peers)
		{//malicious
			honesty = false;
			malicious_peer_count ++;
		}
		//if a car does not have a role, assign -1 as its role-trust
		double role_trust = -1;
		int detection_rate = DETECTION_RATE;
		if(DETECTION_RATE_DELTA > 0)
		{
			detection_rate = DETECTION_RATE - 
				DETECTION_RATE_DELTA + rand() % (DETECTION_RATE_DELTA * 2);
			if(detection_rate < 0)	detection_rate = 0;
			if(detection_rate > 100)	detection_rate = 100;
		}
		//add the car to the map
		Car* car = new Car(id, geomap->vecBlock[blockid], geomap, role_trust, honesty, detection_rate);
		geomap->vecCar.push_back(car);
		if(honesty == false)
		{
			malcars.push_back(car);
		}
		else
		{
			goodcars.push_back(car);
		}
	}

	geomap->centralAuthority = new CentralAuthority(geomap,	geomap->vecBlock, geomap->vecCar);

	//specialize some cars here
	size_t num_mal_peers = malcars.size();
	for(size_t i = 0; i < num_mal_peers; i++)
	{
		if(ENABLE_MAL_PEERS_MOVE == false)
		{	
			malcars[i]->speed = 0;
		}
	}
	//add police cars
	int num_police_cars = (int)((double)carNum / 100.0 * PERCENT_AUTHORITY);
	int count_police = 0;
	for(size_t i = 0; i < geomap->vecCar.size(); i++)
	{
		if(count_police >= num_police_cars)
		{
			break;
		}
		if(geomap->vecCar[i]->honesty == true)
		{//assing the police role to the car
			geomap->vecCar[i]->role_trust = 1.0;
			count_police ++;
		}
	}
	//end of specialization

#ifdef ENABLE_DETAIL
	cout << "Car number: " << carNum << endl;
	cout << "Maclious car number: " << num_mal_peers << endl;
	cout << "Map size: " << geomap->sizeX << " x " << geomap->sizeY << endl;
	cout << "Block count: " << geomap->vecBlock.size() << endl;
	PrintConfiguration();
#endif
	size_t sub_run_times = 1;
//	size_t sub_run_times = 11;
	for(size_t sub_run_t = 0; sub_run_t < sub_run_times; sub_run_t++)
	{
		for(int t = 1; t <= SIMULATIONTIME; t++)
		{
			if(t % 300 == 0) cout << "t = " << t << ", h = " << t / 3600.0 << endl << flush;
			//IMPORTANT////////////
			geomap->TimePass();
			///////////////////////
			//first, each car moves on;
			for(int c = 0; c < carNum; c++)
			{
				geomap->vecCar[c]->Move(1);
			}

			//second, disseminate messages
			vector<Message*> messageCache;
			//size_t num_bad_message = sub_run_t * 1; //run_id = 0, 1, ... 10
			//size_t num_good_message = 10 - num_bad_message;
			//good messages are sent
			if(t % MESSAGE_FREQENCY_SECONDS == 0)
			{
				//for(size_t i = 0; i < geomap->vecCar.size(); i++)
				//{
				//	Car* car = geomap->vecCar[i];
				//	if (car->honesty == true && ((rand() % 100) < 1))
				//	{
				//		bool messageQuality = true;
				//		Message* message = car->SendMessage(messageQuality, 1.0);
				//		messageCache.push_back(message);
				//	}
				//}
				//for(size_t i = 0; i < num_good_message; i++)
				//{
				//	Car* gCar = goodcars[ rand() % goodcars.size() ];
				//	messageCache.push_back(gCar->SendMessage(true, 1.0));
				//}
			}
			//bad messages are sent every MESSAGE_FREQENCY_SECONDS seconds
			if(t % MESSAGE_FREQENCY_SECONDS == 0)
			{
				for(size_t i = 0; i < malcars.size(); i++)
				{
					Car* mCar = malcars[i];
					bool messageQuality = false;
					//bool messageQuality = true;
					//bool messageQuality = ((rand() % 2) == 1);
					Message* message = mCar->SendMessage(messageQuality, 1.0);
					messageCache.push_back(message);
				}
				//for(size_t i = 0; i < num_bad_message; i++)
				//{
				//	//do not attack until some time later
				//	//if(t < 400) break;
				//	Car* mCar = malcars[ rand() % malcars.size() ];
				//	messageCache.push_back(mCar->SendMessage(false, 1.0));
				//}
			}

			//third, handle messages, map trust to peers
			for(size_t i = 0; i < messageCache.size(); i ++)
			{
				int propDis = messageCache[i]->Propagate();
				geomap->logger.intvalues.push_back(propDis);
			}	

			//fouth, free messages and clear cache
			for(size_t i = 0; i < messageCache.size(); i ++)
			{
				delete messageCache[i];
			}
			messageCache.clear();

			if(t % SAMPLE_FREQ == 0)
			{
				//used to test the effect of exp-trust, in terms of number of victims
				//size_t numV = geomap->logger.numVictim.size();
				//double percent = numV * 100 / (double)carNum;
				//cout << "number victim = " << numV << ", " << percent << "%" << endl;
				//geomap->logger.fout << percent << endl;
				//collector->data[run_id].push_back(percent);
				//geomap->logger.numVictim.clear();

				//average number of wrong decisions
				int number = 0;
				for(map<int, int>::const_iterator iter = geomap->logger.numVictim.begin();
					iter != geomap->logger.numVictim.end(); iter++)
				{
					number += iter->second;
				}
				double avg_number = (double)number / (double) geomap->vecCar.size();
				cout << "average number of wrong decisions = " << avg_number << endl;
				geomap->logger.fout << avg_number << endl;
				collector->data[run_id].push_back(avg_number);
				geomap->logger.numVictim.clear();

				//the effect of system effectiveness, wrong decision rate
				//size_t total_decisions = 0;
				//size_t wrong_decisions = 0;
				//for(map<int, int>::const_iterator iter = geomap->logger.numReceivedMessage.begin();
				//	iter != geomap->logger.numReceivedMessage.end(); iter++)
				//{
				//	total_decisions += iter->second;
				//}
				//for(map<int, int>::const_iterator iter = geomap->logger.numVictim.begin();
				//	iter != geomap->logger.numVictim.end(); iter++)
				//{
				//	wrong_decisions += iter->second;
				//}
				//double rate = 100.0 * (double) wrong_decisions / (double) total_decisions;
				//cout << "wrong decision rate = " << rate << endl;
				//geomap->logger.fout << rate << endl;
				//collector->data[run_id].push_back(rate);

				//an alternative of the above
				//double total_rate = 0;
				//double count_rate = 0;
				//for(size_t i = 0; i < geomap->vecCar.size(); i++)
				//{
				//	int id = geomap->vecCar[i]->identity;
				//	int total = geomap->logger.numReceivedMessage[id];
				//	if(total > 0)
				//	{
				//		total_rate += (double)((double)geomap->logger.numVictim[id] / (double) total);
				//		count_rate ++;
				//	}
				//}
				//double rate = total_rate / count_rate * 100.0;
				//cout << "wrong decision rate = " << rate << endl;
				//geomap->logger.fout << rate << endl;
				//collector->data[run_id].push_back(rate);
				
				///////////////////////////////////////
				//used to test the effect of social popularity
				//Car* testedCar = malcars[0];
				//size_t numV = geomap->logger.interaction[testedCar->identity].size();
				//double percentV = numV * 100 / (double)carNum;
				//cout << "Known cars = " << numV << ", " 
				//	<< percentV << "%" << endl;
				//geomap->logger.fout << percentV << endl;
				//collector->data[run_id].push_back(percentV);
				
				//used to count the number of deliveries
				//Car* testedCar = malcars[0];
				//int numV = 0;
				//for(map<int, int>::const_iterator iter = 
				//	geomap->logger.interaction[testedCar->identity].begin();
				//	iter != geomap->logger.interaction[testedCar->identity].end();
				//	iter ++)
				//{
				//	numV += iter->second;
				//}
				//cout << "delivery times= " << numV << endl;
				//geomap->logger.fout << numV << endl;
				//collector->data[run_id].push_back(numV);
				//geomap->logger.interaction.clear();

				////////////////////////////////////////
				//sampling the detection rate of peer (id = 5000)
				//int numD = geomap->logger.numDetectedMessage[5000];
				//int numT = geomap->logger.numReceivedMessage[5000];
				//double detectionRate = (double) numD / (double) numT * 100;
				//cout << "detection: " << numD << "/" << numT << "/" << detectionRate << endl;
				///*if( t > 1000)
				//{	
				//	geomap->logger.numDetectedMessage[5000] = (int)
				//		(0.95 * geomap->logger.numDetectedMessage[5000]);
				//	geomap->logger.numReceivedMessage[5000] = (int)
				//		(0.95 * geomap->logger.numReceivedMessage[5000]);
				//}*/
				//geomap->logger.fout << detectionRate << endl;
				//collector->data[run_id].push_back(detectionRate);

				///////////////////////////////////////////////
				//show how many messages received per peer
				//size_t mcount = 0;
				//for(size_t i = 0; i < geomap->vecCar.size(); i++)
				//{
				//	int id = geomap->vecCar[i]->identity;
				//	mcount += geomap->logger.numReceivedMessage[id];
				//}
				//double mavg = (double) mcount / (double) geomap->vecCar.size();
				//cout << "Average # / peer = " << mavg << endl;
				//geomap->logger.fout << mavg << endl;
				//collector->data[run_id].push_back(mavg);

				//////////////////////////////////////////////////
				//show number of hops of a message
				//int erasetimes = geomap->logger.intvalues.size() / 10;
				//for(int i = 0; i < erasetimes; i++)
				//	geomap->logger.intvalues.erase(geomap->logger.intvalues.begin());
				//int totalhops = 0;
				//for(size_t i = 0; i < geomap->logger.intvalues.size(); i++)
				//{
				//	totalhops += geomap->logger.intvalues[i];
				//}
				//double averagehops = 
				//	(double)totalhops / (double) geomap->logger.intvalues.size();
				//cout << "Average hops = " << averagehops << endl;
				//geomap->logger.fout << averagehops << endl;
				//collector->data[run_id].push_back(averagehops);

				////////////////////////////////////////////////////
				//show number of messages per block
				//double uti_rate = 0, uti_rate_total = 0, total_count = 0;
				//int num_msg_total = 0, num_good_msg_total = 0;
				//for(size_t i = 0; i < geomap->vecBlock.size(); i++)
				//{
				//	int id = geomap->vecBlock[i]->blockID;
				//	int num_msg = geomap->logger.numReceivedMessagePerBlock[id];
				//	num_msg_total += num_msg;

				//	int num_msg_good = num_msg - geomap->logger.numReceivedBadMessagePerBlock[id];
				//	num_good_msg_total += num_msg_good;

				//	if(num_msg > 0)
				//	{
				//		uti_rate_total += ((double)num_msg_good / (double)num_msg);
				//		total_count += 1.0;
	
				//		/*cout << "#msg/gmsg/rate = " << num_msg << "/" << num_msg_good
				//		<< "/" << (double)num_msg_good / (double)num_msg << endl;*/
				//	}
				//	//to cut the past record
				//	if(num_msg > 15)
				//	{
				//		geomap->logger.numReceivedMessagePerBlock[id] *= 0.95;
				//		geomap->logger.numReceivedBadMessagePerBlock[id] *= 0.95;
				//	}
				//}
				//uti_rate = uti_rate_total / total_count;
				//cout << "gmessage / msg / rate = " << num_good_msg_total 
				//	<< "/" << num_msg_total 
				//	<< "/" << (double)num_good_msg_total/num_msg_total << endl;
				//cout << "utilization rate = " << uti_rate << endl;
				//geomap->logger.fout << uti_rate << endl;
				//collector->data[run_id].push_back(uti_rate);
			}
			//clear the counter and go to the next second

		}//end of for int t

		/////////////////////////////////
		//show how many messages per peer after one sub run
		//size_t mcount = 0;
		//for(size_t i = 0; i < geomap->vecCar.size(); i++)
		//{
		//	int id = geomap->vecCar[i]->identity;
		//	mcount += geomap->logger.numReceivedMessage[id];
		//}
		//double mavg = (double) mcount / (double) geomap->vecCar.size();
		//cout << "Average # / peer = " << mavg << endl;
		//geomap->logger.fout << mavg << endl;
		//collector->data[run_id].push_back(mavg);
		//geomap->logger.numDetectedMessage.clear();
		//geomap->logger.numReceivedMessage.clear();
	}//end of for sub_run_times

	//free Cars, Blocks, CA and Map
	delete geomap;
}

void MergeLog(int start, int end)
{
	int numFiles = end - start + 1;
	ifstream* files = new ifstream[numFiles];
	for(int i = 0; i < numFiles; i++)
	{
		string filename;
		stringstream ss;
		ss << "log" << (i + start) << ".txt";
		ss >> filename;
		files[i].open(filename.c_str());
		if(files[i].fail())
		{
			cout << "cannot open " << filename << endl;
			exit(1);
		}
	}
	while(files[0].good())
	{
		double value = 0;
		for(int i = 0; i < numFiles; i++)
		{
			double readvalue;
			files[i] >> readvalue;
			value += readvalue;
		}
		value /= numFiles;
		cout << value << endl;
	}
}

#include <string.h>
int main(int argc, char** argv)
{

	ReadConfigurationFromFile();
	PrintConfiguration();

	if(argc == 4 && strcmp(argv[1], "mergelog") == 0)
	{
		int start = atoi(argv[2]);
		int end = atoi(argv[3]);
		MergeLog(start, end);
		return 1;
	}

	int startRunID = 1;
	if(argc == 2)
	{
		startRunID = atoi(argv[1]);
		if(startRunID < 1 || startRunID > RUN_TIMES)
		{
			cout << "Invalid startRunID =" << startRunID << endl;
			exit(1);
		}
		cout << "You specified startRunID = " << startRunID << endl;
	}


/////important//////////////////////////////////////
	logger_file_index_number = startRunID - 1;
////////////////////////////////////////////////////

	map<int, Logger> logmap;
	Collector* collector = new Collector();
	for(int i = startRunID - 1; i < RUN_TIMES; i++)
	{
		cout << "------------------- Run = "
			<< i + 1 << " --------------------" << endl;
		Run(i, collector);
	}

	logger_file_index_number = 999998;

	Logger wholelog;
	for(size_t k = 0; k < (collector->data.begin()->second).size(); k++)
	{
		for(map<int, vector<double> >::const_iterator iter = collector->data.begin();
			iter != collector->data.end(); iter++)
		{
			wholelog.fout << (iter->second)[k] << "\t";
		}
		wholelog.fout << endl;
	}

	vector<double> result = collector->Average();

	Logger log;
	for(size_t i = 0 ; i < result.size(); i++)
	{
		log.fout << result[i] << endl;
	}
	delete collector;
	return 0;
}