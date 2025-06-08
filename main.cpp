#include <sys/stat.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include "include/initialization.h"

const char* db_path = "../sqlite/database.db";

int main() {
	sqlite3 *db;
	sqlite3_stmt *stmt;
	int rc; //return code

	struct stat buffer;
	bool db_exists;
	
	db_exists = !stat(db_path, &buffer);

	int open_status = sqlite3_open(db_path, &db);

	if(open_status != SQLITE_OK){
		std::cout << "failed to open database!" << std::endl;
		std::exit(open_status);
	}

	if(!db_exists){
		initialization::initialize_database(db, stmt, db_path);
		std::cout << "Database Initialization Complete!" << std::endl;
	} else {
		std::cout << "Existing database Detected!" << std::endl;
	}

	sqlite3_close(db);
}
