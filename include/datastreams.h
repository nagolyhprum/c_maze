#include "serializable.h"
#include <string>
#include <vector>
#include "datastream.h"
class badge;
class behavior;
class character;
class enemy;
class item;
class maze;
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
	void copyTo(user * s) {
		s->fullname = fullname;
		s->username = username;
		s->password = password;
		s->characters = characters;
		s->behaviors = behaviors;
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
	void copyTo(behavior * s) {
		s->category = category;
		s->subcategory = subcategory;
		s->current = current;
		s->required = required;
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
	serializable * maze;
	int x;
	int y;
	int column;
	int row;
	character(int id) : serializable("character", id) {
		charactername = "";
		current = NULL;
		max = NULL;
		equipment = std::vector<serializable *>();
		inventory = std::vector<serializable *>();
		maze = NULL;
		x = 0;
		y = 0;
		column = 0;
		row = 0;
	};
	character() : serializable("character") {
		charactername = "";
		current = NULL;
		max = NULL;
		equipment = std::vector<serializable *>();
		inventory = std::vector<serializable *>();
		maze = NULL;
		x = 0;
		y = 0;
		column = 0;
		row = 0;
	};
	void serialize(datastream ds) {
		ds & charactername;
		ds & current;
		ds & max;
		ds & equipment;
		ds & inventory;
		ds & maze;
		ds & x;
		ds & y;
		ds & column;
		ds & row;
	};
	void copyTo(character * s) {
		s->charactername = charactername;
		s->current = current;
		s->max = max;
		s->equipment = equipment;
		s->inventory = inventory;
		s->maze = maze;
		s->x = x;
		s->y = y;
		s->column = column;
		s->row = row;
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
	void copyTo(statistics * s) {
		s->level = level;
		s->strength = strength;
		s->defense = defense;
		s->intelligence = intelligence;
		s->resistance = resistance;
	};
	static serializable * get_statistics(int id) {
		return new statistics(id);
	};
};
class maze : public serializable {
public:
	std::vector<serializable *> rooms;
	int columns;
	int rows;
	maze(int id) : serializable("maze", id) {
		rooms = std::vector<serializable *>();
		columns = 0;
		rows = 0;
	};
	maze() : serializable("maze") {
		rooms = std::vector<serializable *>();
		columns = 0;
		rows = 0;
	};
	void serialize(datastream ds) {
		ds & rooms;
		ds & columns;
		ds & rows;
	};
	void copyTo(maze * s) {
		s->rooms = rooms;
		s->columns = columns;
		s->rows = rows;
	};
	static serializable * get_maze(int id) {
		return new maze(id);
	};
};
class room : public serializable {
public:
	int doors;
	std::vector<serializable *> enemies;
	std::vector<serializable *> items;
	room(int id) : serializable("room", id) {
		doors = 0;
		enemies = std::vector<serializable *>();
		items = std::vector<serializable *>();
	};
	room() : serializable("room") {
		doors = 0;
		enemies = std::vector<serializable *>();
		items = std::vector<serializable *>();
	};
	void serialize(datastream ds) {
		ds & doors;
		ds & enemies;
		ds & items;
	};
	void copyTo(room * s) {
		s->doors = doors;
		s->enemies = enemies;
		s->items = items;
	};
	static serializable * get_room(int id) {
		return new room(id);
	};
};
class enemy : public serializable {
public:
	std::string name;
	std::string resources;
	serializable * current;
	serializable * max;
	int lastMove;
	int x;
	int y;
	enemy(int id) : serializable("enemy", id) {
		name = "";
		resources = "";
		current = NULL;
		max = NULL;
		lastMove = 0;
		x = 0;
		y = 0;
	};
	enemy() : serializable("enemy") {
		name = "";
		resources = "";
		current = NULL;
		max = NULL;
		lastMove = 0;
		x = 0;
		y = 0;
	};
	void serialize(datastream ds) {
		ds & name;
		ds & resources;
		ds & current;
		ds & max;
		ds & lastMove;
		ds & x;
		ds & y;
	};
	void copyTo(enemy * s) {
		s->name = name;
		s->resources = resources;
		s->current = current;
		s->max = max;
		s->lastMove = lastMove;
		s->x = x;
		s->y = y;
	};
	static serializable * get_enemy(int id) {
		return new enemy(id);
	};
};
class mazemodel : public serializable {
public:
	std::string name;
	std::string resources;
	std::vector<serializable *> roommodels;
	int columns;
	int rows;
	mazemodel(int id) : serializable("mazemodel", id) {
		name = "";
		resources = "";
		roommodels = std::vector<serializable *>();
		columns = 0;
		rows = 0;
	};
	mazemodel() : serializable("mazemodel") {
		name = "";
		resources = "";
		roommodels = std::vector<serializable *>();
		columns = 0;
		rows = 0;
	};
	void serialize(datastream ds) {
		ds & name;
		ds & resources;
		ds & roommodels;
		ds & columns;
		ds & rows;
	};
	void copyTo(mazemodel * s) {
		s->name = name;
		s->resources = resources;
		s->roommodels = roommodels;
		s->columns = columns;
		s->rows = rows;
	};
	static serializable * get_mazemodel(int id) {
		return new mazemodel(id);
	};
};
class roommodel : public serializable {
public:
	std::vector<serializable *> enemymodels;
	roommodel(int id) : serializable("roommodel", id) {
		enemymodels = std::vector<serializable *>();
	};
	roommodel() : serializable("roommodel") {
		enemymodels = std::vector<serializable *>();
	};
	void serialize(datastream ds) {
		ds & enemymodels;
	};
	void copyTo(roommodel * s) {
		s->enemymodels = enemymodels;
	};
	static serializable * get_roommodel(int id) {
		return new roommodel(id);
	};
};
class enemymodel : public serializable {
public:
	std::string name;
	std::string resources;
	serializable * statistics;
	int x;
	int y;
	enemymodel(int id) : serializable("enemymodel", id) {
		name = "";
		resources = "";
		statistics = NULL;
		x = 0;
		y = 0;
	};
	enemymodel() : serializable("enemymodel") {
		name = "";
		resources = "";
		statistics = NULL;
		x = 0;
		y = 0;
	};
	void serialize(datastream ds) {
		ds & name;
		ds & resources;
		ds & statistics;
		ds & x;
		ds & y;
	};
	void copyTo(enemymodel * s) {
		s->name = name;
		s->resources = resources;
		s->statistics = statistics;
		s->x = x;
		s->y = y;
	};
	static serializable * get_enemymodel(int id) {
		return new enemymodel(id);
	};
};
void datastream::register_constructors() {
	register_constructor(user::get_user, "user");
	register_constructor(behavior::get_behavior, "behavior");
	register_constructor(character::get_character, "character");
	register_constructor(statistics::get_statistics, "statistics");
	register_constructor(maze::get_maze, "maze");
	register_constructor(room::get_room, "room");
	register_constructor(enemy::get_enemy, "enemy");
	register_constructor(mazemodel::get_mazemodel, "mazemodel");
	register_constructor(roommodel::get_roommodel, "roommodel");
	register_constructor(enemymodel::get_enemymodel, "enemymodel");
}