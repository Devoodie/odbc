#include <cstdlib>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include "sqlite/sqlite3.h"

int main() {
	sqlite3 *db;
	sqlite3_stmt *stmt;

	struct stat buffer;
	bool db_exists;
	
	db_exists = !stat("../sqlite/database.db", &buffer);

	int open_status = sqlite3_open("../sqlite/database.db", &db);

	if(open_status != SQLITE_OK){
		std::cout << "failed to open database!" << std::endl;
		exit(open_status);
	}

	if(!db_exists){
		std::ifstream ifs; //work here
		ifs.open("../sqlite/initialize_db.sql", std::ifstream::in);
		if(ifs.bad()){
			std::cerr << "IO Stream Error!" << std::endl;
			exit();
		};
	}

	sqlite3_close(db);
}
