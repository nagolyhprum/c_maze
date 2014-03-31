#ifndef SERIALIZE
#define SERIALIZE

class serializable;

#include <string>
#include "datastream.h"

class serializable {
private:
	int id;
	std::string name;
public:
	void save();
	void load();
	serializable(std::string, int);
	serializable(std::string);
	std::string getName();
	int getID();
	virtual void serialize(datastream) = 0;
};

#endif