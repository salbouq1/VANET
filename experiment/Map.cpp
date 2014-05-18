#include "Elements.h"

//////////////////////////////////////////////////////////////
Block* Map::GetNextBlock(Block* currentBlock, Direction currentDir, Direction* newDir)
{
	int x = currentBlock->x, y = currentBlock->y;
	//change the direction if at a crossing block
	if(currentBlock->blockType == CrossingBlock)
	{
		int decision = rand() % 100;
		if( decision < CAR_L_TRUN_PROB_PRECENT )
		{
			*newDir = GetDirection( currentDir - 1 );
		}			
		else if( decision > 100 - CAR_R_TRUN_PROB_PRECENT )
		{
			*newDir = GetDirection( currentDir + 1 );
		}
	}

	if(currentDir == North)
	{
		if (x == 0 || bMap[x-1][y] == NULL)
		{
			*newDir = South;
			return currentBlock;
		}
		else
		{
			*newDir = North;
			return bMap[x-1][y];
		}		
	}		
	else if(currentDir == South)
	{
		if (x+1 >= sizeX || bMap[x+1][y] == NULL)
		{
			*newDir = North;
			return currentBlock;
		}
		else
		{
			*newDir = South;
			return bMap[x+1][y];
		}		
	}
	else if(currentDir == West)
	{
		if(y == 0 || bMap[x][y-1] == NULL)
		{
			*newDir = East;
			return currentBlock;
		}
		else
		{
			*newDir = West;
			return bMap[x][y-1];
		}
	}
	else//East currentDir
	{
		if (y+1 >= sizeY || bMap[x][y+1] == NULL)
		{
			*newDir = West;
			return currentBlock;
		}
		else
		{
			*newDir = East;
			return bMap[x][y+1];
		}	
	}
}

Block* Map::GetBlockByXY(int x, int y)
{
	return bMap[x][y];
}

Block* Map::GetBlockByBlockID(int block_id)
{
	return vecBlock[block_id];
}

Map::Map(string strFile)
{
	global_time = 0;
	centralAuthority = NULL;

	ifstream fs(strFile.c_str(), ifstream::in);
	if(fs.fail())
	{
		cout << "fail to open " << strFile <<endl;
		exit(1);
	}

	fs >> sizeX >> sizeY;

	this->BlockCount = 0;
	for (int i = 0; i < sizeX; i++)
	{
		for(int j = 0; j < sizeY; j++)
		{
			int blockID = this->BlockCount;
			int value; fs >> value;
			switch(value)
			{
			case 0:
				bMap[i][j] = NULL; 
				break;
			case 1:
				bMap[i][j] = new Block(blockID, i, j, NorthSouthBlock);
				this->BlockCount ++;
				this->vecBlock.push_back(bMap[i][j]);
				break;
			case 2:
				bMap[i][j] = new Block(blockID, i, j, WestEastBlock);
				this->BlockCount ++;
				this->vecBlock.push_back(bMap[i][j]);
				break;
			case 3:
				bMap[i][j] = new Block(blockID, i, j, CrossingBlock);
				this->BlockCount ++;
				this->vecBlock.push_back(bMap[i][j]);
				break;
			}
		}
	}
}

Map::~Map()
{
	for(size_t i = 0; i < this->vecBlock.size(); i++)
	{
		delete vecBlock[i];
	}

	for(size_t i = 0; i < this->vecCar.size(); i++)
	{
		delete vecCar[i];
	}

	for(size_t i = 0; i < this->vecMessageCache.size(); i++)
	{
		delete vecMessageCache[i];
	}
}

void Map::PrintMap()
{
	for(int i = 0; i < sizeX; i++)
	{
		for(int j = 0; j < sizeY; j++)
		{
			if(bMap[i][j] == NULL) cout << "    ";
			else
			{
				cout.width(4);
				cout << bMap[i][j]->carSet.size();
			}
		}
		cout << endl;
	}
}

double Map::BlockDistance(Block *sourceBlock, Block *destBlock)
{
	double deltaX = (double)sourceBlock->x - (double)destBlock->x;
	double deltaY = (double)sourceBlock->y - (double)destBlock->y;

	return sqrt(deltaX * deltaX + deltaY * deltaY);
}