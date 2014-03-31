#ifndef DATASTREAM
#define DATASTREAM

class datastream;

#include <fstream>
#include <string>
#include <vector>
#include "serializable.h"

class datastream {
private:
	std::string name;
	int id;
	bool isRead;
	std::fstream * file;	
	void init(std::string, int, bool);
public:	
	datastream(std::string, int, bool);
	datastream(std::string);
	datastream(datastream &);
	void operator &(std::string &);
	void operator &(int &);		
	void operator &(serializable * &);		
	void operator &(std::vector<serializable *> &);	
	static std::vector<serializable * (*)(int)> * constructor_functions;
	static std::vector<std::string> * constructor_names;
	static void register_constructors();
	static void register_constructor(serializable * (*)(int), std::string);
	void close();
};
#endif