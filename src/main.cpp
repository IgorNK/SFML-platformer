#include <iostream>
#include "game_engine.h"

int main(int argc, char* argv[]) {
	try {
		GameEngine ge("user_config.txt", "resources/config.txt");
		ge.run(); 
		return 0;
	}
	catch(std::exception &e) {
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}
	catch(...) {
		std::cerr << "Runtime error" << std::endl;
		return 2;
	}
}