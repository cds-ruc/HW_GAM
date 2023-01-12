project(mcs)

set(CMAKE_CXX_STANDARD 17)
# 如果想要生成的可执行文件拥有符号表，可以gdb调试，就直接加上这句
add_definitions("-Wall -g")

find_package(Boost REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

# 第三方库
set(MSGPACK_DIR ${PROJECT_SOURCE_DIR}/../thrid/msgpack-c/include)
set(SPDLOG_DIR ${PROJECT_SOURCE_DIR}/../thrid/spdlog/include)
set(INC ${PROJECT_SOURCE_DIR})


# 规定.h头文件路径
include_directories(${INC}
        ${MSGPACK_DIR}
        ${SPDLOG_DIR}
        /usr/local/include  /user/include SYSTEM
        )
link_directories(/usr/local/lib /usr/lib)

FILE(GLOB_RECURSE ALL_SRC *.cpp *.c *.cc)
FILE(GLOB MAIN_SRC main.cpp)
MESSAGE("MAIN SRC: " ${MAIN_SRC})
FOREACH (F ${ALL_SRC})

    IF (NOT ${F} STREQUAL ${MAIN_SRC})
        SET(LIB_SRC ${LIB_SRC} ${F})
    ENDIF()

    MESSAGE("Use " ${F})

ENDFOREACH (F)

set(LIBRARIES dl nlohmann_json::nlohmann_json)
set(BOOST boost_filesystem boost_system boost_thread)
set(ABSL absl::base absl::synchronization absl::strings absl::flat_hash_map absl::flat_hash_set absl::random_random absl::time)
set(GRPC gRPC::grpc++_reflection protobuf::libprotobuf)

# 生成可执行文件
add_executable(${PROJECT_NAME} ${ALL_SRC})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES} ${ABSL} ${BOOST} ${GRPC})

# 链接操作

#add_library(${PROJECT_NAME}_static STATIC ${LIB_SRC})