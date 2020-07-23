cmake_minimum_required(VERSION 3.13)
project (win32uv C)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

include_directories(deps/libuv/include)
add_subdirectory(deps/libuv EXCLUDE_FROM_ALL)
set(LIBUV_LIBRARIES uv_a)

add_executable( win32uv WIN32 src/main.c src/win32uv.c src/win32uv.h src/win32uv_window.c src/win32uv_window.h src/win32uv_timers.c src/win32uv_timers.h src/main.manifest )
target_include_directories( win32uv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src )
target_link_libraries( win32uv ${LIBUV_LIBRARIES} )
