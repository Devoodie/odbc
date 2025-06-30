#include "../include/sql_utils.hpp"

std::string sql_utils::query_db(sql_utils::query_handler sql_handler, std::string_view table, std::vector<std::string> columns, std::vector<std::string> keys, std::vector<std::string> values){

	std::string select_columns;
	std::string where_clause = "WHERE ";


	for(int i = 0; i < select_columns.size(); ++i){
		select_columns.append(columns[i]);
		if(i != select_columns.size()) select_columns.append(", ");
	}

	for(int i = 0; i < keys.size(); ++i){	
		where_clause.append(keys[i] + "=" + values[i]);
		if(i != keys.size() - 1) where_clause.append(", ");
	}
	//create the template
	
	//create the sqlite statment 
	return where_clause;
}
