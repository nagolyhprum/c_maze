#include "include/datastreams.h"
#include "include/server.h"
#include "include/lib.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>

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

void characters(client & c, stringstream * ss) {
	if(c.associated != NULL) {
		string cmd = next(ss);
		user * u = (user *)c.associated;
		if(!cmd.compare("getall")) {
			for(int i = 0; i < u->characters.size(); i++) {
				character * chr = (character *)u->characters[i];
				stringstream getone("getone " + to_string(chr->getID()));
				characters(c, &getone);	
			}			
		} else if(!cmd.compare("add")) {
			character * chr = new character();
			chr->charactername = nextString(ss);
			chr->current = new statistics();
			chr->max = new statistics();
			chr->map = NULL;
			u->characters.push_back(chr);
			u->save();
			stringstream getone("getone " + to_string(chr->getID()));
			characters(c, &getone);
		} else if(!cmd.compare("getone")) {
			stringstream res;
			res << "character ";			
			int id = nextInt(ss);
			character * chr;
			for(int i = 0; i < u->characters.size(); i++) {
				character * curr = (character *)u->characters[i];
				if(curr->getID() == id) {
					chr = curr;
				}
			}
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
void signin(client & c, stringstream * ss) {
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
void signup(client & c, stringstream * ss) {		
	string username = next(ss);
	string password = next(ss);
	DIR * dir = opendir(("accounts/" + username).c_str());
	if (dir) {
		c.sendMessage("log that username is already in use");
		closedir(dir);
		return;
	}	
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
		signin(c, &ss);
	} else if(!command.compare("signup")) {	
		signup(c, &ss);
	} else if(!command.compare("characters")) {
		characters(c, &ss);
	}
}

void onClose(client & c) {	
}

void onError(client & c, exception e) {
	c.close();
}

void onConnect(client & c) {
}

int main(void) {
	datastream::register_constructors();
	server s;
	s.onMessage(onMessage);
	s.onClose(onClose);
	s.onError(onError);
	s.onConnect(onConnect);
	if(s.connect()) {
		s.start();
	}
}