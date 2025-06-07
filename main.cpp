#include <cstddef>
#include <sys/stat.h>
#include <cstring>
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
		std::exit(open_status);
	}

	if(!db_exists){
		std::ifstream ifs; 
		ifs.open("../sqlite/initialize_db.sql", std::ifstream::in);
		if(!ifs.is_open()){
			std::cerr << "IO Stream Error!" << std::endl;
			std::exit(-1);
		};

		ifs.seekg(0, ifs.end);
		int length = ifs.tellg();
		ifs.seekg(0, ifs.beg);

		char *buffer = new char[length];
		ifs.read(buffer, length); //continue work here!
		ifs.close();

		if(!ifs) {
			std::cerr << "Stream read error!" << std::endl;
			std::exit(-1);
		}
		sqlite3_prepare_v2(db, buffer, -1, &stmt, NULL);
	}

	sqlite3_close(db);
}
