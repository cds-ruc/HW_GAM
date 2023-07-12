project(mcs)

set(CMAKE_CXX_STANDARD 17)
# 如果想要生成的可执行文件拥有符号表，可以gdb调试，就直接加上这句
add_definitions("-Wall -g")

find_package(Boost REQUIRED)
find_package(Gflags REQUIRED)



# 第三方库
set(MSGPACK_DIR ${PROJECT_SOURCE_DIR}/../thrid/msgpack-c/include)
set(SPDLOG_DIR ${PROJECT_SOURCE_DIR}/../thrid/spdlog/include)
set(INC ${PROJECT_SOURCE_DIR})
set(GAM ${PROJECT_SOURCE_DIR}/../gam/include  ${PROJECT_SOURCE_DIR}/../gam/lib/libcuckoo/cityhash-1.1.1/src ${PROJECT_SOURCE_DIR}/../gam/lib/libcuckoo/src)

include(FetchContent)
set(FETCHCONTENT_QUIET false)
FetchContent_Declare(
        photon
        GIT_REPOSITORY https://gitee.com/mirrors/photonlibos.git
        GIT_TAG v0.6.1
)
set(ENABLE_URING ON CACHE INTERNAL "Enable iouring")
set(PHOTON_INCLUDE_DIR ${photon_SOURCE_DIR}/include/)
FetchContent_MakeAvailable(photon)




# 规定.h头文件路径
include_directories(${INC}
        ${MSGPACK_DIR}
        ${SPDLOG_DIR}
        ${CEX_DIR}
        ${Boost_INCLUDE_DIR}
        ${PHOTON_INCLUDE_DIR}
        ${GAM}
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
set(BOOST boost_filesystem boost_system boost_thread pthread)
set(ABSL absl)
set(Photon photon_static gflags)

set(GAM ${PROJECT_SOURCE_DIR}/../thrid/libgalloc.a ${PROJECT_SOURCE_DIR}/../gam/lib/libcuckoo/cityhash-1.1.1/src/.libs/libcityhash.a ibverbs)

# 生成可执行文件
add_executable(${PROJECT_NAME} ${ALL_SRC})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES} ${ABSL} ${BOOST} ${GRPC} ${Photon} ${GAM})

# 链接操作

#add_library(${PROJECT_NAME}_static STATIC ${LIB_SRC})