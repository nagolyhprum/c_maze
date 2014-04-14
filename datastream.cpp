#include "include/datastream.h"
#include "include/lib.h"

void datastream::init(std::string name, int id, bool isRead) {	
	file = new std::fstream(("data/" + name + "/" + to_string(id) + ".data").c_str(), (this->isRead = isRead) ? std::fstream::in : std::fstream::out);
};

datastream::datastream(std::string name, int id, bool isRead) {
	init(name, id, isRead);
};

datastream::datastream(std::string name) {
	init(name, get_id(name), false);
};

datastream::datastream(datastream & ds) {	
	name = ds.name;
	id = ds.id;
	isRead = ds.isRead;
	file = ds.file;
};

void datastream::close() {
	file->close();
	delete file;
};

void datastream::operator &(std::string & s) {	
	if(isRead) {	
		int size;
		*file >> size;	
		char * data = new char[size];	
		file->get();
		file->read(data, size);
		s = std::string(data, size);	
		delete data;
	} else {
		*file << s.length() << " " << s;	
	} 
};

void datastream::operator &(int & i) {	
	if(isRead) {	
		*file >> i;		
	} else {
		*file << i << " ";	
	}
};

void datastream::operator &(serializable * & s) {
	if(isRead) {	
		int id;
		std::string name;
		*file >> id;
		//this is where caching should be handled
		if(id) {
			*file >> name;
			for(int i = 0; i < datastream::constructor_names->size(); i++) {
				if(!(*constructor_names)[i].compare(name)) {
					s = (*constructor_functions)[i](id);
				}
			}		
			s->load();
		} else {
			s = NULL;
		}
	} else {
		if(s != NULL) {
			*file << s->getID() << " " << s->getName() << " ";
			s->save();
		} else {
			*file << "0 ";
		}
	}
};

void datastream::operator &(std::vector<serializable *> & ss) {
	if(isRead) {			
		int id, size;
		std::string name;
		*file >> size;
		//this is where caching should be handled
		for(; size > 0; size--) {
			*file >> id;
			*file >> name;
			serializable * s;
			for(int i = 0; i < datastream::constructor_names->size(); i++) {
				if(!(*constructor_names)[i].compare(name)) {
					s = (*constructor_functions)[i](id);
				}
			}
			s->load();
			ss.push_back(s);
		}
	} else {
		//there should not be nulls here
		*file << ss.size() << " ";
		for(int i = 0; i < ss.size(); i++) {			
			*file << ss[i]->getID() << " " << ss[i]->getName() << " ";
			ss[i]->save();
		}
	}
};

std::vector<serializable * (*)(int)> * datastream::constructor_functions = new std::vector<serializable * (*)(int)>();
std::vector<std::string> * datastream::constructor_names = new std::vector<std::string>();

void datastream::register_constructor(serializable * (*f)(int), std::string name) {
	datastream::constructor_functions->push_back(f);
	datastream::constructor_names->push_back(name);
}