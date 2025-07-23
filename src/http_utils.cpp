#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/beast/core/file.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/asio.hpp>
#include <boost/asio/read.hpp>
#include <boost/bind/bind.hpp>
#include <boost/beast/http.hpp>
#include "../include/http_utils.hpp"
#include "../include/terminal_colors.h"

void handlers::handle_connection(ip::tcp::socket &socket, sql_utils::query_handler &sql_handler, boost::system::error_code &ec){

	handlers::http_handler handler = {};
	http::read(socket, handler.read_buffer, handler.request, ec);

	if(handler.request.method() == http::verb::get){
		handlers::http_get(handler, sql_handler);
	}
	else if(handler.request.method() == http::verb::post){
		//http_hanndler sql_handler
		handlers::http_post(handler, sql_handler);
	};
	http::write(socket, handler.response, ec);
	socket.close();
}

void handlers::http_get(handlers::http_handler &http_handler, sql_utils::query_handler &sql_handler){
	std::string url = http_handler.request.target();
	char *body;
	int length = 0;
	std::string path = "./frontend";
	path.append(url);

	//if else power!
	if(url == "/"){
		sql_utils::session user_session;
		bool authenticated = sql_utils::CheckSession(sql_handler, http_handler.request[http::field::cookie], user_session);
		if(authenticated){
			path.append("index.html");
		} else {
			path.append("index_locked.html");
		}
		body = endpoints::OpenFile(path);

	} else if (url == "/resources"){
		sql_utils::session user_session;
		bool authenticated = sql_utils::CheckSession(sql_handler, http_handler.request[http::field::cookie], user_session);
		if(authenticated){
			//lets think about what happens here. 
			//allocation 1, when the string is created in get resources. 
			//move 1, when the string is moved into the body_copy variable. 
			//allocation 2, when i create a new char array and assign it to body,
			//copy 1, when i use body_copy into the body variable.
			//how can i reduce this!?
			std::string body_copy = endpoints::GetResources(sql_handler, user_session.id);
			body = new char[body_copy.length()];
			body_copy.copy(body, body_copy.length());
		} else {
			path.append("index_locked.html");
			body = endpoints::OpenFile(path);
		}
	}
	else if(url == "/output.css"){
		body = endpoints::OpenFile(path);
	}
	if(url == "/output.css"){
		http_handler.response.set(http::field::content_type, "text/css");
	} else {
		http_handler.response.set(http::field::content_type, "text/html");
	}

	//check if null

	if(body == nullptr){ 
		body = new char[1];
	}

	http_handler.response.base().result(200);
	http_handler.response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
	http_handler.response.body() = body;
	http_handler.response.content_length(http_handler.response.body().length());
	delete[] body;
}

void handlers::http_post(http_handler &http_handler, sql_utils::query_handler &sql_handler){
	std::string url = http_handler.request.target();
	std::cout << blue << "Posting: " << http_handler.request.target() << clear << std::endl;
	if(url == "/login"){
		endpoints::login(http_handler.request.body().c_str(), http_handler.response, sql_handler);
	}
}

void endpoints::login(const char *body, http::response<http::string_body> &response, sql_utils::query_handler &sql_handler){
		std::string field;
		std::string value;
		bool field_or_val = 1;

		sql_handler.keys = {"user_name"};
		sql_handler.values = std::vector<std::string>(1);
		sql_handler.table = "users";
		sql_handler.columns = {"user_name", "hash", "salt", "ID"};

		std::string password;

		//consider using regular expression for this 
		for(int i = 0; i < std::strlen(body); ++i){
			if(body[i] == '=') {
				field_or_val ^= 1;
			} else if(body[i] == '&' or i == std::strlen(body) - 1) {
				if(i == std::strlen(body) - 1) value += body[i];
				if(field == "username"){
					sql_handler.values[0] = "'" + value + "'";
				} else if(field == "password"){
					password = value;
				} else {
					std::cerr << red << "NO VALID FIELD FOUND: " << field << clear << std::endl;
				}

				field = "";
				value = "";
				field_or_val ^= 1;

			} else if(field_or_val) {
				field += body[i];
			} else {
				value += body[i];
			}
		}

		sql_utils::query_db(sql_handler);
		int bad = sql_utils::GetUserSession(sql_handler, response, password);

		if(!bad){
			std::cout << blue << "Authenticated!" << clear << std::endl;
			response.body() = "";
			response.base().result(200);
			response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
			response.content_length(0);
		} else {
			fstream modal("../templates/html/modal.html");
			if(modal.is_open()){
				modal.seekg(0, modal.end);
				int length = modal.tellg();
				modal.seekg(0, modal.beg);
				char *buffer = new char[length];

				response.body() = *buffer;
				response.base().result(401);
				modal.read(buffer, length);
				response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
				response.content_length(length);

				std::cout << *buffer << std::endl;
				modal.close();
				delete[] buffer;
			} else {
				std::cerr << red << "ERROR READING TEMPLATE: modal.html!" << clear << std::endl;
				return;
			}
			std::cout << "DENIED!" << clear << std::endl;
		}
}

char* endpoints::OpenFile(std::string path){
	char* file;
	int length = 0;
	std::ifstream file_stream;
	file_stream.open(path.c_str());

	if(!file_stream.good()){
		std::cerr << red << "Unable to open file: " << path << "\n" << "STREAM STATUS: " <<  file_stream.badbit << clear << std::endl;
		return nullptr;
	} else {
		std::cout << blue << "Retrieving: " << path << clear << std::endl;
	}

	file_stream.seekg(0, file_stream.end);
	length = file_stream.tellg();
	file_stream.seekg(0, file_stream.beg);

	file = new char[length];

	file_stream.read(file, length);

	if(!file_stream.good()){ 
		std::cerr << red << "STREAM READ ERROR: " << file_stream.badbit << clear << std::endl;
		return nullptr;
	}

	file_stream.close();

	return file; 
}

//the challenge is to create a body on the heap and then return the pointer or just change the function signature. 
//I would like for the function signature to stay the same on endpoints

std::string endpoints::GetResources(sql_utils::query_handler &sql_handler, int user_id){
	std::string body;
	sql_handler.rc = sqlite3_prepare_v2(sql_handler.db, "SELECT vm_id, vm_name FROM resources\nINNER JOIN users\nON owner = ID;", -1, &sql_handler.stmt, nullptr);

	if(sql_handler.rc != SQLITE_OK){	
		std::cout << red << "SQLITE PREPARE ERROR: " << sql_handler.rc << clear << std::endl;
		return "";
	}

	sql_handler.rc = sqlite3_step(sql_handler.stmt);

	//replace this with a template
	body += "<p class=\"dark:text-white h-8 pt-1 pl-2 border-b border-sky-500\">Resources</p>";

	while(sql_handler.rc == SQLITE_ROW){
		int vm_id = sqlite3_column_int(sql_handler.stmt, 0);
		std::string vm_name = reinterpret_cast<const char *>(sqlite3_column_text(sql_handler.stmt, 1));
		body += "<p>(" + to_string(vm_id) + ")" + " " + vm_name + "</p>";
		sql_handler.rc = sqlite3_step(sql_handler.stmt);
	}

	sqlite3_finalize(sql_handler.stmt);

	return body;
}; 
