#include <sys/stat.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio/registered_buffer.hpp>
#include <boost/asio/write.hpp>
#include <boost/beast.hpp>
#include "include/initialization.h"
#include "include/http_utils.hpp"
#include "sqlite/sqlite3.h"


const char* db_path = "../sqlite/database.db";

using namespace boost;
using namespace boost::asio::ip;
using namespace boost::beast;
int main() {
	//initialize sqlite db

	sql_utils::query_handler sql_handler = {};

	struct stat buffer;
	bool db_exists;
	
	db_exists = !stat(db_path, &buffer);

	int open_status = sqlite3_open(db_path, &sql_handler.db);

	if(open_status != SQLITE_OK){
		std::cout << "failed to open database!" << std::endl;
		std::exit(open_status);
	}

	if(!db_exists){
		initialization::initialize_database(sql_handler.db, sql_handler.stmt, db_path);
		std::cout << blue << "Database Initialization Complete!" << clear << std::endl;
	} else {
		std::cout << blue << "Existing database Detected!" << clear << std::endl;
	}

	//initialize boost asio 

	asio::io_context io_context;
	boost::system::error_code ec;
	tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8085));

	try {
		tcp::socket socket(io_context);
		while(true){
			acceptor.accept(socket);
			handlers::handle_connection(socket, sql_handler, ec);
		}

	} catch (std::exception& e) {
		std::cerr << red << e.what() << clear << std::endl;
	}

	sqlite3_close(sql_handler.db);
}
