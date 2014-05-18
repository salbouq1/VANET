#include "Elements.h"
///////////////////////////////////////////////////////////////
Car::Car(int id, Block* block, Map* carmap, double trust, bool honsty, int drate)
	: identity(id), currentBlock(block), geomap(carmap), 
	role_trust(trust), honesty(honsty), detection_rate(drate)
{
	//initialize car speed
	speed = CAR_SPEED_MIN + rand() % CAR_SPEED_DELTA;
	//randomize a inner-block offset
	inBlockOffset = rand() % BLOCKWIDTH;
	//determine the direction
	if(currentBlock->blockType == NorthSouthBlock)
	{//north = 0, south = 2
		driving_direction = GetDirection((rand() % 2) * 2);
	}
	else if(currentBlock->blockType == WestEastBlock)
	{//west = 3, east = 1
		driving_direction = GetDirection((rand() % 2) * 2 + 1);
	}
	else
	{//crossing block type
		driving_direction = GetDirection(rand() % 4);
	}

	this->currentBlock->carSet.insert(this);
}

//move forward, please keep time_sec as small as 1, e.g.
Block* Car::Move(int time_sec)
{
	int moveDistance = speed * time_sec;
	inBlockOffset = moveDistance + inBlockOffset;
	if (inBlockOffset > BLOCKWIDTH)
	{//the case that moving out of current block
		inBlockOffset = inBlockOffset % BLOCKWIDTH;
		currentBlock->carSet.erase(this);
		currentBlock = this->geomap->GetNextBlock(currentBlock, driving_direction, 
			&driving_direction);	
		currentBlock->carSet.insert(this);
	}
	//return the block after move
	return currentBlock;
}

