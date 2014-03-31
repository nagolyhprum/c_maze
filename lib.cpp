#include "include/lib.h"
#include <sstream> 
#include <fstream> 
#include <direct.h>
#include <unistd.h>

std::string to_string(int i) {
	std::stringstream ss;
	ss << i;
	return ss.str();
}

int get_id(std::string name) {	
	int id = 0;
	std::fstream file;
	mkdir("data/");
	mkdir(("data/" + name).c_str());
	file.open(("data/" + name + "/ds.conf").c_str(), std::fstream::in);
	file >> id;
	file.close();
	id++;
	file.open(("data/" + name + "/ds.conf").c_str(), std::fstream::out | std::fstream::trunc);
	file << id;	
	file.close();
	return id;
}