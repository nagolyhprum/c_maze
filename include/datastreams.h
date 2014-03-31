#include "serializable.h"
#include <string>
#include <vector>
#include "datastream.h"

class user;
class character;
class maze;
class statistics;

class user : public serializable {
public:
	std::string fullname;
	std::string username;
	std::string password;
	std::vector<serializable *> characters;
	void serialize(datastream ds) {
		ds & fullname;
		ds & username;
		ds & password;
		ds & characters;
	};
	user(int id) : serializable("user", id) {};
	user() : serializable("user") {};
};
serializable * get_user(int id) {
	user * s = new user(id);
	s->load();
	return s;
}

class character : public serializable {
public:
	std::string charactername;
	serializable * current;
	serializable * max;
	void serialize(datastream ds) {
		ds & charactername;
		ds & current;
		ds & max;
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
void datastream::register_constructors() {
	datastream::register_constructor(get_user, "user");
	datastream::register_constructor(get_character, "character");
	datastream::register_constructor(get_statistics, "statistics");
}