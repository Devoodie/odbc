#include <boost/uuid/uuid.hpp>
#include <string_view>
#include <vector>
#include <inja.hpp>
#include "../sqlite/sqlite3.h"

namespace sql_utils{
	struct query_handler{
		sqlite3 *db;
		sqlite3_stmt *stmt;
		int sqlite_err;
		std::string table;
		std::string* columns;
	};

	boost::uuids::uuid GetUserSession(query_handler sql_handler, std::string_view user, std::string_view password);

	std::string query_db(query_handler sql_handler, std::vector<std::string> keys, std::vector<std::string> values); 
}
