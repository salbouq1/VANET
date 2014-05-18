#include "Elements.h"

void init_random(unsigned int seed)
{
		srand(seed);
}

Direction GetDirection(int value)
{
	switch( (value+4) % 4)
	{  
	case 0: return North;
	case 1: return East;
	case 2: return South;
	case 3: return West;
	}

	throw new std::exception();
}

////////////////////////////////////////////////////////////
TrustOpinion::TrustOpinion(Car* evaluator_car, bool trust_action, double trust_confidence)
{
	evaluator = evaluator_car;
	trustAction = trust_action;
	trustConfidence = trust_confidence;
}


//////////////////////////////////////////////////////////////
Block::	Block(int block_id, int x_coord, int y_coord, BlockType block_type)
{
	blockID = block_id;
	x = x_coord;
	y = y_coord;
	blockType = block_type;
}

