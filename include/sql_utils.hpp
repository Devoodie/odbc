#include <vector>
#include <iostream>
#include <boost/beast.hpp>
#include "../sqlite/sqlite3.h"

using namespace std;
using namespace boost::beast;

namespace sql_utils{
	struct query_handler {
		sqlite3 *db;
		sqlite3_stmt *stmt;
		int rc;
		string table;
		vector<string> keys;
		vector<string> values;
		vector<string> columns;
	};

	struct session {
		int id;
		string username;
		uint32_t expiration;
	};

	struct resources{
		int vm_id;
		string vm_name;
	};

	int GetUserSession(query_handler &sql_handler, http::response<http::string_body> &response, string password);

	bool CheckSession(query_handler &sql_handler, std::string cookie, session &user_session);

	//prepares query
	void query_db(query_handler &sql_handler); 

	int insert_db(query_handler &sql_handler);
	
}
