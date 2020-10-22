#include <gtest/gtest.h>
#include <glog/logging.h>
#include <string>

#include "c_socket.hpp"

const std::string host = "localhost";
const int port = 8888;

TEST(CSocketClient, OpenSocket)
{
	c_socket_client socket;
	EXPECT_EQ(socket.connect(host, port), 1);
}

TEST(CSocketClient, TryDifferentSocketHost)
{
	c_socket_client socket;
	EXPECT_LT(socket.connect("Arrancar", port), 1);
}

TEST(CSocketClient, TryDifferentSocketPort)
{
	c_socket_client socket;
	EXPECT_LT(socket.connect(host, 1818), 1);
}

TEST(CSocketClient, SendInfo)
{
	c_socket_client socket;
	EXPECT_EQ(socket.connect(host, port), 1);
	EXPECT_EQ(socket.send_info(), 1);
}

int main(int argc, char *argv[])
{
	FLAGS_logtostderr = 1;
	FLAGS_colorlogtostderr = 1;
	::google::InitGoogleLogging(argv[0]);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
