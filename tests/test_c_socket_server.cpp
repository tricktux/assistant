#include <gtest/gtest.h>
#include <glog/logging.h>

#include "c_socket.hpp"

const int port = 8888;

TEST(CSocketServer, OpenSocket)
{
	c_socket_server socket;
	EXPECT_EQ(socket.connect(port), 1);

	// Socket already connected
	EXPECT_EQ(socket.connect(-1), 0);
}

TEST(CSocketServer, BadSocketPort)
{
	c_socket_server socket;
	EXPECT_EQ(socket.connect(-1), -1);
}

TEST(CSocketServer, NeedToCallConnectFirst)
{
	c_socket_server socket;
	// Should print error and return immediately
	EXPECT_LT(socket.wait_for_incoming_connection(), 1);
}

TEST(CSocketServer, WaitForConnection)
{
	c_socket_server socket;
	EXPECT_EQ(socket.connect(port), 1);

	// Should sit here and wait for a connection
	EXPECT_EQ(socket.wait_for_incoming_connection(), 1);
}

TEST(CSocketServer, GetInfo)
{
	c_socket_server socket;
	EXPECT_EQ(socket.connect(port), 1);

	// Should sit here and wait for a connection
	EXPECT_EQ(socket.wait_for_incoming_connection(), 1);

	EXPECT_EQ(socket.recv_info(), 1);
}

int main(int argc, char *argv[])
{
	FLAGS_logtostderr = 1;
	FLAGS_colorlogtostderr = 1;
	::google::InitGoogleLogging(argv[0]);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
