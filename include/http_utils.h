#include <boost/asio.hpp>

namespace handlers {

	class http_handler{
		public:
			boost::asio::ip::tcp::socket socket;
			boost::asio::io_context &io_context;
			boost::asio::ip::tcp::acceptor acceptor;
			std::string read_buffer;
			std::string write_buffer;

			http_handler(boost::asio::io_context &io_context_) : 
				io_context(io_context_), socket(io_context_), 
				acceptor(io_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8085)){};

			void listen(boost::system::error_code &ec);
			void handle_http(boost::system::error_code &ec);
			void read_http(boost::system::error_code &ec);
	};

}
