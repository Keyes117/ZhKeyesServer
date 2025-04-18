cmake_minimum_required(VERSION 3.00)

project (Calc24_Server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall -O0 -Wno-unused-variable -pthread")

include_directories(
    utils/
	net/
    appsrc/
    common/
)


set(net_srcs 
    net/Acceptor.cpp
    net/ByteBuffer.cpp
    net/Epoll.cpp
    net/EventLoop.cpp
    net/Poll.cpp
    net/Select.cpp
    net/TCPConnection.cpp
    net/TCPServer.cpp
    net/ThreadPool.cpp
    net/WakeUpEventDispatcher.cpp
    net/Timer.cpp
)

set(util_srcs
utils/util.cpp
)


set(app_srcs
    appsrc/Calc24Server.cpp
    appsrc/Calc24Session.cpp
)


add_executable(Calc24_Server
    ${net_srcs} 
    ${util_srcs} 
    ${app_srcs}
    main.cpp
)
#�������Ŀ¼��û�õģ�������ʹ��TARGET_LINK_LIBRARIES���Ӹÿ�
TARGET_LINK_LIBRARIES(Calc24_Server)