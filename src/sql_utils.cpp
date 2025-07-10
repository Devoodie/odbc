#include "../include/sql_utils.hpp"
#include "../include/terminal_colors.h"
#include "argon2.h"
#define HASHLEN 32
#define SALTLEN 16 

constexpr uint32_t t_cost = 2;
constexpr uint32_t m_cost = 1<<16;
constexpr uint32_t parallelism = 1;

//prepares query for processing
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
		if(i != sql_handler.keys.size() - 1) where_clause.append(" AND ");
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

int sql_utils::GetUserSession(query_handler &sql_handler, std::string password){
	sql_utils::query_db(sql_handler);
	if(sql_handler.rc != SQLITE_OK) return sql_handler.rc;

	sql_handler.rc = sqlite3_step(sql_handler.stmt);

	switch(sql_handler.rc){
		case SQLITE_ROW:
			{
				//continue work here
	
				//get the has from the db along with the salt
				const uint8_t *db_hash = sqlite3_column_text(sql_handler.stmt, 1);
				const uint8_t *salt_ptr = sqlite3_column_text(sql_handler.stmt, 2);
				
				uint8_t db_salt[SALTLEN];
				uint8_t calculated_hash[HASHLEN];

				std::copy(salt_ptr, salt_ptr + SALTLEN, std::begin(db_salt));

				argon2d_hash_raw(t_cost, m_cost, parallelism, password.c_str(), password.length(), db_salt, SALTLEN, calculated_hash, HASHLEN);

				return 0;
			}
		case SQLITE_DONE:
			{
				std::cout << red << "INVALID LOGIN!" << clear << std::endl;
				return 3;
			}
		default:
			{
				std::cerr << red << "Unknown Error Code: " << sql_handler.rc << clear << std::endl;
				return 1;
			}
	}
}
