include_directories(${CMAKE_SOURCE_DIR}/inc)

include_directories(SYSTEM
		"${RAPIDJSON_INCLUDE_DIRS}"
		"${GLOG_INCLUDE_DIRS}"
		"${GTEST_INCLUDE_DIRS}"
	)

# Add executable and list sources to the executable
add_executable(test_c_socket_server test_c_socket_server.cpp ${CMAKE_SOURCE_DIR}/src/c_socket.cpp)
add_executable(test_c_socket_client test_c_socket_client.cpp ${CMAKE_SOURCE_DIR}/src/c_socket.cpp)

# Link test executable against gtest & gtest_main
target_link_libraries(test_c_socket_server
		${GTEST_BOTH_LIBRARIES}
		${GLOG_LIBRARIES}
	)

target_link_libraries(test_c_socket_client
	${GTEST_BOTH_LIBRARIES}
	${GLOG_LIBRARIES}
	)

add_test(test_c_socket_server ${EXECUTABLE_OUTPUT_PATH})
add_test(test_c_socket_client ${EXECUTABLE_OUTPUT_PATH})
