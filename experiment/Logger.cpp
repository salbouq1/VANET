#include "Elements.h"
int logger_file_index_number;

Logger::Logger()
{
	stringstream ss; 
	string log_file;
	ss << "log" << ++logger_file_index_number << ".txt";
	ss >> log_file;
	
	fout.open(log_file.c_str(), fstream::out);
	if(fout.fail())
	{
		cout << "Error open " << log_file << endl;
		exit(1);
	}
}
