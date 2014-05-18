#include "Config.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

////////////////////////////////////////////////
//parameters in paper
double RELAY_THESHOLD;
double ALPHA;
double BETA;
int MAX_TIME_DIFFERENCE;
double LAMBDA;
//local actions
double PHI;
double TAU;
double GAMMA;

///////////////////////////////////////////////////
//parameters in experiment
int BLOCKWIDTH;
double TRUST_OPINION_MAX_AVAIL_DISTANCE_BLOCK;
int MESSAGE_MAX_PROPAGATION_DISTANCE_BLOCK;
int CAR_SPEED_MIN;
int CAR_SPEED_DELTA;
int CAR_NUMBER_PER_BLOCK;

int CAR_L_TRUN_PROB_PRECENT;
int CAR_R_TRUN_PROB_PRECENT;

int SIMULATIONTIME;
int SAMPLE_FREQ;

bool ENABLE_MAL_PEERS_MOVE;
bool FIRST_SENDER_AT_CORNER;
bool ENABLE_MULTIPLE_MAL_PEERS;
int PERCENT_OF_MAL_PEERS;
int DETECTION_RATE;
int DETECTION_RATE_DELTA;

double PERCENT_AUTHORITY;
bool EXP_TRUST_ON;
bool ROLE_TRUST_ON;
bool RELAY_CONTROL_ON;
bool FULL_DETECT_ON;
bool TURN_TRUST_OPINION_ON;

int MESSAGE_FREQENCY_SECONDS;

int RUN_TIMES;

void PrintConfiguration()
{
	cout << "RELAY_THESHOLD = " << RELAY_THESHOLD << endl;

	cout << "increment ALPHA = " << ALPHA << endl;
	cout << "decrement BETA = " << BETA << endl;
	cout << "MAX_TIME_DIFFERENCE = " << MAX_TIME_DIFFERENCE << endl;
	cout << "aging factor LAMBDA = " << LAMBDA << endl;

	cout << "------- trust values for local actions: " << endl;
	cout << "trust action PHI = " << PHI << endl;
	cout << "trust threshold TAU = " << TAU << endl;
	cout << "sender weight GAMMA = " << GAMMA << endl;

	cout << "------- parameters in experiment" << endl;
	cout << " BLOCKWIDTH = " << BLOCKWIDTH<< endl;
	cout << " TRUST_OPINION_MAX_AVAIL_DISTANCE_BLOCK = " << TRUST_OPINION_MAX_AVAIL_DISTANCE_BLOCK<< endl;
	cout << " MESSAGE_MAX_PROPAGATION_DISTANCE_BLOCK = " << MESSAGE_MAX_PROPAGATION_DISTANCE_BLOCK<< endl;
	cout << " CAR_SPEED_MIN = " << CAR_SPEED_MIN<< endl;
	cout << " CAR_SPEED_DELTA = " << CAR_SPEED_DELTA<< endl;
	cout << " CAR_NUMBER_PER_BLOCK = " << CAR_NUMBER_PER_BLOCK<< endl;

	cout << " CAR_L_TRUN_PROB_PRECENT = " << CAR_L_TRUN_PROB_PRECENT<< endl;
	cout << " CAR_R_TRUN_PROB_PRECENT = " << CAR_R_TRUN_PROB_PRECENT<< endl;

	cout << " SIMULATIONTIME = " << SIMULATIONTIME<< endl;
	cout << " SAMPLE_FREQ = " << SAMPLE_FREQ << endl;
	
	cout << " ENABLE_MAL_PEERS_MOVE = " << (ENABLE_MAL_PEERS_MOVE? "true" : "false") << endl;
	cout << " FIRST_SENDER_AT_CORNER = " << (FIRST_SENDER_AT_CORNER? "true" : "false") << endl;
	cout << " ENABLE_MULTIPLE_MAL_PEERS = " << (ENABLE_MULTIPLE_MAL_PEERS? "true" : "false") << endl;
	cout << " PERCENT_OF_MAL_PEERS = " << PERCENT_OF_MAL_PEERS<< endl;
	cout << " DETECTION_RATE = " << DETECTION_RATE << endl;
	cout << " DETECTION_RATE_DELTA = " << DETECTION_RATE_DELTA << endl;

	cout << "PERCENT_AUTHORITY = " << PERCENT_AUTHORITY << endl;
	cout << "EXP_TRUST_ON = " << (EXP_TRUST_ON ? "true":"false") << endl;
	cout << "ROLE_TRUST_ON = " << (ROLE_TRUST_ON ? "true":"false") << endl;
	cout << "RELAY_CONTROL_ON = " << (RELAY_CONTROL_ON ? "true":"false") << endl;
	cout << "FULL_DETECT_ON = " << (FULL_DETECT_ON ? "true":"false") << endl;
	cout << "TURN_TRUST_OPINION_ON = " << (TURN_TRUST_OPINION_ON ? "true":"false") << endl;

	cout << " MESSAGE_FREQENCY_SECONDS = " << MESSAGE_FREQENCY_SECONDS << endl;
	
	cout << " RUN_TIMES = " << RUN_TIMES << endl;
}

