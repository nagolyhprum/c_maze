#include <iostream>
#include <string>
#include <vector>

using namespace std;

typedef struct {
	string type;
	string name;
} field;

typedef struct {
	string name;
	vector<field> fields;
} clazz;

void freeit(field f) {
	if(f.type.find("vector") != string::npos) {
		cout << "\t\tfor(int i = 0; i < " << f.name << ".size(); i++) {\n";
		cout << "\t\t\tdelete " << f.name << "[i];\n";
		cout << "\t\t}\n";
	} else if(f.type.find("serializable") != string::npos) {
		cout << "\t\tdelete " << f.name << ";\n";
	}
}

string defaultValue(string type) {
	if(type.find("vector") != string::npos) {
		return " = std::vector<serializable *>();";
	} else if(type.find("serializable") != string::npos) {
		return " = NULL;";
	} else if(type.find("string") != string::npos) {
		return " = \"\";";
	} else if(type.find("int") != string::npos) {
		return " = 0;";
	}	
	return ";";
}

int main(void) {
	vector<clazz *> clazzez;
	clazz * c;
	bool serialize = false;
	while(!cin.eof()) {
		string line;
		getline(cin, line);
		if(serialize) {
			if(line.find("#end") == 0) {
				serialize = false;
			} else {
				field f;
				int name = line.find_last_of(" ");
				f.name = line.substr(name + 1);
				f.type = line.substr(0, name);
				c->fields.push_back(f);
			}
		} else {
			if(line.find("#serialize") == 0) {
				clazzez.push_back(c = new clazz());
				c->name = line.substr(line.find(" ") + 1);
				serialize = true;
			} else if(line.length()) {
				cout << line << "\n";
			}
		}
	}
	for(int i = 0; i < clazzez.size(); i++) {
		c = clazzez[i];
		cout << "class " << c->name << " : public serializable {\n";
		cout << "public:\n";
		//fields
		for(int j = 0; j < c->fields.size(); j++) {
			field f = c->fields[j];
			cout << f.type << " " << f.name << ";\n";
		}
		//constructor(int)
		cout << "\t" << c->name << "(int id) : serializable(\"" << c->name << "\", id) {\n";
		for(int j = 0; j < c->fields.size(); j++) {
			field f = c->fields[j];
			cout << "\t\t" << f.name << defaultValue(f.type) << "\n";
		}
		cout << "\t};\n";
		//constructor()
		cout << "\t" << c->name << "() : serializable(\"" << c->name << "\") {\n";
		for(int j = 0; j < c->fields.size(); j++) {
			field f = c->fields[j];
			cout << "\t\t" << f.name << defaultValue(f.type) << "\n";
		}
		cout << "\t};\n";
		//serialize
		cout << "\tvoid serialize(datastream ds) {\n";
		for(int j = 0; j < c->fields.size(); j++) {
			field f = c->fields[j];
			cout << "\t\tds & " << f.name << ";\n";
		}
		cout << "\t};\n";
		//deconstructor
		cout << "\t~" << c->name << "() {\n";
		for(int j = 0; j < c->fields.size(); j++) {
			field f = c->fields[j];
			freeit(f);
		}
		cout << "\t};\n";
		//getter
		cout << "\tstatic serializable * get_" << c->name << "(int id) {\n";
		cout << "\t\treturn new " << c->name << "(id);\n";
		cout << "\t};\n";
		cout << "};\n";
	}	
	
	cout << "void datastream::register_constructors() {\n";
	for(int i = 0; i < clazzez.size(); i++) {
		c = clazzez[i];		
		cout << "\tregister_constructor(" << c->name << "::get_" << c->name << ", \"" << c->name << "\");\n"; 
	}
	cout << "}";
}