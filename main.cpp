#include <cstdlib>
#include <iostream>
#include "sqlite/sqlite3.h"

int main() {
	std::cout << "penis" << std::endl;
	sqlite3 *db;

	int status = sqlite3_open("../sqlite/database.db", &db);

	if(status != SQLITE_OK){
		std::cout << "failed to open database!" << std::endl;
		exit(status);
	}

	sqlite3_stmt *stmt;

	sqlite3_close(db);
}
