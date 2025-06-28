#include <boost/uuid/uuid.hpp>
#include <string_view>
namespace sql_utils{
	boost::uuids::uuid GetUserSession(std::string_view user, std::string_view password);
}
