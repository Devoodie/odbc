# ODBC
- This repository serves to teach me database interaction in C++.
---
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
### file structure
```
--root
   --sqlite
      --(Put Your SQLite amalgamation files here!)
   --build
      --(CMake cache and build in this directory!)
   --include
   --src
```
