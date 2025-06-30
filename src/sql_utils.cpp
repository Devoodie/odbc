#include "../include/sql_utils.hpp"
#include "../include/terminal_colors.h"

std::string sql_utils::query_db(sql_utils::query_handler sql_handler){
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

	std::cout << yellow << query << clear << std::endl;

	return query;
}
