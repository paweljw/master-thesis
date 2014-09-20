add_executable(dbctl
    src/lib/sqlite/sqlite3.c
    src/lib/json_spirit/json_spirit_reader.cpp
    src/lib/json_spirit/json_spirit_writer.cpp
    src/lib/json_spirit/json_spirit_value.cpp
    src/utilhead.hpp 
    src/utilhead.cpp
    src/db/ServerDatabaseAccessor.hpp
    src/db/ServerSQLiteDatabaseAccessor.hpp
    src/db/ServerSQLiteDatabaseAccessor.cpp
    src/db/ServerSQLiteDBScheme.hpp 
    src/models/Solution.hpp
    src/models/Solution.cpp
    src/models/Wave.hpp
    src/models/Wave.cpp
    src/models/Task.hpp
    src/models/Task.cpp
    src/dbctl.cpp)

target_link_libraries(dbctl ${Boost_LIBRARIES})