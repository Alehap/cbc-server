
project(cbc)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_VERBOSE_MAKEFILE 0)

# set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Options are: Debug Release RelWithDebInfo."
        FORCE)
endif()

set(CMAKE_C_FLAGS_RELEASE "-O2 -std=gnu99 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

add_definitions(-Wall -pedantic -Werror -Wfatal-errors -Wno-unused-result)

include(FindLua51)
if (LUA51_FOUND)
    include_directories(${LUA_INCLUDE_DIR})
else ()
    message(FATAL_ERROR "cannot found lua 5.1")
endif ()

#add_library(static_tools ltools.c)
#target_link_libraries(static_tools ${LUA_LIBRARIES})

add_executable(cbcdaemon cbcdaemon.c ltools.c)
target_link_libraries(cbcdaemon ${LUA_LIBRARIES} dl)
set_target_properties(cbcdaemon PROPERTIES LINK_FLAGS -Wl,-E)

#install(TARGETS lua DESTINATION bin)
#install(TARGETS tools DESTINATION lib/lua/5.1)

