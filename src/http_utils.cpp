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
		handlers::http_get(handler.request.target(), handler.response);
		http::write(socket, handler.response, ec);
	}
	else if(handler.request.method() == http::verb::post){
		std::cout << yellow << "BODY:\n" << handler.request.body().c_str() << clear << std::endl;
		handlers::http_post(handler.request.target(), handler.response, handler.request.body().c_str(), sql_handler);
	};
	socket.close();
};

void handlers::http_get(std::string_view url, http::response<http::string_body> &response){
	char *body;
	int length = 0;
	std::string path = "./frontend";
	path.append(url);

	if(url == "/"){
		path.append("index.html");
	}
	if(url == "/output.css"){
		response.set(http::field::content_type, "text/css");

	} else {
		response.set(http::field::content_type, "text/html");
	}


	std::ifstream file_stream;
	file_stream.open(path.c_str());

	if(!file_stream.good()){
		std::cerr << red << "Unable to open file: " << url << "\n" << "STREAM STATUS: " <<  file_stream.badbit << clear << std::endl;
		return;
	} else {
		std::cout << blue << "Retrieving: " << url << clear << std::endl;
	}

	file_stream.seekg(0, file_stream.end);
	length = file_stream.tellg();
	file_stream.seekg(0, file_stream.beg);

	body = new char[length];

	file_stream.read(body, length);

	if(!file_stream.good()){ 
		std::cerr << red << "STREAM READ ERROR: " << file_stream.badbit << clear << std::endl;
		return;
	}

	file_stream.close();

	//continue fillin out response
	response.base().result(200);
	response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
	response.content_length(length);
	response.body() = body;
	delete[] body; //don't forget to free memory
}

void handlers::http_post(std::string_view url, http::response<http::string_body> &response, const char* body, sql_utils::query_handler &sql_handler){
	std::cout << blue << "Posting: " << url << clear << std::endl;
	if(url == "/login"){
		endpoints::login(body, sql_handler);
	}
}

void endpoints::login(const char *body, sql_utils::query_handler &sql_handler){
		std::string field;
		std::string value;
		bool field_or_val = 1;

		sql_handler.keys = {"username", "password"};
		sql_handler.values = std::vector<std::string>(2);
		sql_handler.table = "users";
		sql_handler.columns = {"username", "password"};

		//consider using regular expression for this 
		for(int i = 0; i < std::strlen(body); ++i){
			if(body[i] == '=') {
				field_or_val ^= 1;
			} else if(body[i] == '&' or i == std::strlen(body) - 1) {

				if(field == "username"){
					sql_handler.values[0] = value;
				} else if(field == "password"){
					sql_handler.values[1] = value;
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
}
