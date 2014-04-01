#include "include/datastreams.h"
#include <iostream>
#include <sstream>
#include "include/server.h"
#include <fstream>
#include <sys/types.h>
#include <dirent.h>

//hash password
void signin(client c, stringstream * ss) {
	string username;
	string password;
	*ss >> username;
	*ss >> password;
	int id;
	string s;
	fstream in(("accounts/" + username + "/password.data").c_str(), fstream::in);
	if(in.good()) {
		in >> s;
		in.close();
		if(!s.compare(password)) {
			in.open(("accounts/" + username + "/id.data").c_str(), fstream::in);
			if(in.good()) {
				in >> id;	
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
void signup(client c, stringstream * ss) {		
	string username;
	string password;
	*ss >> username;
	*ss >> password;
	DIR * dir = opendir(("accounts/" + username).c_str());
	if (dir) {
		c.sendMessage("log that username is already in use");
		closedir(dir);
		return;
	}	
	int length;
	*ss >> length;
	char * fn = new char[length];
	ss->get();
	ss->read(fn, length);
	string fullname(fn, length);
	user * u = new user();
	u->fullname = fullname;
	u->username = username;
	u->password = password;
	u->save();	
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

void onMessage(client c, string message) {
	c.sendMessage("echo " + message);
	stringstream * ss = new stringstream(message);
	string command;
	*ss >> command;
	if(!command.compare("signin")) {
		signin(c, ss);
	} else if(!command.compare("signup")) {	
		signup(c, ss);
	}
	delete ss;
}

void onClose(client c) {
}

void onError(client c, exception e) {
	c.close();
}

void onConnect(client c) {
}

int main(void) {
	server s;
	s.onMessage(onMessage);
	s.onClose(onClose);
	s.onError(onError);
	s.onConnect(onConnect);
	if(s.connect()) {
		s.start();
	}
}