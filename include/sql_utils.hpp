#include <vector>
#include <iostream>
#include "../sqlite/sqlite3.h"
#include <boost/beast.hpp>

using namespace std;
using namespace boost::beast;

namespace sql_utils{
	struct query_handler{
		sqlite3 *db;
		sqlite3_stmt *stmt;
		int rc;
		string table;
		vector<string> keys;
		vector<string> values;
		vector<string> columns;
	};

	int GetUserSession(query_handler &sql_handler, http::response<http::string_body> &response, string password);

	//prepares query
	void query_db(query_handler &sql_handler); 

	int insert_db(query_handler &sql_handler);
}
