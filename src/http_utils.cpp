#include <iostream>
#include <fstream>
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
		handlers::http_post(handler.request.target(), handler.response, handler.request.body().c_str(), sql_handler);
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

	if(url == "/"){

		bool authenticated = sql_utils::CheckSession(sql_handler, http_handler.request[http::field::cookie]);
		if(authenticated){
			path.append("index.html");
		} else {
			path.append("index_locked.html");
		}
		body = endpoints::open_file(path, length);

	} else if(url == "/output.css"){
		body = endpoints::open_file(path, length);
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
	http_handler.response.content_length(length);
	http_handler.response.body() = body;
	delete[] body;
}

void handlers::http_post(std::string_view url, http::response<http::string_body> &response, const char* body, sql_utils::query_handler &sql_handler){
	std::cout << blue << "Posting: " << url << clear << std::endl;
	if(url == "/login"){
		endpoints::login(body, response, sql_handler);
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
			fstream modal("../templates/modal.html");
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

//finish routing
void endpoints::route(std::string &endpoint){
	if(false);
}

char* endpoints::open_file(std::string path, int &length){
	char* file;
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
