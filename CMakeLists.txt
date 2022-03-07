cmake_minimum_required(VERSION 3.17)
project(RocksDbViewer)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories($ENV{FLOW_ROOT})

set(FLUID_SRC
        fluid/MainGui.cxx
        fluid/MainGui.h
        )
if (MSVC)
    add_executable(RocksDbViewer WIN32 main.cpp ${FLUID_SRC})
else ()
    add_executable(RocksDbViewer main.cpp ${FLUID_SRC} Style.h)
endif ()
file(COPY icon.png DESTINATION ${CMAKE_BINARY_DIR})

find_package(PNG)
include_directories(${PNG_INCLUDE_DIRS})
target_link_libraries(RocksDbViewer ${PNG_LIBRARIES})

find_package(FLTK)
include_directories(${FLTK_INCLUDE_DIR})
target_link_libraries(RocksDbViewer ${FLTK_LIBRARIES})

find_package(CURL)
include_directories(${CURL_INCLUDE_DIRS})
target_link_libraries(RocksDbViewer ${CURL_LIBRARIES})

if (MSVC)
    target_link_libraries(RocksDbViewer rpcrt4.lib)
    target_link_libraries(RocksDbViewer shlwapi.lib)
    find_package(RocksDB CONFIG REQUIRED)
    target_link_libraries(RocksDbViewer RocksDB::rocksdb)
else ()
    target_link_libraries(RocksDbViewer rocksdb)
    target_link_libraries(RocksDbViewer snappy)
    target_link_libraries(RocksDbViewer lz4)
    target_link_libraries(RocksDbViewer bz2)
    target_link_libraries(RocksDbViewer z)
    target_link_libraries(RocksDbViewer zstd)
endif ()

if (UNIX)
    include(GNUInstallDirs)

    install(TARGETS RocksDbViewer
            RUNTIME DESTINATION bin
            )
endif ()