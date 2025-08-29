# ODBC
- This repository serves to teach me database interaction in C++.
---
# GOALS
- This projects main purpose was to familairze me with database interaction within C++ and deepen my understanding of fullstack development. While incorrectly named odbc, sqlite3 doesnt support odbc connection, the SQLITE API was used to prepare and execute queries.
- The project's scope evolved into -> "Create a single page application that can use terraform to manage a users resources". I quickly realized, after viewing the terraform plugin sdk, that I was using the incorrect tool for the job as Terraforms SDK only supports a select few languages. As a result, I will simply use this as an opportunity to get familiarized with the fork() or exec() system calls and end the project there.
- *Project Goals:*
    - <s>Create a basic initialization procedure for websockets and database connection!</s>
    - <s>Create basic API endpoints for GET/POST basic data!</s>
    - <s>Create Basic Homepage with working login modal!</s>
    - <s>Create Basic Authentication mechanism for fetching locked resources!</s>
    - <s>Use a frontend framework to load data based upon session keys!</s>
    - Create CI\CD Tests to deploy to github actions.
    - Fork terraform to deploy resources based upon database values.

# Compilation
- **Dependencies**:
    - boost asio
    - boost beast
    - sqlite3 
    - inja
    - argon2
    - tailwind css 
- In order to compile this project the <a href="https://www.sqlite.org/download.html">sqlite amalgamation</a> files are needed. Place them into the sqlite directory.
- It is expected by cmake that you have <a href="https://github.com/pantor/inja">inja</a> and its <a href="https://github.com/nlohmann/json/releases">dependencies</a>, nlohmann/json installed system include locations.
    - *Note: Be sure to copy the nlohmann directory, from single includes, into the system includes directory.*
- Follow the installation instructions for <a href="https://github.com/P-H-C/phc-winner-argon2">argon</a> to ensure proper compilation.
### Project Structure
```
--root
   --sqlite
      --(Put Your SQLite amalgamation files here!)
   --build
      --(CMake cache and build in this directory!)
   --include
   --src
```
### System Requirements
- GNU/LINUX Distrobution
- C++ 17 Compatible Compiler
- CMAKE version >= 3.10
