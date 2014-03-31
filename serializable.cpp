#include "include/serializable.h"
#include "include/lib.h"

std::string serializable::getName() {
	return name;
};

int serializable::getID() {
	return id;
};

serializable::serializable(std::string name) {
	this->name = name;
	id = get_id(name);
};

serializable::serializable(std::string name, int id) {
	this->name = name;
	this->id = id;
};

void serializable::save() {
	datastream ds(getName(), getID(), false);
	serialize(ds);	
	ds.close();
};

void serializable::load() {
	datastream ds(getName(), getID(), true);
	serialize(ds);
	ds.close();
};