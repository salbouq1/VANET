#include "Elements.h"

vector<double> Collector::Average()
{
	vector<double> avgResult;

	size_t size = data.size();
	size_t length = (data.begin()->second).size();
	for(size_t i = 0; i < length; i++)
	{
		double value = 0;
		for(map<int, vector<double> >::const_iterator iter = data.begin();
			iter != data.end(); iter++)
		{
			value += (iter->second)[i];
		}
		value = value / (double)size;
		avgResult.push_back(value);
	}
	return avgResult;
}