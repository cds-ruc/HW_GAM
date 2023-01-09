project(mcs)

set(CMAKE_CXX_STANDARD 17)
# 如果想要生成的可执行文件拥有符号表，可以gdb调试，就直接加上这句
add_definitions("-Wall -g")

#FIND_PACKAGE(Boost)
#IF (Boost_FOUND)
#    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
#    ADD_DEFINITIONS( "-DHAS_BOOST" )
#ENDIF()

# 第三方库
set(MSGPACK_DIR ${PROJECT_SOURCE_DIR}/../thrid/msgpack-c/include)
set(SPDLOG_DIR ${PROJECT_SOURCE_DIR}/../thrid/spdlog/include)
set(INC ${PROJECT_SOURCE_DIR})

# 规定.h头文件路径
include_directories(${INC}
        ${MSGPACK_DIR}
        ${SPDLOG_DIR}
        /usr/local/include SYSTEM
        )
link_directories(/usr/local/lib)

FILE(GLOB_RECURSE ALL_SRC *.cpp *.c)
FILE(GLOB MAIN_SRC main.cpp)
MESSAGE("MAIN SRC: " ${MAIN_SRC})
FOREACH (F ${ALL_SRC})

    IF (NOT ${F} STREQUAL ${MAIN_SRC})
        SET(LIB_SRC ${LIB_SRC} ${F})
    ENDIF()

    MESSAGE("Use " ${F})

ENDFOREACH (F)

set(LIBRARIES dl nlohmann_json::nlohmann_json)
set(BOOST boost_filesystem boost_system)
set(ABSL absl::base absl::synchronization absl::strings absl::random_random absl::time)

# 生成可执行文件
add_executable(${PROJECT_NAME} ${ALL_SRC})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES} ${ABSL} ${BOOST})

# 链接操作

#add_library(${PROJECT_NAME}_static STATIC ${LIB_SRC})