#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <map>
#include <cmath>

#include "Config.h"

/////////////////////////////////////////////////
using namespace std;

class TrustOpinion; class Message;
class Car; class Block; class Map; 
class CentralAuthority; class Logger;

enum BlockType {NorthSouthBlock, WestEastBlock, CrossingBlock};

enum Direction {North, East, South, West};

void init_random(unsigned int seed);
Direction GetDirection(int value);

class TrustOpinion
{
public:
	Car* evaluator;
	bool trustAction;
	double trustConfidence;
	
	TrustOpinion(Car* evaluator, bool trust_action, double trust_confidence);
};

class Collector
{
public:
	map<int, vector<double> > data;
	vector<double> Average();

};

class Message
{
public:
	Message(Car* sender, double sender_confidence, bool actual_quality, 
		int time_stamp, Block* in_block_of_message, Map* geomap);
	Message(Message* message);
	//message can propage until dropped at some block (the return value is distance) 
	int Propagate();	
	int Propagate(Direction direction_of_propagation);
	//let message be evaluate by the block
	void BroadcastToBlock(Block* block);
	//get the majority relay decision from the message
	bool GetMajorityRelayDecision(Block* block_of_the_message);
	

	//sender
	Car* sender;
	double senderConfidence;
	//evaluators
	vector<TrustOpinion> vecTrustOpinion;
	//message quality: good / bad, 0 / 1
	bool actualQuality;
	//message id
	int messageID;
	//the time
	int timeStamp;
	//the block where the message is in
	Block* blockOfMessageOrigin;
	//global geomap knowledge
	Map* geomap;
	//longest propagation distance
};

class Block
{
public:
	BlockType blockType;
	int blockID, x, y;
	set<Car*> carSet;
	
	Block(int block_id, int x_coord, int y_coord, BlockType block_type);
};

class Logger
{
public:
	//used for the effect of exp-trust
	map<int,int> numVictim;
	//used for count the social popularity of a peer
	map<int, map<int, int> > interaction;
	//used for detection rate of bad messages
	map<int, int> numDetectedMessage;
	//used for the number of messages received per peer
	map<int, int> numReceivedMessage;
	//used for the number of messages received per peer
	///////////////////
	vector<int> intvalues;
	///////////////////
	//used for the number of messages per block
	map<int, int> numReceivedMessagePerBlock;
	map<int, int> numReceivedBadMessagePerBlock;
	///////////////////
	fstream fout;
	Logger();
};


class Map
{
public:
	int sizeX, sizeY;
	int BlockCount;
	vector<Block*> vecBlock;
	vector<Car*> vecCar;
	vector<Message*> vecMessageCache;
	CentralAuthority* centralAuthority;

	//for experiment log purpose
	Logger logger;

	Block* GetNextBlock(Block* currentBlock, Direction currentDir, Direction* newDir);
	Block* GetBlockByXY(int x_coord, int y_cood);
	Block* GetBlockByBlockID(int block_id);

	Map(string strFile);
	~Map();
	void PrintMap();
	//global time(real time)
	int global_time;
	int TimePass() { return global_time++; }
	//utility functions
	double BlockDistance(Block* sourceBlock, Block* destBlock);

private:
	map<int, map<int, Block*> > bMap;

};

class Car
{
public:
	int identity, speed, inBlockOffset;
	//each car has an initial role-based trust [0,1]
	double role_trust;
	//indicate the honesty and lazyness
	bool honesty;
	int detection_rate;
	//each car maintains a list of trust for other peers.
	//a table that maps the id to the value of exp-trust.
	map<int, double> exp_trust;
	//int exp_trust_version;
	//double Exp_Trust(int car_identity);
	//each car maintains the time for each other of last interaction
	map<int, int> last_interaction_time;

	Direction driving_direction;
	Block* currentBlock;
	Map* geomap;

	Car(int identity, Block* block, Map* geomap, double role_based_trust, 
		bool honesty, int detection_rate);
	//move forward, please keep time_sec as small as 1, e.g.
	Block* Move(int time_sec);
	//change current location to a new location (block)
	bool ChangeLocation(Block* current_block_location, Block* new_block_location);
	//tell if a trust opinion can be generated
	bool IfTrustOpinionAvailable(Message* message);
	//generate a feedback
	TrustOpinion GenerateTrustOpinion(Message* message);
	//action module, derive a local action {trust, distrust}
	bool ActionModule(Message* message);
	//send a customized message
	Message* SendMessage(bool message_quality, double confidence);
	//update experience-based trust
	void UpdateExperienceBasedTrust(Message* message);
private:
	//increase exp-based trust
	double IncreaseExperienceBasedTrust(double currentTrust, 
		double confidence, int time_difference_from_last_interaction);
	//decrease exp-based trust
	double DecreaseExperienceBasedTrust(double currentTrust, 
		double confidence, int time_difference_from_last_interaction);
};

class CentralAuthority
{
public:
	CentralAuthority(Map* geomap, vector<Block*> block_list,
		vector<Car*> car_list);
	//compute the global trust from exp-based trust of each peer
	void ComputeGlobalTrust();
	//force the update for experience-based trust for all peers
	void UpdateExpTrustForAllPeers();
	//print the average number of exp-based trust that one peer has locally
	void PrintAverageNumberExpBasedTrust();
	//print the experience-trust for a peer
	void PrintExpBasedTrustForPeer(int peerID);

	Map* geomap;
	vector<Block*> blockList;
	vector<Car*> carList;

	map<int, double> globalExpBasedTrust;
};


