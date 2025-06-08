#include <iostream>
#include <fstream>
#include "../sqlite/sqlite3.h"
namespace initialization {
	void initialize_database(sqlite3 *db, sqlite3_stmt *stmt, const char* db_path){
		int rc = 0;
		std::cout << "No SQLITE database file found!\n";
			std::ifstream ifs; 
			ifs.open("../sqlite/initialize_db.sql", std::ifstream::in);
			if(!ifs.is_open()){
				std::cerr << "IO Stream Error!" << std::endl;
				std::exit(-1);
			};

			std::string statement;
			std::getline(ifs, statement, ';');

			while(!ifs.eof()) {
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
				std::getline(ifs, statement, ';');
			}
	}
}
