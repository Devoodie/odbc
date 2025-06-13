#include <iostream>
#include "../include/http_utils.h"
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

void handlers::http_handler::listen(boost::system::error_code &ec){
	if(!ec){
		this->acceptor.async_accept(this->socket, boost::bind(&handlers::http_handler::handle_http, this, ec));
	}
}

void handlers::http_handler::handle_http(boost::system::error_code &ec){
	if(!ec){
		boost::asio::async_read(this->socket, boost::asio::buffer(this->read_buffer), boost::bind(&handlers::http_handler::read_http, this, ec));
	}
	
}

void handlers::http_handler::read_http(boost::system::error_code &ec){
	if(!ec){
		std::cout << this->read_buffer << std::endl;
	}
}
