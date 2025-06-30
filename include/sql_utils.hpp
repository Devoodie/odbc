#include <vector>
#include <memory>
#include <string_view>
#include <boost/uuid/uuid.hpp>
#include <inja.hpp>
#include "../sqlite/sqlite3.h"

using namespace std;
namespace sql_utils{
	struct query_handler{
		sqlite3 *db;
		sqlite3_stmt *stmt;
		int sqlite_rc;
		string table;
		vector<string> keys;
		vector<string> values;
		vector<string> columns;
	};

	void GetUserSession(query_handler &sql_handler);

	std::string query_db(query_handler sql_handler); 
}
