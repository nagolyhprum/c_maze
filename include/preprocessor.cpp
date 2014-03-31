#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(void) {
	vector<string> classNames;
	string line;
	bool isSerialize = false;
	string serialize;
	string className;
	while(!cin.eof()) {
		getline(cin, line);
		if(line.find("#end") == 0 && isSerialize) {
			isSerialize = false;
			std::cout << "\tvoid serialize(datastream ds) {\n";
			std::cout << serialize;
			serialize = "";
			std::cout << "\t};\n";
			std::cout << "\t" << className << "(int id) : serializable(\"" << className << "\", id) {};\n";
			std::cout << "\t" << className << "() : serializable(\"" << className << "\") {};\n";
			std::cout << "};\n";
			std::cout << "serializable * get_" << className << "(int id) {\n";
			std::cout << "\t" << className << " * s = new " << className << "(id);\n"; 
			std::cout << "\ts->load();\n";
			std::cout << "\treturn s;\n";
			std::cout << "}\n";
		} else if(isSerialize) {
			cout << line << "\n";
			serialize += "\t\tds &" + line.substr(line.find_last_of(" ")) + "\n";
		} else if(line.find("#serialize") == 0) {
			int space = line.find(" ");
			className = line.substr(space + 1);
			classNames.push_back(className);
			isSerialize = true;
			std::cout << "class " << className << " : public serializable {\n";
			std::cout << "public:\n";
		} else {
			cout << line << "\n";
		}
	}
	cout << "void datastream::register_constructors() {\n";
	for(int i = 0; i < classNames.size(); i++) {
		cout << "\tdatastream::register_constructor(get_" + classNames[i] + ", \"" + classNames[i] + "\");\n";
	}
	cout << "}";
	return 0;
}