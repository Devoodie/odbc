#include <boost/asio/registered_buffer.hpp>
#include <boost/asio/write.hpp>
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

	try {
	asio::io_context io_context;
	boost::system::error_code ec;
//	handlers::http_handler handler(io_context);
	tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8085));

	std::string read_buffer;

	tcp::socket socket(io_context);
	acceptor.accept(socket);

	std::string write_buffer = "HTTP/1.1 200 OK\nContent\nContent-Type: text/html; charset=UTF-8\nConnection: Closed\n\n<html><h1>Hello World! </h1></html>\n";

	asio::write(socket, boost::asio::buffer(write_buffer), ec);

	int bytes_read = asio::read(socket, asio::buffer(read_buffer));
	std::cout << bytes_read << " bytes read!\n" << read_buffer << std::endl;
	std::cout << read_buffer << std::endl;

	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}


	sqlite3_close(db);
}
