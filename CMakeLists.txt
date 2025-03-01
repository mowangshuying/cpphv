cmake_minimum_required(VERSION 3.6)
project(cpphv VERSION 0.0.0)

# set dirs;
set(INCDIR include)
set(SRCDIR src)
set(LIBDIR lib)
set(BINDIR bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${LIBDIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${BINDIR})

set(BUILD_EXAMPLES OFF)
add_subdirectory(libhv)
add_subdirectory(json)
add_subdirectory(example)
add_subdirectory(src)
