#include "include/datastreams.h"
#include "include/server.h"
#include "include/lib.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <cstdlib>
#include <ctime>

#define UP 1
#define RIGHT 2
#define DOWN 4
#define LEFT 8

int nextInt(int max) {
	return rand() % max;
}

string nextString(istream * in) {
	int length;
	*in >> length;
	char * s = new char[length];
	in->get();
	in->read(s, length);
	string ret(s, length);
	delete s;
	return ret;
}

int nextInt(istream * in) {
	int i;
	*in >> i;
	return i;
}

string next(istream * in) {
	string s;
	*in >> s;
	return s;
}

void writeInt(ostream * out, int i) {
	*out << i << " ";
}

void writeString(ostream * out, string s) {
	*out << s.length() << " " << s;
}

void sendStatistics(client & c, statistics * s) {
	stringstream ss;
	ss << "statistics ";
	writeInt(&ss, s->getID());
	writeInt(&ss, s->level);
	writeInt(&ss, s->strength);
	writeInt(&ss, s->defense);
	writeInt(&ss, s->intelligence);
	writeInt(&ss, s->resistance);
	c.sendMessage(ss.str());
}

character * getCharacterById(user * u, int id) {
	for(int i = 0; i < u->characters.size(); i++) {
		character * curr = (character *)u->characters[i];
		if(curr->getID() == id) {
			return curr;
		}
	}
	return NULL;
}

void visit(maze * m, bool * visited, int row, int column) {
	visited[row * m->columns + column] = true;
	vector<string> directions;
	if(column > 0) {
		directions.push_back("left");
	}
	if(row > 0) {
		directions.push_back("up");
	}
	if(column + 1 < m->columns) {
		directions.push_back("right");
	}
	if(row + 1 < m->rows) {
		directions.push_back("down");
	}
	while(directions.size() > 0) {
		int index = nextInt(directions.size());
		string next = directions[index];
		directions.erase(directions.begin() + index);
		int nextRow = row, nextColumn = column;
		if(next.compare("up") == 0) {
			nextRow--;
		} else if(next.compare("right") == 0) {
			nextColumn++;
		} else if(next.compare("down") == 0) {
			nextRow++;
		} else if(next.compare("left") == 0) {
			nextColumn--;
		}
		if(!visited[nextRow * m->columns + nextColumn]) {
			room * currentRoom = (room *) m->rooms[row * m->columns + column];
			room * nextRoom = (room *) m->rooms[nextRow * m->columns + nextColumn];
			if(next.compare("up")) {
				currentRoom->doors |= UP;
				nextRoom->doors |= DOWN;
			} else if(next.compare("right")) {
				currentRoom->doors |= RIGHT;
				nextRoom->doors |= LEFT;
			} else if(next.compare("down")) {
				currentRoom->doors |= DOWN;
				nextRoom->doors |= UP;
			} else if(next.compare("left")) {
				currentRoom->doors |= LEFT;
				nextRoom->doors |= RIGHT;
			}
			visit(m, visited, nextRow, nextColumn);
		}
	}
}

void mazeCmd(client & c, stringstream * ss) {
	if(c.associated != NULL) {
		user * u = (user *)c.associated;
		if(!ss->eof()) {
			character * chr = getCharacterById(u, nextInt(ss));
			if(chr->maze != NULL) { //cant make a maze until they are done this one
				stringstream res;
				//get one
				c.sendMessage(res.str());
			} else if(!ss->eof()) {
				int id = nextInt(ss);
				c.sendMessage("log maze generate");
				maze * m = (maze *)(chr->maze = new maze());
				chr->maze = m;
				//generate
				mazemodel * mm = new mazemodel(id);
				mm->load();				
				m->rows = mm->rows;
				m->columns = mm->columns;
				//setup walls
				bool * visited = new bool[mm->columns * mm->rows];
				for(int i = 0; i < mm->columns * mm->rows; i++) {
					visited[m->rooms.size()] = false;
					room * r = new room();
					//setup enemies
					int index = nextInt(mm->roommodels.size());
					roommodel * rm = (roommodel *) mm->roommodels[index];
					mm->roommodels.erase(mm->roommodels.begin() + index);
					for(int j = 0; j < rm->enemymodels.size(); j++) {
						enemymodel * em = (enemymodel *) rm->enemymodels[j];
						enemy * e = new enemy();
						e->name = em->name;
						e->resources = em->resources;
						e->x = em->x;
						e->y = em->y;
						e->current = new statistics();
						statistics * s = (statistics *) em->statistics;
						s->copyTo((statistics *) e->current);
						e->max = new statistics();
						s->copyTo((statistics *) e->max);
						r->enemies.push_back(e);
					}
					//enemies are all good
					m->rooms.push_back(r);
				}
				visit(m, visited, 0, 0);
				//walls are all good
				chr->save();
				delete visited;
				delete mm;
				delete m;
				stringstream get(to_string(chr->getID()));
				mazeCmd(c, &get);
			}
		} else {
			//show maze options
		}
	}
}

