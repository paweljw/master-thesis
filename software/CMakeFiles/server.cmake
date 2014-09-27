add_executable(server
    src/lib/sqlite/sqlite3.c
	src/lib/mongoose/mongoose.h
	src/lib/mongoose/mongoose.c
    src/lib/inih/ini.c
    src/lib/inih/INIReader.cpp
    src/lib/json_spirit/json_spirit_reader.cpp
    src/lib/json_spirit/json_spirit_writer.cpp
    src/lib/json_spirit/json_spirit_value.cpp
    src/db/DatabaseAccessor.hpp
    src/db/SQLiteDatabaseAccessor.hpp
    src/db/SQLiteDatabaseAccessor.cpp
    src/net/RequestManager.hpp
    src/net/RequestManager.cpp
    src/models/Solution.hpp
    src/models/Solution.cpp
    src/models/Wave.hpp
    src/models/Wave.cpp
    src/models/Task.hpp
    src/models/Task.cpp
	src/utilhead.hpp
	src/utilhead.cpp
	src/server.cpp
	)

target_link_libraries(server ${Boost_LIBRARIES})