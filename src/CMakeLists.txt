# file(GLOB SOURCES "*.cpp")

include_directories(${CMAKE_SOURCE_DIR}/inc)

include_directories(SYSTEM
		"${GLOG_INCLUDE_DIRS}"
		"${RAPIDJSON_INCLUDE_DIRS}"
	)

# Show more warnings at compiletime.
if (MSVC)
    # FIXME: put Windows specific options here.
else ()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
endif ()

# Create the executable.
add_executable(c_socket c_sockets.cpp)
target_link_libraries(c_socket
		${GLOG_LIBRARIES}
		"${RAPIDJSON_INCLUDE_DIRS}"
	)
