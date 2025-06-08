#include <cstddef>
#include <string>
#include <sys/stat.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include "sqlite/sqlite3.h"

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
		std::cout << "No SQLITE database file found!\n";
		std::ifstream ifs; 
		ifs.open("../sqlite/initialize_db.sql", std::ifstream::in);
		if(!ifs.is_open()){
			std::cerr << "IO Stream Error!" << std::endl;
			std::exit(-1);
		};

		while(ifs.peek() != EOF) {
			std::string statement;
			std::getline(ifs, statement, ';');
			statement.append(";");
			if(!ifs) {
				std::cerr << "Stream read error!" << std::endl;
				std::exit(-1);
			} else {
				std::cout << "Initialization statement successfully read:" << "\n" << statement << "\n";
			}
			
			rc = sqlite3_prepare_v2(db, statement.c_str(), -1, &stmt, NULL); 
			if(rc != SQLITE_OK){
				std::cerr << "SQLITE prepare error: " << rc << "!" << "\n" << "Cleaning up db file!" << std::endl;

				rc = std::remove(db_path);
				if(!rc) std::cerr << "DB cleanup failed, ERROR: " << rc << std::endl;

				std::cout << "EXITING!\n" << std::endl;
				std::exit(rc);
			} else {
				std::cout << "SQL statement prepared!" << std::endl;
			}

			rc = sqlite3_step(stmt);
			if(rc != SQLITE_DONE){
				std::cerr << "SQLITE COMMIT ERROR: " << rc << std::endl;
				std::exit(rc);
			}
			sqlite3_finalize(stmt);
		}
		std::cout << "Database Initialization Complete!" << std::endl;
	} else {
		std::cout << "Existing database Detected!" << std::endl;
	}

	sqlite3_close(db);
}