void LoadDefault()
{
	//parameters in paper
	RELAY_THESHOLD = 0.800;
	ALPHA = 0.01;
	BETA = 0.10;
	MAX_TIME_DIFFERENCE = 100;
	LAMBDA = 0.95;
	//local actions
	PHI = 0.2;
	TAU = 0.1; // [0,1]
	GAMMA = 2.0;

	///////////////////////////////////////////////////
	//parameters in experiment
	BLOCKWIDTH = 500;
	TRUST_OPINION_MAX_AVAIL_DISTANCE_BLOCK = 2;
	MESSAGE_MAX_PROPAGATION_DISTANCE_BLOCK = 10;
	CAR_SPEED_MIN = 20;
	CAR_SPEED_DELTA = 20;
	CAR_NUMBER_PER_BLOCK = 5;

	CAR_L_TRUN_PROB_PRECENT = 20;
	CAR_R_TRUN_PROB_PRECENT = 20;

	MESSAGE_FREQENCY_SECONDS = 5;
	SAMPLE_FREQ = 30;

	ENABLE_MAL_PEERS_MOVE = false;
	FIRST_SENDER_AT_CORNER = false;
	ENABLE_MULTIPLE_MAL_PEERS = true;
	PERCENT_OF_MAL_PEERS = 1;
	DETECTION_RATE = 70;
	DETECTION_RATE_DELTA = 30;

	PERCENT_AUTHORITY = 2.0;
	EXP_TRUST_ON = true;
	ROLE_TRUST_ON = true;
	RELAY_CONTROL_ON = true;
	FULL_DETECT_ON = false;
	TURN_TRUST_OPINION_ON = true;

	SIMULATIONTIME = 36000;

	RUN_TIMES = 1;
}

void ReadConfigurationFromFile()
{
	LoadDefault();

	ifstream file("config.txt", istream::in);
	if(file.fail())
	{
		cout << "missing config.txt, using default values" << endl;
		return;
	}
	cout << "---- begin of reading configuration ----" << endl;
	while(file.good())
	{
		string parameter;
		file >> parameter;
		cout << "reading values for parameter: " << parameter << endl;
		if(parameter == "RELAY_THESHOLD")
			file >> RELAY_THESHOLD;
		else if(parameter == "ALPHA")
			file >> ALPHA; 
		else if(parameter == "BETA")
			file >> BETA; 
		else if(parameter == "MAX_TIME_DIFFERENCE")
			file >> MAX_TIME_DIFFERENCE; 
		else if(parameter == "LAMBDA")
			file >> LAMBDA;
		else if(parameter == "PHI")
			file >> PHI; 
		else if(parameter == "TAU")
			file >> TAU; 
		else if(parameter == "GAMMA")
			file >> GAMMA; 
		else if(parameter == "BLOCKWIDTH")
			file >> BLOCKWIDTH; 
		else if(parameter == "TRUST_OPINION_MAX_AVAIL_DISTANCE_BLOCK")
			file >> TRUST_OPINION_MAX_AVAIL_DISTANCE_BLOCK; 
		else if(parameter == "MESSAGE_MAX_PROPAGATION_DISTANCE_BLOCK")
			file >> MESSAGE_MAX_PROPAGATION_DISTANCE_BLOCK;
		else if(parameter == "CAR_SPEED_MIN")
			file >> CAR_SPEED_MIN; 
		else if(parameter == "CAR_SPEED_DELTA")
			file >> CAR_SPEED_DELTA; 
		else if(parameter == "CAR_NUMBER_PER_BLOCK")
			file >> CAR_NUMBER_PER_BLOCK; 
		else if(parameter == "SIMULATIONTIME")
			file >> SIMULATIONTIME; 
		else if(parameter == "CAR_L_TRUN_PROB_PRECENT")
			file >> CAR_L_TRUN_PROB_PRECENT; 
		else if(parameter == "CAR_R_TRUN_PROB_PRECENT")
			file >> CAR_R_TRUN_PROB_PRECENT; 
		else if(parameter == "SAMPLE_FREQ")
			file >> SAMPLE_FREQ; 
		else if(parameter == "ENABLE_MAL_PEERS_MOVE")
			file >> ENABLE_MAL_PEERS_MOVE; 
		else if(parameter == "FIRST_SENDER_AT_CORNER")
			file >> FIRST_SENDER_AT_CORNER;
		else if(parameter == "ENABLE_MULTIPLE_MAL_PEERS")
			file >> ENABLE_MULTIPLE_MAL_PEERS; 
		else if(parameter == "PERCENT_OF_MAL_PEERS")
			file >> PERCENT_OF_MAL_PEERS; 
		else if(parameter == "DETECTION_RATE")
			file >> DETECTION_RATE;
		else if(parameter == "DETECTION_RATE_DELTA")
			file >> DETECTION_RATE_DELTA;
		else if(parameter == "PERCENT_AUTHORITY")
			file >> PERCENT_AUTHORITY ;
		else if(parameter == "EXP_TRUST_ON")
			file >> EXP_TRUST_ON;
		else if(parameter == "ROLE_TRUST_ON")
			file >> ROLE_TRUST_ON;
		else if(parameter == "RELAY_CONTROL_ON")
			file >> RELAY_CONTROL_ON;
		else if(parameter == "FULL_DETECT_ON")
			file >> FULL_DETECT_ON;
		else if(parameter == "TURN_TRUST_OPINION_ON")
			file >> TURN_TRUST_OPINION_ON;
		else if(parameter == "MESSAGE_FREQENCY_SECONDS")
			file >> MESSAGE_FREQENCY_SECONDS; 
		else if(parameter == "RUN_TIMES")
			file >> RUN_TIMES; 
		else
		{
			cout << "unknown parameter: " << parameter << endl;
		}
	}
	cout << "---- end of reading configuration ----" << endl;
}
