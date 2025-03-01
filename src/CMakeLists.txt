# include dirs
include_directories(../libhv ../libhv/base ../libhv/ssl ../libhv/event ../libhv/util ../libhv/evpp ../libhv/cpputil ../libhv/http ../libhv/http/client ../libhv/http/server ./ ../json)

# gateserver
add_executable(GateServer GateServer.hpp gateserver/main.cpp)
target_link_libraries(GateServer ${HV_LIBRARIES})
target_link_libraries(GateServer ${JSON_LIBRARIES})

# linkserver
add_executable(LinkServer LinkServer.hpp linkserver/main.cpp)
target_link_libraries(LinkServer ${HV_LIBRARIES})
target_link_libraries(LinkServer ${JSON_LIBRARIES})