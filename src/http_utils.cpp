#include <iostream>
#include "../include/http_utils.h"
#include <boost/asio.hpp>
#include <boost/asio/read.hpp>
#include <boost/bind/bind.hpp>

void handlers::http_handler::listen(boost::system::error_code &ec){
	if(!ec){
//		this->acceptor.async_accept(this->socket, boost::bind(&handlers::http_handler::handle_http, this, ec));
		this->acceptor.accept(this->socket);
		this->handle_http(ec);
	}
}

void handlers::http_handler::handle_http(boost::system::error_code &ec){
	if(!ec){
		if(this->socket.is_open()){
			std::cout << "Socket is open!\n";
		} else {
			std::cout << "Socket is closed!\n";
		}
		std::size_t bytes_read = boost::asio::read(this->socket, boost::asio::buffer(this->read_buffer, 4096));
		std::cout << bytes_read << " bytes read!" << std::endl;
		std::cout << this->read_buffer << std::endl;
	}
}

void handlers::http_handler::read_http(boost::system::error_code &ec){
	if(!ec){
		std::cout << this->read_buffer << std::endl;
	}
}
