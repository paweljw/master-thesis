add_executable(splitter
    src/splitter.cpp)

target_link_libraries(splitter ${Boost_LIBRARIES} ${ZLIB_LIBRARIES})
