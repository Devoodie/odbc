#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>


using namespace boost::asio;
using namespace boost::beast;
namespace handlers {
	struct http_handler{
			flat_buffer read_buffer;
			std::string write_buffer;
			http::request<http::string_body> request;
			http::response<http::string_body> response;
	};

	void handle_connection(ip::tcp::socket &socket, boost::system::error_code &ec);

	void http_get(std::string_view url, http::response<http::string_body> &response);
}
