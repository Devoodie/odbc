#include <iostream>
#include <fstream>
#include "../sqlite/sqlite3.h"
#include "terminal_colors.h"

namespace initialization {
	void initialize_database(sqlite3 *db, sqlite3_stmt *stmt, const char* db_path){
		int rc = 0;
		std::cout << yellow << "No SQLITE database file found!\n" << clear;
			std::ifstream ifs; 
			ifs.open("../sqlite/initialize_db.sql", std::ifstream::in);
			if(!ifs.is_open()){
				std::cerr << red << "IO Stream Error!" << clear << std::endl;
				std::exit(-1);
			};

			std::string statement;
			std::getline(ifs, statement, ';');

			while(!ifs.eof()) {
				statement.append(";");
				if(!ifs) {
					std::cerr << red << "Stream read error!" << clear << std::endl;
					std::exit(-1);
				} else {
					std::cout << blue << "Initialization statement successfully read:" << "\n" << clear << statement << "\n";
				}
				
				rc = sqlite3_prepare_v2(db, statement.c_str(), -1, &stmt, NULL); 
				if(rc != SQLITE_OK){
					std::cerr << red << "SQLITE prepare error: " << rc << "!" << "\n" << clear << "Cleaning up db file!" << std::endl;

					rc = std::remove(db_path);
					if(!rc) std::cerr << red << "DB cleanup failed, ERROR: " << rc << clear << std::endl;

					std::cout << yellow << "EXITING!\n" << clear << std::endl;
					std::exit(rc);
				} else {
					std::cout << blue << "SQL statement prepared!\n" << clear << std::endl;
				}

				rc = sqlite3_step(stmt);
				if(rc != SQLITE_DONE){
					std::cerr << red << "SQLITE COMMIT ERROR: " << rc << clear << std::endl;
					std::exit(rc);
				}
				sqlite3_finalize(stmt);
				std::getline(ifs, statement, ';');
			}
	}
}
