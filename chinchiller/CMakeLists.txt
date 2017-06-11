cmake_minimum_required(VERSION 3.8)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

project (chinchiller)

include(cmake/glob_code.cmake)

glob_code (${CMAKE_CURRENT_LIST_DIR}/include source)
glob_code (${CMAKE_CURRENT_LIST_DIR}/src source)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

add_avr_executable (chinchiller ${source})