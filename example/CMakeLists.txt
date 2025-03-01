include_directories(../libhv ../libhv/base ../libhv/ssl ../libhv/event ../libhv/util ../libhv/evpp ../libhv/cpputil ../libhv/http ../libhv/http/client ../libhv/http/server)

# logtest
add_executable(logtest logtest.c)
target_link_libraries(logtest ${HV_LIBRARIES})

# timer test
add_executable(timertest timertest.c)
target_link_libraries(timertest ${HV_LIBRARIES})

# tcp server test
add_executable(tcpservertest tcpservertest.c)
target_link_libraries(tcpservertest ${HV_LIBRARIES})

#tcp client test
add_executable(tcpclienttest tcpclienttest.c)
target_link_libraries(tcpclienttest ${HV_LIBRARIES})

# tcp server test with cpp
add_executable(tcpservertest_cpp tcpservertest.cpp)
target_link_libraries(tcpservertest_cpp ${HV_LIBRARIES})

#tcp client test with cpp
add_executable(tcpclienttest_cpp tcpclienttest.cpp)
target_link_libraries(tcpclienttest_cpp ${HV_LIBRARIES})

# websocket server test
add_executable(websocketservertest websocketservertest.cpp)
target_link_libraries(websocketservertest ${HV_LIBRARIES})

#websocket client test
add_executable(websocketclienttest websocketclienttest.cpp)
target_link_libraries(websocketclienttest ${HV_LIBRARIES})