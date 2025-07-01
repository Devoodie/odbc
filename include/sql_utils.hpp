#include <vector>
#include <iostream>
#include <boost/uuid/uuid.hpp>
#include <inja.hpp>
#include "../sqlite/sqlite3.h"

using namespace std;
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

	int GetUserSession(query_handler &sql_handler);

	//prepares query
	void query_db(query_handler &sql_handler); 
}
