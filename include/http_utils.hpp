#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include "sql_utils.hpp"


using namespace boost::asio;
using namespace boost::beast;
namespace handlers {
	struct http_handler{
			flat_buffer read_buffer;
			std::string write_buffer;
			http::request<http::string_body> request;
			http::response<http::string_body> response;
	};

	void handle_connection(ip::tcp::socket &socket, sql_utils::query_handler &sql_handler, boost::system::error_code &ec);

	void http_get(http_handler &http_handler, sql_utils::query_handler &sql_handler);

	void http_post(std::string_view url, http::response<http::string_body> &response, const char* body, sql_utils::query_handler &sql_handler);
}

namespace endpoints {
	void login(const char* body, http::response<http::string_body> &response, sql_utils::query_handler &sql_handler);

	void route(std::string &endpoint);

	char* open_file(std::string path, int &length);
}
