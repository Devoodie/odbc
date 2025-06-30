#include "../include/sql_utils.hpp"
#include "../include/terminal_colors.h"

void sql_utils::query_db(sql_utils::query_handler &sql_handler){
	inja::Environment env;
	inja::json query_params;

	std::string select_columns;
	std::string where_clause = "WHERE ";

	std::string query;

	for(int i = 0; i < sql_handler.columns.size(); ++i){
		select_columns.append(sql_handler.columns[i]);
		if(i != sql_handler.columns.size() - 1) select_columns.append(", ");
	}

	for(int i = 0; i < sql_handler.keys.size(); ++i){	
		where_clause.append(sql_handler.keys[i] + "=" + sql_handler.values[i]);
		if(i != sql_handler.keys.size() - 1) where_clause.append(", ");
	}

	query_params["columns"] = select_columns;
	query_params["table"] = sql_handler.table;
	query_params["where"] = where_clause;

	query = env.render_file("../templates/query.sql", query_params);

	std::cout << yellow << "QUERY:\n" << query << clear << std::endl;

	sql_handler.rc = sqlite3_prepare_v2(sql_handler.db, query.c_str(), -1, &sql_handler.stmt, NULL);

	if(sql_handler.rc != SQLITE_OK){
		std::cerr << red << "SQL PREPARE ERROR: " << sql_handler.rc << clear << std::endl;
		return;
	}
}

void sql_utils::GetUserSession(query_handler &sql_handler){
	sql_utils::query_db(sql_handler);
	if(sql_handler.rc != SQLITE_OK) return;

	sql_handler.rc = sqlite3_step(sql_handler.stmt);

	switch(sql_handler.rc){
		case SQLITE_ROW:
			//continue work here

			std::string db_username = std::dynamic_cast<char*>(sqlite3_column_text(sql_handler.stmt, 0));
			std::string db_password = std::dynamic_cast<char*>(sqlite3_column_text(sql_handler.stmt, 1));
			break;
		default:
			std::cerr << red << "Unknown Error Code: " << sql_handler.rc << clear << std::endl;
			return;
	}
}
