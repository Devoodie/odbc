CREATE TABLE users(
	ID integer PRIMARY KEY AUTOINCREMENT,
	user_name varchar,
	first_name varchar,
	last_name varchar,
	hash blob,
	salt varchar,
	UNIQUE(user_name)
);
CREATE TABLE resources(
	vm_id INTEGER,
	vm_name varchar,
	owner INTEGER, 
	minecraft_version varchar,
	forge_version varchar,
	FOREIGN KEY (owner) REFERENCES users(ID)
);
CREATE TABLE sessions(
	user_id integer,
	session_token varchar,
	expiration unsigned integer
);
