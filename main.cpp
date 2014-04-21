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
	vector<int> directions;
	if(column > 0) {
		directions.push_back(LEFT);
	}
	if(row > 0) {
		directions.push_back(UP);
	}
	if(column + 1 < m->columns) {
		directions.push_back(RIGHT);
	}
	if(row + 1 < m->rows) {
		directions.push_back(DOWN);
	}
	while(directions.size() > 0) {
		int index = nextInt(directions.size());
		int next = directions[index];
		directions.erase(directions.begin() + index);
		int nextRow = row, nextColumn = column;
		if(next == UP) {
			nextRow--;
		} else if(next == RIGHT) {
			nextColumn++;
		} else if(next == DOWN) {
			nextRow++;
		} else if(next == LEFT) {
			nextColumn--;
		}
		if(!visited[nextRow * m->columns + nextColumn]) {
			room * currentRoom = (room *) m->rooms[row * m->columns + column];
			room * nextRoom = (room *) m->rooms[nextRow * m->columns + nextColumn];
			if(next == UP) {
				currentRoom->doors |= UP;
				nextRoom->doors |= DOWN;
			} else if(next == RIGHT) {
				currentRoom->doors |= RIGHT;
				nextRoom->doors |= LEFT;
			} else if(next == DOWN) {
				currentRoom->doors |= DOWN;
				nextRoom->doors |= UP;
			} else if(next == LEFT) {
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
			if(chr != NULL) {
				if(chr->maze != NULL) { //cant make a maze until they are done this one
					stringstream res;
					res << "maze ";
					maze * m = (maze *) chr->maze;
					writeInt(&res, m->rows);
					writeInt(&res, m->columns);
					for(int i = 0; i < m->rooms.size(); i++) {
						room * r = (room *) m->rooms[i];	
						writeInt(&res, r->discovered ? r->doors : 0);
					}
					c.sendMessage(res.str());
				} else if(!ss->eof()) {
					int id = nextInt(ss);
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
							e->max = new statistics();
							statistics * s = (statistics *) em->statistics;
							s->copyTo((statistics *) e->current);
							s->copyTo((statistics *) e->max);
							r->enemies.push_back(e);
						}
						//enemies are all good
						m->rooms.push_back(r);
					}
					((room *)m->rooms[0])->discovered = 1;
					visit(m, visited, 0, 0);
					//walls are all good
					statistics * s = (statistics *) chr->max;
					s->copyTo((statistics *) chr->current);
					chr->x = 0;
					chr->y = 0;
					chr->column = 0;
					chr->row = 0;
					chr->save();
					delete visited;
					//delete mm; //this needs to be cached
					stringstream get(to_string(chr->getID()));
					mazeCmd(c, &get);
				}
			}
		} else { //show maze options
			
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
			if(chr != NULL) {
				writeInt(&res, id);
				writeInt(&res, chr->x);
				writeInt(&res, chr->y);
				writeInt(&res, chr->column);
				writeInt(&res, chr->row);
				writeInt(&res, chr->current->getID());
				writeInt(&res, chr->max->getID());
				writeInt(&res, chr->maze != NULL);
				sendStatistics(c, (statistics *)chr->current);
				sendStatistics(c, (statistics *)chr->max);
				c.sendMessage(res.str());
			}
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
				c.sendMessage("user 0");
				return;
			}
		}
		c.sendMessage("user 16 invalid password");
		return;
	}
	c.sendMessage("user 16 invalid username");
}

void moveCmd(client & c, stringstream * ss) {
	if(c.associated != NULL) {
		user * u = (user *) c.associated;
		character * chr = getCharacterById(u, nextInt(ss));
		if(chr != NULL) {
			maze * m = (maze *)chr->maze;
			if(m != NULL) {
				room * r = (room *)m->rooms[chr->row * m->columns + chr->column];
				int direction = nextInt(ss);
				if(direction & r->doors & UP) {
					chr->row--;
				} else if(direction & r->doors & RIGHT) {
					chr->column++;
				} else if(direction & r->doors & DOWN) {
					chr->row++;
				} else if(direction & r->doors & LEFT) {
					chr->column--;
				}
				if(direction & r->doors) {
					c.sendMessage("move " + to_string(chr->getID()) + " " + to_string(direction));
				}
				r = (room *)m->rooms[chr->row * m->columns + chr->column];
				r->discovered = 1;		
				chr->save();				
			}
		}
	}
}

//extra validation
//hash password
void signupCmd(client & c, stringstream * ss) {
	string username = next(ss);
	string password = next(ss);
	DIR * dir = opendir(("accounts/" + username).c_str());
	if (dir) {
		c.sendMessage("user 31 that username is already in use");
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
	c.sendMessage("user 0");
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
	} else if(!command.compare("move")) {
		moveCmd(c, &ss);
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
	roommodel * empty = new roommodel();
	mm->columns = 5;
	mm->rows = 5;
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
	for(int i = 0; i < mm->rows * mm->rows; i++) {		
		if(i) {
			mm->roommodels.push_back(rm);
		} else {
			mm->roommodels.push_back(empty);
		}
	}
	mm->save();
	delete mm;
	delete em;
	delete s;
	delete rm;
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