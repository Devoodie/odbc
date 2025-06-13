#include <sys/stat.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include "include/initialization.h"
#include "include/http_utils.h"


const char* db_path = "../sqlite/database.db";

using namespace boost;
using namespace boost::asio::ip;
int main() {
	//initialize sqlite db
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
		std::cout << blue << "Database Initialization Complete!" << clear << std::endl;
	} else {
		std::cout << blue << "Existing database Detected!" << clear << std::endl;
	}

	//initialize boost asio 

	asio::io_context io_context;
	boost::system::error_code ec;
	handlers::http_handler handler(io_context);


	try {
		handler.listen(ec);
		io_context.run();
	} catch(std::exception err) {
		std::cout << err.what() << std::endl;
	}

	sqlite3_close(db);
}
