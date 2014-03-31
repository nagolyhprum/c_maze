#include "include/datastreams.h"
#include <iostream>

int userA() {
	user a;
	a.fullname = "Logan Murphy";
	a.password = "Pa55word";
	a.username = "nagolyphrum";
	character * c = new character();
	statistics * current = new statistics();
	statistics * max = new statistics();
	c->current = current;
	c->max = max;
	c->charactername = "Idgaf";
	max->level = current->level = 5;
	max->strength = current->strength = 4;
	max->defense = current->defense = 3;
	max->intelligence = current->intelligence = 2;
	max->resistance = current->resistance = 1;
	a.characters.push_back(c);
	a.save();
	return a.getID();
}

void userB(int id) {
	user b(id);
	b.load();
	std::cout << b.fullname << "\n";
	std::cout << b.password << "\n";
	std::cout << b.username << "\n";
	std::cout << b.characters.size() << "\n";
	for(int i = 0; i < b.characters.size(); i++) {
		character * c = (character *) b.characters[i];
		statistics * current = (statistics *)c->current;
		statistics * max = (statistics *)c->max;
		std::cout << "\t" << c->charactername << "\n";
		std::cout << "\t" << current->level << " / " << max->level << "\n";
		std::cout << "\t" << current->strength << " / " << max->strength << "\n";
		std::cout << "\t" << current->defense << " / " << max->defense << "\n";
		std::cout << "\t" << current->intelligence << " / " << max->intelligence << "\n";
		std::cout << "\t" << current->resistance << " / " << max->resistance << "\n";
	}
}

int main(void) {
	datastream::register_constructors();
	userB(userA());
	return 0;
}