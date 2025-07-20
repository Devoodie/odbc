#include "argon2.h"
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <cassert>
#include <inja.hpp>
#include "../include/sql_utils.hpp"
#include "../include/terminal_colors.h"

#define HASHLEN 32
#define SALTLEN 16 

constexpr uint32_t t_cost = 2;
constexpr uint32_t m_cost = 1<<16;
constexpr uint32_t parallelism = 1;

//prepares select query for processing
void sql_utils::query_db(sql_utils::query_handler &sql_handler){
	inja::Environment env;
//	env.set_html_autoescape(true);
	inja::json query_params;

	std::string select_columns;
	std::string where_clause = "WHERE ";

	std::string query;

	assert(sql_handler.keys.size() == sql_handler.values.size());

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

	query = env.render_file("../templates/select.sql", query_params);

	std::cout << yellow << "QUERY:\n" << query << clear << std::endl;

	sql_handler.rc = sqlite3_prepare_v2(sql_handler.db, query.c_str(), -1, &sql_handler.stmt, NULL);

	if(sql_handler.rc != SQLITE_OK){
		std::cerr << red << "SQL PREPARE ERROR: " << sql_handler.rc << clear << std::endl;
		return;
	}
}

int sql_utils::insert_db(sql_utils::query_handler &sql_handler){
	inja::Environment env;
//	env.set_html_autoescape(true);
	inja::json query_params;

	std::string insert_columns;
	std::string values;

	std::string query;

	for(int i = 0; i < sql_handler.columns.size(); ++i){
		insert_columns.append(sql_handler.columns[i]);
		if(i != sql_handler.columns.size() - 1) insert_columns.append(", ");
	}

	for(int i = 0; i < sql_handler.values.size(); ++i){	
		values.append(sql_handler.values[i]);
		if(i != sql_handler.values.size() - 1) values.append(", ");
	}

	query_params["columns"] = insert_columns;
	query_params["table"] = sql_handler.table;
	query_params["values"] = values;

	query = env.render_file("../templates/insert.sql", query_params);

	std::cout << yellow << "QUERY:\n" << query << clear << std::endl;

	sql_handler.rc = sqlite3_prepare_v2(sql_handler.db, query.c_str(), -1, &sql_handler.stmt, NULL);

	if(sql_handler.rc != SQLITE_OK){
		std::cerr << red << "SQL PREPARE ERROR: " << sql_handler.rc << clear << std::endl;
	}

	sql_handler.rc = sqlite3_step(sql_handler.stmt);

	if(sql_handler.rc != SQLITE_DONE){
		std::cerr << red << "SQL INSERTION ERROR: " << sql_handler.rc << clear << std::endl;
	}

	return sql_handler.rc;
};

int sql_utils::GetUserSession(query_handler &sql_handler, http::response<http::string_body> &response, std::string password){
	sql_utils::query_db(sql_handler);
	if(sql_handler.rc != SQLITE_OK) return sql_handler.rc;

	sql_handler.rc = sqlite3_step(sql_handler.stmt);

	switch(sql_handler.rc){
		case SQLITE_ROW:
			{
				//get the hash from the db along with the salt
				const uint8_t *db_hash = (uint8_t*)sqlite3_column_blob(sql_handler.stmt, 1);
				const uint8_t *salt_ptr = sqlite3_column_text(sql_handler.stmt, 2);
				
				uint8_t db_salt[SALTLEN];
				uint8_t calculated_hash[HASHLEN];

				std::copy(salt_ptr, salt_ptr + SALTLEN, std::begin(db_salt));

				argon2d_hash_raw(t_cost, m_cost, parallelism, password.c_str(), password.length(), db_salt, SALTLEN, calculated_hash, HASHLEN);

				std::cout << blue <<"DB_HASH: "  << db_hash <<  "\n" << "CALCULATED HASH: " << calculated_hash << clear << std::endl;

				for(int i = 0; i < HASHLEN; ++i){
					if(calculated_hash[i] != db_hash[i]){
						sqlite3_finalize(sql_handler.stmt);
						return 1;
					}
				}
			
				int user_id = sqlite3_column_int(sql_handler.stmt, 3);
				sqlite3_finalize(sql_handler.stmt);

				boost::uuids::random_generator gen;
				boost::uuids::uuid uuid = gen();

				//wonder what the performance difference will be compared to storing as blob
				std::string session = boost::uuids::to_string(uuid);

				uint32_t expiration = time(NULL);

				sql_handler.table = "sessions";

				sql_handler.keys.clear();
				sql_handler.values.clear();
				sql_handler.columns.clear();

				sql_handler.columns = {"user_id", "session_token", "expiration"};
				sql_handler.values = { std::to_string(user_id), "'" + session + "'", std::to_string(expiration)};
				sql_utils::insert_db(sql_handler);

				sqlite3_finalize(sql_handler.stmt);

				std::string cookie = "session_token=" + session + ";" + "SameSite=Strict; Max-Age=120";
				response.set(http::field::set_cookie, cookie);
				return 0;
			}
		case SQLITE_DONE:
			{
				std::cout << red << "INVALID LOGIN!" << clear << std::endl;
				return 1;
			}
		default:
			{
				std::cerr << red << "Unknown Error Code: " << sql_handler.rc << clear << std::endl;
				return 1;
			}
	}
}

// save the query!
bool sql_utils::CheckSession(query_handler &sql_handler, std::string cookie){

	sql_handler.keys.clear();
	sql_handler.values.clear();
	sql_handler.columns.clear();

	sql_handler.keys = {"session_token"};
	sql_handler.columns = {"user_id", "session_token", "expiration"};
	sql_handler.table = "sessions"; 

	bool appending = 0;
	std::string token;

	for(int i = 0; i < cookie.size(); ++i){
		if(appending) token += cookie[i];
		if(cookie[i] == '=') appending = 1;
	}

	sql_handler.values.push_back(token);

	std::cout << yellow << token << clear << std::endl;
	sql_utils::query_db(sql_handler);

	sqlite3_step(sql_handler.stmt);
	return true;
}
