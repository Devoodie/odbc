CREATE TABLE users(
	ID INTEGER PRIMARY KEY AUTOINCREMENT,
	user_name varchar,
	first_name varchar,
	last_name varchar,
	hash varchar
);
CREATE TABLE resources(
	vm_id INTEGER,
	vm_name varchar,
	owner INTEGER, 
	minecraft_version varchar,
	forge_version varchar,
	FOREIGN KEY (owner) REFERENCES users(ID)
);