//change current block to the new block
bool Car::ChangeLocation(Block* current_block_location, Block* new_block_location)
{
	current_block_location->carSet.erase(this);
	new_block_location->carSet.insert(this);
	this->currentBlock = new_block_location;

	if(current_block_location == new_block_location)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Car::IfTrustOpinionAvailable(Message* message)
{
	if(TURN_TRUST_OPINION_ON == false)
	{
		return false;
	}
	//IMPORTANT
	//assume trust opinions are only available
	//within TRUST_OPINION_MAX_AVAIL_DISTANCE_BLOCK
	double messageDistance = this->geomap->BlockDistance(
		message->blockOfMessageOrigin, this->currentBlock);
	if(messageDistance <= TRUST_OPINION_MAX_AVAIL_DISTANCE_BLOCK)
	{	
		return true;
	}
	else
	{
		return false;
	}
}

TrustOpinion Car::GenerateTrustOpinion(Message* message)
{
	double messageDistance = this->geomap->BlockDistance(
		message->blockOfMessageOrigin, this->currentBlock);
	//IMPORTANT//////////////////////////////////////////
	//assume the confidence is dependent on distance of message
	double confidence = 
		(TRUST_OPINION_MAX_AVAIL_DISTANCE_BLOCK + 1 - messageDistance) 
		/ (TRUST_OPINION_MAX_AVAIL_DISTANCE_BLOCK + 1);
	/////////////////////////////////////////////////////
	//case of authority
	if(this->role_trust >= 0.9)
	{
		return TrustOpinion(this, message->actualQuality, 1.0);
	}
	////////////////////////////////////
	if(this->honesty == true)
	{
		if(FULL_DETECT_ON == true)
			return TrustOpinion(this, message->actualQuality, confidence);
		
		if(this->detection_rate > (rand() % 100))
		{//detected
			return TrustOpinion(this, message->actualQuality, confidence);
		}
		else
		{//not detected
			return TrustOpinion(this, true, confidence);		
		}
	}
	else//malicious peers
	{
		return TrustOpinion(this, true, confidence);		
	}
}

bool Car::ActionModule(Message* message)
{
	//if(this->IfTrustOpinionAvailable(message) == true)
	//{
	//	TrustOpinion trop = this->GenerateTrustOpinion(message);
	//	return trop.trustAction;
	//}
	//////////////////////////
	//compute the average confidence
	double dividee = 0, divider = 0;
	//add sender trust
	double cs = message->senderConfidence;
	double ts;
	if(message->sender->role_trust >= 0)
	{
		ts = message->sender->role_trust;
	}
	else
	{
		ts = (1 + exp_trust[message->sender->identity]) * 0.5;
	}

	dividee += GAMMA * cs * ts;
	divider += GAMMA * ts;
	//add evaluators
	for(size_t i = 0; i < message->vecTrustOpinion.size(); i++)
	{
		double ci = message->vecTrustOpinion[i].trustConfidence;
		double ti;
		if(message->vecTrustOpinion[i].evaluator->role_trust >= 0)
		{
			ti = message->vecTrustOpinion[i].evaluator->role_trust;
		}
		else
		{
			int identity =  message->vecTrustOpinion[i].evaluator->identity;
			ti = (1 + this->exp_trust[identity]) * 0.5;
		}

		//if the evalutor is not trusted then skip to the next one
		if (ti < TAU) continue;
		//other put its evaluations into consideration
		if(message->vecTrustOpinion[i].trustAction == true)
		{
			dividee += ci * ti;
		}
		else
		{
			dividee -= ci * ti;
		}

		divider += ti;
	}

	//////////////////////////////////////
	bool finalAction;
	//if no trust opinion is there attached
	if(message->vecTrustOpinion.size() == 0)
	{
		finalAction = (ts >= TAU);
	}
	else
	{//other compute the ta and make a trust decision
		finalAction = (dividee >= divider * PHI);
	}
	//if there is any trust opinion from the authority, take it
	if(ROLE_TRUST_ON == true)
	{
		for(size_t i = 0; i < message->vecTrustOpinion.size(); i++)
		{
			Car* eva = message->vecTrustOpinion[i].evaluator;
			if(eva->role_trust >= 0.9)
			{
				finalAction = message->vecTrustOpinion[i].trustAction;
				break;
			}
		}
	}
	/////////////////////////////////////
	//count the mistake a node makes
	if(finalAction != message->actualQuality)
		geomap->logger.numVictim[this->identity] ++ ;

	////////////////////////////////////
	// code for detection rate of bad messages
	//if(this->identity == 5000)
	//{
	//	geomap->logger.numReceivedMessage[5000] ++;
	//	if(finalAction == message->actualQuality)
	//		geomap->logger.numDetectedMessage[5000] ++;
	//}
	if(message->actualQuality == false && finalAction == false)
	{
		geomap->logger.numDetectedMessage[this->identity]++;
	}

	geomap->logger.numReceivedMessage[this->identity]++;

	return finalAction;
}

Message* Car::SendMessage(bool message_quality, double confidence)
{
	int currentTime = this->geomap->global_time;

	Message* badMsg = new Message(
		this, confidence, message_quality, currentTime, currentBlock, geomap);
	return badMsg;
}


void Car::UpdateExperienceBasedTrust(Message* message)
{
	if(EXP_TRUST_ON == false)
	{
		return;
	}
	//////////////////////////////////
	// SPECIAL
	// this part of code is used to count the bad message
	// detection rate from a peer's perspective
	// To speed up, only Car[5000] will update exp-trust for others
	//if (this->identity != 5000)
	//	return;
	// SPECIAL
	///////////////////////////////////


	//Handle Sender First!!!
	//sender cannot update the trust for himself
	if(this != message->sender)
	{	
		int senderID = message->sender->identity;
		if(message->actualQuality == true)
		{//increase sender's trust
			exp_trust[senderID] = IncreaseExperienceBasedTrust(
				exp_trust[senderID], message->senderConfidence, 
				message->timeStamp - last_interaction_time[senderID]);
		}
		else
		{
			exp_trust[senderID] = DecreaseExperienceBasedTrust(
				exp_trust[senderID], message->senderConfidence, 
				message->timeStamp - last_interaction_time[senderID]);
		}
		//update the interaction time with sender
		last_interaction_time[senderID] = message->timeStamp;
	}
	//increase the trust for all good peers
	//decrease the trust for all bad peers
	for(vector<TrustOpinion>::const_iterator tropIter = message->vecTrustOpinion.begin();
		tropIter != message->vecTrustOpinion.end(); tropIter ++)
	{
		//evaluator cannot update the trust for itself
		if(this == tropIter->evaluator)
			continue;
		//otherwise, update the trust for each evaluator
		int evaluatorID = tropIter->evaluator->identity;
		if(tropIter->trustAction == message->actualQuality)
		{
			exp_trust[evaluatorID] = IncreaseExperienceBasedTrust(
				exp_trust[evaluatorID], tropIter->trustConfidence,
				message->timeStamp - last_interaction_time[evaluatorID]);
		}
		else
		{
			exp_trust[evaluatorID] = DecreaseExperienceBasedTrust(
				exp_trust[evaluatorID], tropIter->trustConfidence,
				message->timeStamp - last_interaction_time[evaluatorID]);
		}
		//update interaction time stamps
		last_interaction_time[evaluatorID] = message->timeStamp;
	}

	return;
}

double Car::IncreaseExperienceBasedTrust(double currentTrust, double confidence, 
										 int time_difference_from_last_interaction)
{
	double newTrust = 0, t_diff = 1.0;

	if(time_difference_from_last_interaction < MAX_TIME_DIFFERENCE)
	{
		t_diff = (double)time_difference_from_last_interaction / (double)MAX_TIME_DIFFERENCE;
	}

	if(currentTrust >= 0.0)
	{
		newTrust = pow(LAMBDA, t_diff) * (1 - confidence * ALPHA) * currentTrust 
			+ confidence * ALPHA;
	}
	else
	{
		newTrust = pow(LAMBDA, -t_diff) * (1 + confidence * ALPHA) * currentTrust
			+ confidence * ALPHA;
	}

	if(newTrust > 1)
	{
		//cout << "newTrust > 1" << endl << flush;
		newTrust = 1;	
	}

	return newTrust;
}

double Car::DecreaseExperienceBasedTrust(double currentTrust, double confidence, 
										 int time_difference_from_last_interaction)
{
	double newTrust = 0, t_diff = 1.0;

	if(time_difference_from_last_interaction < MAX_TIME_DIFFERENCE)
	{
		t_diff = (double)time_difference_from_last_interaction / (double)MAX_TIME_DIFFERENCE;
	}

	if(currentTrust >= 0.0)
	{
		newTrust = pow(LAMBDA, t_diff) * (1 + confidence * BETA) * currentTrust 
			- confidence * BETA;
	}
	else
	{
		newTrust = pow(LAMBDA, -t_diff) * (1 - confidence * BETA) * currentTrust
			- confidence * BETA;
	}
	
	if(newTrust < -1)
	{
		//cout << "newTrust < -1" << endl << flush;
		newTrust = -1;	
	}

	return newTrust;
}
