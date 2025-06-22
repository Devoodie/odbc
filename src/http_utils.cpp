#include <boost/beast/core/file.hpp>
#include <boost/beast/http/write.hpp>
#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/asio/read.hpp>
#include <boost/bind/bind.hpp>
#include <boost/beast/http.hpp>
#include "../include/http_utils.h"
#include "../include/terminal_colors.h"

void handlers::handle_connection(ip::tcp::socket &socket, boost::system::error_code &ec){

	handlers::http_handler handler = {};
	http::read(socket, handler.read_buffer, handler.request, ec);

	if(handler.request.method() == http::verb::get){
		handlers::http_get(handler.request.target(), handler.response);
		http::write(socket, handler.response, ec);
	}
	else if(handler.request.method() == http::verb::post){
		std::cout << yellow << "Post detected: " << handler.request.target() << std::endl;
		std::cout << handler.request.body() << clear << std::endl;
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

