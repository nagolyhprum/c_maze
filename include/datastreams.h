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
	user(int id) : serializable("user", id) {
		fullname = "";
		username = "";
		password = "";
		characters = std::vector<serializable *>();
		behaviors = std::vector<serializable *>();
	};
	user() : serializable("user") {
		fullname = "";
		username = "";
		password = "";
		characters = std::vector<serializable *>();
		behaviors = std::vector<serializable *>();
	};
	void serialize(datastream ds) {
		ds & fullname;
		ds & username;
		ds & password;
		ds & characters;
		ds & behaviors;
	};
	~user() {
		for(int i = 0; i < characters.size(); i++) {
			delete characters[i];
		}
		for(int i = 0; i < behaviors.size(); i++) {
			delete behaviors[i];
		}
	};
	static serializable * get_user(int id) {
		return new user(id);
	};
};
class behavior : public serializable {
public:
	std::string category;
	std::string subcategory;
	int current;
	int required;
	behavior(int id) : serializable("behavior", id) {
		category = "";
		subcategory = "";
		current = 0;
		required = 0;
	};
	behavior() : serializable("behavior") {
		category = "";
		subcategory = "";
		current = 0;
		required = 0;
	};
	void serialize(datastream ds) {
		ds & category;
		ds & subcategory;
		ds & current;
		ds & required;
	};
	~behavior() {
	};
	static serializable * get_behavior(int id) {
		return new behavior(id);
	};
};
class character : public serializable {
public:
	std::string charactername;
	serializable * current;
	serializable * max;
	std::vector<serializable *> equipment;
	std::vector<serializable *> inventory;
	serializable * map;
	character(int id) : serializable("character", id) {
		charactername = "";
		current = NULL;
		max = NULL;
		equipment = std::vector<serializable *>();
		inventory = std::vector<serializable *>();
		map = NULL;
	};
	character() : serializable("character") {
		charactername = "";
		current = NULL;
		max = NULL;
		equipment = std::vector<serializable *>();
		inventory = std::vector<serializable *>();
		map = NULL;
	};
	void serialize(datastream ds) {
		ds & charactername;
		ds & current;
		ds & max;
		ds & equipment;
		ds & inventory;
		ds & map;
	};
	~character() {
		delete current;
		delete max;
		for(int i = 0; i < equipment.size(); i++) {
			delete equipment[i];
		}
		for(int i = 0; i < inventory.size(); i++) {
			delete inventory[i];
		}
		delete map;
	};
	static serializable * get_character(int id) {
		return new character(id);
	};
};
class statistics : public serializable {
public:
	int level;
	int strength;
	int defense;
	int intelligence;
	int resistance;
	statistics(int id) : serializable("statistics", id) {
		level = 0;
		strength = 0;
		defense = 0;
		intelligence = 0;
		resistance = 0;
	};
	statistics() : serializable("statistics") {
		level = 0;
		strength = 0;
		defense = 0;
		intelligence = 0;
		resistance = 0;
	};
	void serialize(datastream ds) {
		ds & level;
		ds & strength;
		ds & defense;
		ds & intelligence;
		ds & resistance;
	};
	~statistics() {
	};
	static serializable * get_statistics(int id) {
		return new statistics(id);
	};
};
class map : public serializable {
public:
	std::vector<serializable *> rooms;
	int columns;
	int rows;
	map(int id) : serializable("map", id) {
		rooms = std::vector<serializable *>();
		columns = 0;
		rows = 0;
	};
	map() : serializable("map") {
		rooms = std::vector<serializable *>();
		columns = 0;
		rows = 0;
	};
	void serialize(datastream ds) {
		ds & rooms;
		ds & columns;
		ds & rows;
	};
	~map() {
		for(int i = 0; i < rooms.size(); i++) {
			delete rooms[i];
		}
	};
	static serializable * get_map(int id) {
		return new map(id);
	};
};
class room : public serializable {
public:
	std::vector<serializable *> enemies;
	std::vector<serializable *> items;
	room(int id) : serializable("room", id) {
		enemies = std::vector<serializable *>();
		items = std::vector<serializable *>();
	};
	room() : serializable("room") {
		enemies = std::vector<serializable *>();
		items = std::vector<serializable *>();
	};
	void serialize(datastream ds) {
		ds & enemies;
		ds & items;
	};
	~room() {
		for(int i = 0; i < enemies.size(); i++) {
			delete enemies[i];
		}
		for(int i = 0; i < items.size(); i++) {
			delete items[i];
		}
	};
	static serializable * get_room(int id) {
		return new room(id);
	};
};
void datastream::register_constructors() {
	register_constructor(user::get_user, "user");
	register_constructor(behavior::get_behavior, "behavior");
	register_constructor(character::get_character, "character");
	register_constructor(statistics::get_statistics, "statistics");
	register_constructor(map::get_map, "map");
	register_constructor(room::get_room, "room");
}