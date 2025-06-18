#include <sys/stat.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio/registered_buffer.hpp>
#include <boost/asio/write.hpp>
#include <boost/beast.hpp>
#include "include/initialization.h"
#include "include/http_utils.h"
#include "sqlite/sqlite3.h"


const char* db_path = "../sqlite/database.db";

using namespace boost;
using namespace boost::asio::ip;
using namespace boost::beast;
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

	std::string insert_test = "INSERT INTO users (user_name, first_name, last_name)\nVALUES('Devooty','test','another_test');";

	rc = sqlite3_prepare_v2(db, insert_test.c_str(), -1, &stmt, NULL);

	if(rc != SQLITE_OK){
		std::cerr << red << "PREPARE FAILURE: " << rc << clear << std::endl;
		std::exit(rc);
	} else {
		std::cout << "Prepare success!\n";
		rc = sqlite3_step(stmt);
		if(rc != SQLITE_DONE) std::cout << yellow << "Insert unsuccessful: " << rc << clear << std::endl;
	}


	//initialize boost asio 

	asio::io_context io_context;
	boost::system::error_code ec;
	tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8085));

	boost::beast::flat_buffer read_buffer;
	http::request<http::string_body> request;

	try {
		tcp::socket socket(io_context);
		acceptor.accept(socket);

		http::read(socket, read_buffer, request, ec);

		auto iterator = request.begin();

		std::cout << request.method_string() << "\n";
		while(iterator != request.end()){
			std::cout << iterator->name_string() << ": " << iterator->value() << "\n";
			iterator++;
		} 
		std::cout << std::endl;

	} catch (std::exception& e) {
		std::cerr << red << e.what() << clear << std::endl;
	}

	sqlite3_close(db);
}
