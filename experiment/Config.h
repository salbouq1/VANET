//////////////////////////////////////////////////
#define MAP_FILE "map_small.txt"

#define ENABLE_TRUST
#define ENABLE_DETAIL
////////////////////////////////////////////////
//parameters in paper
extern double RELAY_THESHOLD;
extern double ALPHA;
extern double BETA;
extern int MAX_TIME_DIFFERENCE;
extern double LAMBDA;
//local actions
extern double PHI;
extern double TAU;
extern double GAMMA;

///////////////////////////////////////////////////
//parameters in experiment
extern int BLOCKWIDTH;
extern double TRUST_OPINION_MAX_AVAIL_DISTANCE_BLOCK;
extern int MESSAGE_MAX_PROPAGATION_DISTANCE_BLOCK;
extern int CAR_SPEED_MIN;
extern int CAR_SPEED_DELTA;
extern int CAR_NUMBER_PER_BLOCK;

extern int CAR_L_TRUN_PROB_PRECENT;
extern int CAR_R_TRUN_PROB_PRECENT;

extern int SIMULATIONTIME;
extern int SAMPLE_FREQ;

extern bool ENABLE_MAL_PEERS_MOVE;
extern bool FIRST_SENDER_AT_CORNER;
extern bool ENABLE_MULTIPLE_MAL_PEERS;
extern int PERCENT_OF_MAL_PEERS;
extern int DETECTION_RATE;
extern int DETECTION_RATE_DELTA;

extern double PERCENT_AUTHORITY;
extern bool EXP_TRUST_ON;
extern bool ROLE_TRUST_ON;
extern bool RELAY_CONTROL_ON;
extern bool FULL_DETECT_ON;
extern bool TURN_TRUST_OPINION_ON;

extern int MESSAGE_FREQENCY_SECONDS;

extern int RUN_TIMES;

/////////////////////////////////////////////////
void PrintConfiguration();
void ReadConfigurationFromFile();