void charactersCmd(client & c, stringstream * ss) {
	if(c.associated != NULL) {
		string cmd = next(ss);
		user * u = (user *)c.associated;
		if(!cmd.compare("getall")) {
			for(int i = 0; i < u->characters.size(); i++) {
				character * chr = (character *)u->characters[i];
				stringstream getone("getone " + to_string(chr->getID()));
				charactersCmd(c, &getone);
			}
		} else if(!cmd.compare("add")) {
			character * chr = new character();
			chr->charactername = nextString(ss);
			chr->current = new statistics();
			chr->max = new statistics();
			u->characters.push_back(chr);
			u->save();
			stringstream getone("getone " + to_string(chr->getID()));
			charactersCmd(c, &getone);
		} else if(!cmd.compare("getone")) {
			stringstream res;
			res << "character ";
			int id = nextInt(ss);
			character * chr = getCharacterById(u, id);
			writeInt(&res, id);
			writeInt(&res, chr->current->getID());
			writeInt(&res, chr->max->getID());
			sendStatistics(c, (statistics *)chr->current);
			sendStatistics(c, (statistics *)chr->max);
			c.sendMessage(res.str());
		}
	}
}

//hash password
void signinCmd(client & c, stringstream * ss) {
	string username = next(ss);
	string password = next(ss);
	fstream in(("accounts/" + username + "/password.data").c_str(), fstream::in);
	if(in.good()) {
		string s = next(&in);
		in.close();
		if(!s.compare(password)) {
			in.open(("accounts/" + username + "/id.data").c_str(), fstream::in);
			if(in.good()) {
				int id = nextInt(&in);
				in.close();
				user * u = new user(id);
				u->load();
				c.associated = u;
				c.sendMessage("log successfully signed in");
				return;
			}
		}
		c.sendMessage("log invalid password");
		return;
	}
	c.sendMessage("log invalid username");
}

//extra validation
//hash password
void signupCmd(client & c, stringstream * ss) {
	string username = next(ss);
	string password = next(ss);
	DIR * dir = opendir(("accounts/" + username).c_str());
	if (dir) {
		c.sendMessage("log that username is already in use");
		closedir(dir);
		return;
	}
	closedir(dir);
	user * u = new user();
	u->fullname = nextString(ss);
	u->username = username;
	u->password = password;
	u->save();
	c.associated = u;
	mkdir("accounts");
	mkdir(("accounts/" + username).c_str());
	fstream out(("accounts/" + username + "/password.data").c_str(), fstream::out);
	out << password;
	out.close();
	out.open(("accounts/" + username + "/id.data").c_str(), fstream::out);
	out << u->getID();
	out.close();
	c.sendMessage("log account created");
}

void onMessage(client & c, string message) {
	c.sendMessage("echo " + message);
	stringstream ss(message);
	string command = next(&ss);
	if(!command.compare("signin")) {
		signinCmd(c, &ss);
	} else if(!command.compare("signup")) {
		signupCmd(c, &ss);
	} else if(!command.compare("characters")) {
		charactersCmd(c, &ss);
	} else if(!command.compare("maze")) {
		mazeCmd(c, &ss);
	}
}

void onClose(client & c) {
}

void onError(client & c, exception e) {
	c.close();
}

void onConnect(client & c) {
}

void makeMazeModels() {
	//a
	mazemodel * mm = new mazemodel();
	enemymodel * em = new enemymodel();
	statistics * s = new statistics();
	roommodel * rm = new roommodel();
	mm->columns = 3;
	mm->rows = 3;
	mm->resources = "small";
	mm->name = "Hedge Maze";
	s->level = 5;
	s->strength = 4;
	s->defense = 3;
	s->intelligence = 2;
	s->resistance = 1;
	em->name = "Skeleton";
	em->resources = "skeleton";
	em->statistics = s;
	rm->enemymodels.push_back(em);
	for(int i = 0; i < 9; i++) {
		mm->roommodels.push_back(rm);
	}
	mm->save();
	delete mm;
}

int main(void) {
	srand(time(NULL));
	datastream::register_constructors();
	makeMazeModels();
	server s;
	s.onMessage(onMessage);
	s.onClose(onClose);
	s.onError(onError);
	s.onConnect(onConnect);
	if(s.connect()) {
		s.start();
	}
}