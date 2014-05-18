#include "Elements.h"

/////////////////////////////////////////////////////////////
Message::Message(Car* sender_ptr, double sender_confidence, bool actual_quality, 
				 int time_stamp, Block* block, Map* global_map)
{
	sender = sender_ptr;
	senderConfidence = sender_confidence;
	actualQuality = actual_quality;
	timeStamp = time_stamp;
	blockOfMessageOrigin = block;
	geomap = global_map;

	static int msgID = 0;
	messageID = ++ msgID;
}

Message::Message(Message* message)
{
	sender = message->sender;
	senderConfidence = message->senderConfidence;
	actualQuality = message->actualQuality;
	timeStamp = message->timeStamp;
	blockOfMessageOrigin = message->blockOfMessageOrigin;
	geomap = message->geomap;
	messageID = message->messageID;

	vecTrustOpinion = message->vecTrustOpinion;
}

int Message::Propagate()
{
	//first of all, locally propagate
	this->BroadcastToBlock(this->blockOfMessageOrigin);
	if(this->GetMajorityRelayDecision(this->blockOfMessageOrigin) == false)
	{//drop the message
		//the propagation is local, so it is one
		return 1;
	}
	//otherwise, propagate to each direction
	int propDistance = 1;
	switch(this->sender->driving_direction)
	{
	case North:
		propDistance += this->Propagate(South);
		break;
	case South:
		propDistance += this->Propagate(North);
		break;
	case East:
		propDistance +=	this->Propagate(West);
		break;
	case West:
		propDistance += this->Propagate(East);
		break;
	}
	//cout << "Message " << this->messageID << ": propDistance = " << propDistance << endl;

	return propDistance;
}

int Message::Propagate(Direction direction_of_propagation)
{
	int delta_x, delta_y;
	if(direction_of_propagation == East)
	{
		delta_x = 0; delta_y = 1;
	}
	else if(direction_of_propagation == West)
	{
		delta_x = 0; delta_y = -1;
	}
	else if(direction_of_propagation == North)
	{
		delta_x = -1; delta_y = 0;
	}
	else //(direction_of_propagation == South)
	{
		delta_x = 1; delta_y = 0;
	}

	Block* currentMessageBlock = this->blockOfMessageOrigin;
	int propDistance = 0;

	for(int propCount = 1; propCount <= MESSAGE_MAX_PROPAGATION_DISTANCE_BLOCK; propCount++)
	{
		Block* nextMessageBlock = this->geomap->GetBlockByXY(
			currentMessageBlock->x + delta_x, 
			currentMessageBlock->y + delta_y );
		if(nextMessageBlock != NULL)
		{
			currentMessageBlock = nextMessageBlock;
		}
		else
		{
			break;
		}		

		propDistance = propCount;
		//now ask each peer in the current block to evaluate
		this->BroadcastToBlock(currentMessageBlock);

		//compute the relay decision from the majority
		if(this->GetMajorityRelayDecision(currentMessageBlock) == false)
		{//drop the message
			break;
		}
		//otherwise, moving onto the next block
	}
	return propDistance;
}

void Message::BroadcastToBlock(Block* block)
{
	//this is for the statistics of number of messages per block
	if(this->blockOfMessageOrigin != block)
	{
		geomap->logger.numReceivedMessagePerBlock[block->blockID] ++;
		if(this->actualQuality == false)
		{
			geomap->logger.numReceivedBadMessagePerBlock[block->blockID] ++;
		}
	}
	//step1, collect trust opinions if there is any
	//int count = 0;
	for(set<Car*>::const_iterator carIter = block->carSet.begin();
		carIter != block->carSet.end(); carIter++)
	{
		Car* evaluator = *carIter;
		if(evaluator == this->sender)
		{//the sender cannot have the trust opinion for itself
			continue;
		}

		if(evaluator->IfTrustOpinionAvailable(this))
		{
			TrustOpinion trop = evaluator->GenerateTrustOpinion(this);
			this->vecTrustOpinion.push_back(trop);
			//count++;
		}
	}

	//cout << "Trust Availability: " << count << " / " << block->carSet.size() << endl;

	//step2, those without trustopinions, derive a local decision
	for(set<Car*>::const_iterator carIter = block->carSet.begin();
		carIter != block->carSet.end(); carIter++)
	{
		Car* car = *carIter;
		if(car != this->sender)
			car->ActionModule(this);
	}

	//step3, each peer in the block updates exp-based trust
	for(set<Car*>::const_iterator carIter = block->carSet.begin();
		carIter != block->carSet.end(); carIter++)
	{
		Car* receiver = *carIter;
		receiver->UpdateExperienceBasedTrust(this);
	}

/////////////////////////////////////////////////////////////////////////
//this line of code is used to count the social popularity of the sender, or say
//the number of deliveries
	for(set<Car*>::const_iterator carIter = block->carSet.begin();
		carIter != block->carSet.end(); carIter++)
	{
		Car* receiver = *carIter;
		if(this->blockOfMessageOrigin != block)
		{
			geomap->logger.interaction[this->sender->identity][receiver->identity] ++;
		}
	}
/////////////////////////////////////////////////////////////////////////
}

bool Message::GetMajorityRelayDecision(Block* block_message)
{
	//if there is no car in this block, drop it
	if(block_message->carSet.empty())
		return false;
	if(RELAY_CONTROL_ON == false)
		return true;
	//if the message has no trust opinion, relay it
	if(vecTrustOpinion.empty())
		return true;
	//if there is an authority, simply trust the authority
	for(set<Car*>::iterator carIter = block_message->carSet.begin();
		carIter != block_message->carSet.end();
		carIter ++)
	{
		Car* car = (*carIter);
		//in case of authority
		if(ROLE_TRUST_ON == true && car->role_trust >= 0.99)
			return this->actualQuality;
	}
	//otherwise compute the majority opinion
	double weight_trust = 0, weight_distrust = 0;

	//the relayer is randomly picked up
	//to make it simple, choose the first car in the block
	set<Car*>::iterator relayerIter =  block_message->carSet.begin();
	Car* relayer = (*relayerIter);

	for(vector<TrustOpinion>::iterator trop = vecTrustOpinion.begin();
		trop != vecTrustOpinion.end();
		trop ++)
	{
		double ti; // trust for each evaluator
		if(trop->evaluator->role_trust >= 0)
		{
			ti = trop->evaluator->role_trust;
		}
		else
		{
			ti = (relayer->exp_trust[trop->evaluator->identity] + 1 ) / 2.0;
		}

		//those whose trust is below the threshold are ignored
		if(ti < TAU) continue;

		if(trop->trustAction == true)
		{
			weight_trust += trop->trustConfidence * ti;
		}
		else
		{
			weight_distrust += trop->trustConfidence * ti;
		}
	}

	bool decision;
	if(weight_trust > (1-RELAY_THESHOLD) * (weight_trust + weight_distrust))
	{
		decision = true;
	}
	else
	{
		decision = false;
	}

	return decision;
}
