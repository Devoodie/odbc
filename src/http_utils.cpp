#include <boost/beast/core/file.hpp>
#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/asio/read.hpp>
#include <boost/bind/bind.hpp>
#include <boost/beast/http.hpp>
#include "../include/http_utils.h"
#include "../include/terminal_colors.h"

void handle_connection(ip::tcp::socket &socket, boost::system::error_code &ec){

	handlers::http_handler handler = {};
	http::read(socket, handler.read_buffer, handler.request, ec);

	if(handler.request.method_string() == "GET"){}
	else if(handler.request.method_string() == "POST"){};
};

void http_get(std::string_view url, http::response<http::string_body> &response){
	char *body;
	std::string path = ".";
	path.append(url);
	int length = 0;

	std::ifstream file_stream;
	file_stream.open(path.c_str());

	if(!file_stream.good()){
		std::cerr << red << "Unable to open file: " << url << "\n" << "STREAM STATUS: " <<  file_stream.badbit << clear << std::endl;
		return;
	} else {
		std::cout << blue << "Retrieving: " << url << std::endl;
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
	//continue fillin out response
	response.base().result(200);
	response.body() = body;

	file_stream.close();

}

