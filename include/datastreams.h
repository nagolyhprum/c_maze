#include "serializable.h"
#include <string>
#include <vector>
#include "datastream.h"

class badge;
class behavior;
class character;
class enemy;
class item;
class map;
class room;
class user;
class skill;
class statistics;

class user : public serializable {
public:
	std::string fullname;
	std::string username;
	std::string password;
	std::vector<serializable *> characters;
	std::vector<serializable *> behaviors;
	void serialize(datastream ds) {
		ds & fullname;
		ds & username;
		ds & password;
		ds & characters;
		ds & behaviors;
	};
	user(int id) : serializable("user", id) {};
	user() : serializable("user") {};
};
serializable * get_user(int id) {
	user * s = new user(id);
	s->load();
	return s;
}

class behavior  : public serializable {
public:
	std::string category;
	std::string subcategory;
	int current;
	int required;
	void serialize(datastream ds) {
		ds & category;
		ds & subcategory;
		ds & current;
		ds & required;
	};
	behavior (int id) : serializable("behavior ", id) {};
	behavior () : serializable("behavior ") {};
};
serializable * get_behavior (int id) {
	behavior  * s = new behavior (id);
	s->load();
	return s;
}

class character : public serializable {
public:
	std::string charactername;
	serializable * current;
	serializable * max;
	std::vector<serializable *> equipment;
	std::vector<serializable *> inventory;
	serializable * map;
	void serialize(datastream ds) {
		ds & charactername;
		ds & current;
		ds & max;
		ds & equipment;
		ds & inventory;
		ds & map;
	};
	character(int id) : serializable("character", id) {};
	character() : serializable("character") {};
};
serializable * get_character(int id) {
	character * s = new character(id);
	s->load();
	return s;
}

class statistics : public serializable {
public:
	int level;
	int strength;
	int defense;
	int intelligence;
	int resistance;
	void serialize(datastream ds) {
		ds & level;
		ds & strength;
		ds & defense;
		ds & intelligence;
		ds & resistance;
	};
	statistics(int id) : serializable("statistics", id) {};
	statistics() : serializable("statistics") {};
};
serializable * get_statistics(int id) {
	statistics * s = new statistics(id);
	s->load();
	return s;
}

class map : public serializable {
public:
	std::vector<serializable *> rooms;
	int columns;
	int rows;
	void serialize(datastream ds) {
		ds & rooms;
		ds & columns;
		ds & rows;
	};
	map(int id) : serializable("map", id) {};
	map() : serializable("map") {};
};
serializable * get_map(int id) {
	map * s = new map(id);
	s->load();
	return s;
}

class room : public serializable {
public:
	std::vector<serializable *> enemies;
	std::vector<serializable *> items;
	void serialize(datastream ds) {
		ds & enemies;
		ds & items;
	};
	room(int id) : serializable("room", id) {};
	room() : serializable("room") {};
};
serializable * get_room(int id) {
	room * s = new room(id);
	s->load();
	return s;
}
void datastream::register_constructors() {
	datastream::register_constructor(get_user, "user");
	datastream::register_constructor(get_behavior , "behavior ");
	datastream::register_constructor(get_character, "character");
	datastream::register_constructor(get_statistics, "statistics");
	datastream::register_constructor(get_map, "map");
	datastream::register_constructor(get_room, "room");
}